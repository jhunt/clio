#include "manager.h"
#include <assert.h>
#include <string.h>

#define DISPATCHER(what) static pdu_t* _dispatch_ ## what (manager_t *m, pdu_t *q)
#define DISPATCH(a, fn) do { \
	if (!a && strcmp(pdu_type(q), "." #fn) == 0) a = _dispatch_ ## fn (m,q); \
} while (0)

DISPATCHER(netsum) {
	return pdu_reply(q, ".netsum", 8,
		"1",  /* global users */
		"1",  /* global services */
		"2",  /* servers */
		"3",  /* opers */
		"5",  /* unknowns */
		"8",  /* channels */
		"13", /* local users */
		"21"  /* local servers */
	);
}

DISPATCHER(login) {
	char *ident = pdu_string(q, 1);
	user_t *u = hash_get(&m->users, ident);
	if (u) {
		free(ident);
		return pdu_reply(q, ".error", 2,
				"E401", "already authenticated");
	}

	u = pool_acq(&m->all.users);
	if (!u) {
		free(ident);
		return pdu_reply(q, ".tempfail", 2,
				"E501", "server overloaded");
	}

	if (!user_parse(ident, u)) {
		free(ident);
		pool_rel(&m->all.users, u);
		return pdu_reply(q, ".error", 2,
				"E402", "invalid username");
	}

	/* FIXME: implement *real* authentication */
	if (strcmp(u->user, "badpass") == 0) {
		free(ident);
		pool_rel(&m->all.users, u);
		return pdu_reply(q, ".error", 2,
				"E403", "authentication failed");
	}

	char *mode = pdu_string(q, 3);
	u->mode = umode_f(0, mode); free(mode);
	strncpy(u->addr, "0.0.0.0", MAX_USER_ADDR);
	u->last_active = time_ms();

	m->count.users++;
	hash_set(&m->users, ident, u);
	free(ident);
	return pdu_reply(q, ".ok", 0);
}

DISPATCHER(logout) {
	char *ident = pdu_string(q, 1);

	hash_set(&m->users, ident, NULL);
	free(ident);
	return pdu_reply(q, ".ok", 0);
}

DISPATCHER(usermod) {
	char *ident = pdu_string(q, 1);
	user_t *u = hash_get(&m->users, ident);
	if (!u) {
		free(ident);
		return pdu_reply(q, ".error", 2,
				"E405", "no such user");
	}


	char *field, *value;
	size_t n = 2;
	for (;;) {
		field = pdu_string(q, n++);
		if (!field) break;

		value = pdu_string(q, n++);
		if (value) {
			if (strcmp(field, "mode") == 0) {
				u->mode = umode_f(u->mode, value);

			} else if (strcmp(field, "addr") == 0) {
				strncpy(u->addr, value, MAX_USER_ADDR);

			} else if (strcmp(field, "away") == 0) {
				free(u->away);
				u->away = value;
				value = NULL;
			}

			/* skip unknown field names */

			free(value);
		}
		free(field);
	}

	free(ident);
	return pdu_reply(q, ".ok", 0);
}

DISPATCHER(userinfo) {
	char *ident = pdu_string(q, 1);
	user_t *u = hash_get(&m->users, ident);
	if (!u) {
		free(ident);
		return pdu_reply(q, ".error", 2,
				"E405", "no such user");
	}

	char *ts = string("%li", u->last_active);
	pdu_t *a = pdu_reply(q, ".user", 5,
		u->handle,
		u->addr,
		umode_s(u->mode),
		ts,
		u->away ? u->away : "");

	free(ts);
	free(ident);
	return a;
}

DISPATCHER(userping) {
	char *ident = pdu_string(q, 1);
	user_t *u = hash_get(&m->users, ident);
	if (!u) {
		free(ident);
		return pdu_reply(q, ".error", 2,
				"E405", "no such user");
	}

	u->last_active = time_ms();
	free(ident);
	return pdu_reply(q, ".ok", 0);
}

DISPATCHER(chanadd) {
	char *name = pdu_string(q, 1);
	channel_t *c = hash_get(&m->channels, name);
	if (c) {
		free(name);
		return pdu_reply(q, ".error", 2,
				"E406", "channel exists");
	}

	c = pool_acq(&m->all.channels);
	if (!c) {
		free(name);
		return pdu_reply(q, ".tempfail", 2,
				"E501", "server overloaded");
	}

	if (!channame_valid(name)) {
		free(name);
		pool_rel(&m->all.channels, c);
		return pdu_reply(q, ".error", 2,
				"E402", "invalid channel name");
	}

	strncpy(c->name, name, MAX_CHAN_NAME);
	c->type = c->name[0];

	char *mode = pdu_string(q, 3);
	c->mode = cmode_f(0, mode); free(mode);

	hash_set(&m->channels, name, c);

	free(name);
	return pdu_reply(q, ".ok", 0);
}

DISPATCHER(chaninfo) {
	char *name = pdu_string(q, 1);
	channel_t *c = hash_get(&m->channels, name);
	if (!c) {
		free(name);
		return pdu_reply(q, ".error", 2,
				"E405", "no such channel");
	}

	pdu_t *a = pdu_reply(q, ".chan", 6,
		name,
		cmode_s(c->mode),
		"", /* topic */
		"", /* key */
		"", /* userlim */
		""); /* user list */
	free(name);
	return a;
}

void* manager_thread(void *m_)
{
	int rc;
	manager_t *m = (manager_t*)m_;

	m->zrouter = zmq_socket(m->zmq, ZMQ_ROUTER);
	assert(m->zrouter);
	rc = zmq_bind(m->zrouter, MANAGER_ENDPOINT);
	assert(rc == 0);

	m->zclients = zmq_socket(m->zmq, ZMQ_PUB);
	assert(m->zclients);
	rc = zmq_bind(m->zclients, CLIENTS_ENDPOINT);
	assert(rc == 0);

	m->zpeers = zmq_socket(m->zmq, ZMQ_PUB);
	assert(m->zpeers);
	rc = zmq_bind(m->zpeers, PEERS_ENDPOINT);
	assert(rc == 0);

	for (;;) {
		pdu_t *q = pdu_recv(m->zrouter),
		      *a = NULL;
		if (!q) break;

		if (pdu_type(q)) {
			DISPATCH(a, netsum);
			DISPATCH(a, login);
			DISPATCH(a, logout);
			DISPATCH(a, usermod);
			DISPATCH(a, userinfo);
			DISPATCH(a, userping);

			DISPATCH(a, chanadd);
			DISPATCH(a, chaninfo);
		}

		if (!a) a = pdu_reply(q, ".error", 1, "unknown query type");
		pdu_free(q);
		pdu_send_and_free(a, m->zrouter);
	}

	return NULL; /* LCOV_EXCL_LINE */
}
