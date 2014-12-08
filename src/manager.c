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

	m->count.users++;
	hash_set(&m->users, ident, u);
	free(ident);
	return pdu_reply(q, ".ok", 0);
}

DISPATCHER(logout) {
	char *nick = pdu_string(q, 1);

	hash_set(&m->users, nick, NULL);
	return pdu_reply(q, ".ok", 0);
}

DISPATCHER(usermod) {
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

	pdu_t *a = pdu_reply(q, ".user", 4,
		ident,
		"0.0.0.0",
		umode_s(u->mode),
		"");

	free(ident);
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
		}

		if (!a) a = pdu_reply(q, ".error", 1, "unknown query type");
		pdu_free(q);
		pdu_send_and_free(a, m->zrouter);
	}

	return NULL; /* LCOV_EXCL_LINE */
}
