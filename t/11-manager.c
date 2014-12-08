#include "test.h"
#include <pthread.h>

TESTS {
	subtest { /* basic login/logout */
		char *s;

		manager_t m = {0};
		pool_init(&m.all.users,     100000, sizeof(user_t),    user_reset);
		pool_init(&m.all.svcs,         100, sizeof(svc_t),     svc_reset);
		pool_init(&m.all.sessions,  100100, sizeof(session_t), session_reset);
		pool_init(&m.all.channels,   30000, sizeof(channel_t), channel_reset);
		pool_init(&m.all.members,  1200000, sizeof(member_t),  member_reset);
		strncpy(m.motd_file, "/etc/motd", MAX_PATH);
		m.zmq = zmq_ctx_new();

		pthread_t tid;
		is_int(pthread_create(&tid, NULL, manager_thread, &m), 0,
			"spawned manager thread");

		void *zocket = zmq_socket(m.zmq, ZMQ_DEALER);
		is_int(zmq_connect(zocket, MANAGER_ENDPOINT), 0,
			"connected to manager thread");
		sleep_ms(150);

		pdu_t *q, *a;

		/**   successful login   **********************************/
		q = pdu_make(".login", 3,
			"test!user@host.tld",
			"letmein",
			"+isw");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.login] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.login]");
		is_string(pdu_type(a), ".ok", "manager accepted our [.login]");

		/**   repeat login   **************************************/
		q = pdu_make(".login", 3,
			"test!user@host.tld",
			"letmein",
			"+isw");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.login] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.login]");
		is_string(pdu_type(a), ".error", "manager rejected our [.login]");
		is_string(s = pdu_string(a, 1), "E401", "second login is E401"); free(s);
		is_string(s = pdu_string(a, 2), "already authenticated",
			"human-friendly authfail message"); free(s);

		/**   unsuccessful login   ********************************/
		q = pdu_make(".login", 3,
			"test!badpass@host.tld",
			"incorrect-password",
			"+isw");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.login] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.login]");
		is_string(pdu_type(a), ".error", "manager rejected our [.login]");
		is_string(s = pdu_string(a, 1), "E403", "failed login is an E403"); free(s);
		is_string(s = pdu_string(a, 2), "authentication failed",
			"human-friendly authfail message"); free(s);

		/**   invalid nickname   **********************************/
		q = pdu_make(".login", 3,
			"@invalidnick",
			"password",
			"+isw");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.login] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.login]");
		is_string(pdu_type(a), ".error", "manager rejected our [.login]");
		is_string(s = pdu_string(a, 1), "E402", "failed login is an E403"); free(s);
		is_string(s = pdu_string(a, 2), "invalid username",
			"human-friendly authfail message"); free(s);

		/**   logout / login   ************************************/
		q = pdu_make(".logout", 1,
			"test!user@host.tld");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.logout] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.logout]");
		is_string(pdu_type(a), ".ok", "manager processed our [.logout]");

		q = pdu_make(".login", 3,
			"test!user@host.tld",
			"password",
			"+isw");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.login] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.login]");
		is_string(pdu_type(a), ".ok", "manager accepted second [.login]");

		void *ret;
		pthread_cancel(tid);
		pthread_join(tid, &ret);
	}

	subtest { /* capacity testing */
		char *s;

		manager_t m = {0};
		pool_init(&m.all.users,    1, sizeof(user_t),    user_reset);
		pool_init(&m.all.svcs,     1, sizeof(svc_t),     svc_reset);
		pool_init(&m.all.sessions, 2, sizeof(session_t), session_reset);
		pool_init(&m.all.channels, 1, sizeof(channel_t), channel_reset);
		pool_init(&m.all.members,  1, sizeof(member_t),  member_reset);
		strncpy(m.motd_file, "/etc/motd", MAX_PATH);
		m.zmq = zmq_ctx_new();

		pthread_t tid;
		is_int(pthread_create(&tid, NULL, manager_thread, &m), 0,
			"spawned manager thread");

		void *zocket = zmq_socket(m.zmq, ZMQ_DEALER);
		is_int(zmq_connect(zocket, MANAGER_ENDPOINT), 0,
			"connected to manager thread");
		sleep_ms(150);

		pdu_t *q, *a;

		/**   successful login   **********************************/
		q = pdu_make(".login", 3,
			"test!user@host.tld",
			"letmein",
			"+isw");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.login] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.login]");
		is_string(pdu_type(a), ".ok", "manager accepted our [.login]");

		/**   overloaded login   **********************************/
		q = pdu_make(".login", 3,
			"test!user2@host.tld",
			"letmein",
			"+isw");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.login] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.login]");
		is_string(pdu_type(a), ".tempfail", "manager rejected our [.login]");
		is_string(s = pdu_string(a, 1), "E501", "overloaded server is E501"); free(s);
		is_string(s = pdu_string(a, 2), "server overloaded",
			"human-friendly overload message"); free(s);

		void *ret;
		pthread_cancel(tid);
		pthread_join(tid, &ret);
	}

	done_testing();
}
