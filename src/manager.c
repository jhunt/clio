#include "manager.h"
#include <assert.h>
#include <string.h>

void* manager_thread(void *m_)
{
	int rc;
	manager_t *m = (manager_t*)m_;

	void *router = zmq_socket(m->zmq, ZMQ_ROUTER);
	assert(router);
	rc = zmq_bind(router, MANAGER_ENDPOINT);
	assert(rc == 0);

	void *client_pub = zmq_socket(m->zmq, ZMQ_PUB);
	assert(client_pub);
	rc = zmq_bind(client_pub, CLIENTS_ENDPOINT);
	assert(rc == 0);

	void *peer_pub = zmq_socket(m->zmq, ZMQ_PUB);
	assert(peer_pub);
	rc = zmq_bind(peer_pub, PEERS_ENDPOINT);
	assert(rc == 0);

	for (;;) {
		pdu_t *q = pdu_recv(router), *a;
		if (!q) break;

		fprintf(stderr, "received a <%s> query\n", pdu_type(q));
		if (strcmp(pdu_type(q), ".lusers") == 0) {
			a = pdu_reply(q, ".ok", 8, "1", "1", "2", "3",
			                       "5", "8", "13", "21");

		} else if (strcmp(pdu_type(q), ".register") == 0) {
			char *nick = pdu_string(q, 1);

			user_t *who = hash_get(&m->users, nick);
			if (who) {
				free(nick);
				a = pdu_reply(q, ".conflict", 0);
				goto respond;
			}

			char *user = pdu_string(q, 2);
			char *host = pdu_string(q, 3);
			char *pass = pdu_string(q, 4);

			/* FIXME: actually authenticate the u@h:p */
			if (0) {
				free(user); free(host); free(pass);
				a = pdu_reply(q, ".authnfail", 0);
				goto respond;
			}

			who = pool_acq(&m->all.users);
			if (!who) {
				a = pdu_reply(q, ".capfail", 0);
				goto respond;
			}

			strncpy(who->nick, nick, MAX_NICK);
			strncpy(who->user, user, MAX_USER_NAME);
			strncpy(who->host, host, MAX_USER_HOST);

			m->count.users++;
			hash_set(&m->users, nick, who);
			a = pdu_reply(q, ".ok", 0);

		} else if (strcmp(pdu_type(q), ".getmode") == 0) {
			a = pdu_reply(q, ".error", 1, "UNIMPLEMENTED");

		} else if (strcmp(pdu_type(q), ".setmode") == 0) {
			a = pdu_reply(q, ".error", 1, "UNIMPLEMENTED");

		} else {
			a = pdu_reply(q, ".error", 1, "unknown query type");
		}

respond:
		pdu_free(q);
		pdu_send_and_free(a, router);
	}

	return NULL;
}
