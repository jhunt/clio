#include "manager.h"
#include <assert.h>
#include <string.h>

#define DISPATCHER(what) static pdu_t* _dispatch_ ## what (manager_t *m, pdu_t *q)
#define DISPATCH(fn) do {                \
	if (strcmp(pdu_type(q), "." #fn) == 0) {      \
		pdu_t *a = _dispatch_ ## fn (m,q);  \
		pdu_free(q);                        \
		pdu_send_and_free(a, m->zrouter);    \
	} } while (0)

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
	char *nick = pdu_string(q, 1);

	user_t *who = hash_get(&m->users, nick);
	if (who) {
		free(nick);
		return pdu_reply(q, ".conflict", 0);
	}

	char *user = pdu_string(q, 2);
	char *host = pdu_string(q, 3);
	char *pass = pdu_string(q, 4);

	/* FIXME: actually authenticate the u@h:p */
	if (0) {
		free(user); free(host); free(pass);
		return pdu_reply(q, ".authnfail", 0);
	}

	who = pool_acq(&m->all.users);
	if (!who) {
		free(user); free(host); free(pass);
		return pdu_reply(q, ".capfail", 0);
	}

	strncpy(who->nick, nick, MAX_NICK);
	strncpy(who->user, user, MAX_USER_NAME);
	strncpy(who->host, host, MAX_USER_HOST);

	m->count.users++;
	hash_set(&m->users, nick, who);
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
		pdu_t *q = pdu_recv(m->zrouter);
		if (!q) break;

		fprintf(stderr, "received a <%s> query\n", pdu_type(q));
		DISPATCH(netsum);
		DISPATCH(login);
		DISPATCH(logout);
		DISPATCH(usermod);

		pdu_t *a = pdu_reply(q, ".error", 1, "unknown query type");
		pdu_free(q);
		pdu_send_and_free(a, m->zrouter);
	}

	return NULL;
}
