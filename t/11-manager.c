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
		is_string(s = pdu_string(a, 1), "E402", "failed login is an E402"); free(s);
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

	subtest { /* userinfo */
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

		/**   failed userinfo   ***********************************/
		q = pdu_make(".userinfo", 1,
			"test!user@host.tld");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.userinfo] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.userinfo]");
		is_string(pdu_type(a), ".error", "manager rejected our [.userinfo]");
		is_string(s = pdu_string(a, 1), "E405", "no user == E405"); free(s);
		is_string(s = pdu_string(a, 2), "no such user",
			"human-friendly failure message"); free(s);

		/**   successful login   **********************************/
		q = pdu_make(".login", 3,
			"test!user@host.tld",
			"letmein",
			"+wsi");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.login] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.login]");
		is_string(pdu_type(a), ".ok", "manager accepted our [.login]");

		q = pdu_make(".userinfo", 1,
			"test!user@host.tld");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.userinfo] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.userinfo]");
		is_string(pdu_type(a), ".user", "manager accepted our [.userinfo]");
		is_string(s = pdu_string(a, 1), "test!user@host.tld",
			"user identity returned as .userinfo[0]"); free(s);
		is_string(s = pdu_string(a, 2), "0.0.0.0",
			"default IP address returned as .userinfo[1]"); free(s);
		is_string(s = pdu_string(a, 3), "+isw",
			"mode string returned as .userinfo[2]"); free(s);
		is_string(s = pdu_string(a, 4), "",
			"no away string (.userinfo[3])"); free(s);

		/**   second login   **************************************/
		q = pdu_make(".login", 3,
			"test!user2@host.tld",
			"letmein",
			"+ws");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.login] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.login]");
		is_string(pdu_type(a), ".ok", "manager accepted our [.login]");

		q = pdu_make(".userinfo", 1,
			"test!user2@host.tld");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.userinfo] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.userinfo]");
		is_string(pdu_type(a), ".user", "manager accepted our [.userinfo]");
		is_string(s = pdu_string(a, 1), "test!user2@host.tld",
			"user identity returned as .userinfo[0]"); free(s);
		is_string(s = pdu_string(a, 2), "0.0.0.0",
			"default IP address returned as .userinfo[1]"); free(s);
		is_string(s = pdu_string(a, 3), "+sw",
			"mode string returned as .userinfo[2]"); free(s);
		is_string(s = pdu_string(a, 4), "",
			"no away string (.userinfo[3])"); free(s);

		void *ret;
		pthread_cancel(tid);
		pthread_join(tid, &ret);
	}

	subtest { /* user updates */
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
			"+wsi");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.login] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.login]");
		is_string(pdu_type(a), ".ok", "manager accepted our [.login]");

		q = pdu_make(".userinfo", 1,
			"test!user@host.tld");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.userinfo] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.userinfo]");
		is_string(pdu_type(a), ".user", "manager accepted our [.userinfo]");
		is_string(s = pdu_string(a, 1), "test!user@host.tld",
			"user identity returned as .userinfo[0]"); free(s);
		is_string(s = pdu_string(a, 2), "0.0.0.0",
			"default IP address returned as .userinfo[1]"); free(s);
		is_string(s = pdu_string(a, 3), "+isw",
			"mode string returned as .userinfo[2]"); free(s);
		is_string(s = pdu_string(a, 4), "",
			"no away string (.userinfo[3])"); free(s);

		/**   failed usermod (no such user)   *********************/

		q = pdu_make(".usermod", 5,
			"test!NONEXISTENT@host.tld",
			"mode", "+o-si",
			"addr", "1.2.3.4");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.usermode] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.usermod]");
		is_string(pdu_type(a), ".error", "manager rejected our [.usermod]");
		is_string(s = pdu_string(a, 1), "E405", "no user == E405"); free(s);
		is_string(s = pdu_string(a, 2), "no such user",
			"human-friendly failure message"); free(s);

		/**   successful usermod   ********************************/

		q = pdu_make(".usermod", 5,
			"test!user@host.tld",
			"mode", "+o-si",
			"addr", "1.2.3.4");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.usermode] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.usermod]");
		is_string(pdu_type(a), ".ok", "manager accepted our [.usermod]");

		q = pdu_make(".userinfo", 1,
			"test!user@host.tld");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.userinfo] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.userinfo]");
		is_string(pdu_type(a), ".user", "manager accepted our [.userinfo]");
		is_string(s = pdu_string(a, 1), "test!user@host.tld",
			"user identity returned as .userinfo[0]"); free(s);
		is_string(s = pdu_string(a, 2), "1.2.3.4",
			"updated IP address returned as .userinfo[1]"); free(s);
		is_string(s = pdu_string(a, 3), "+ow",
			"updated mode string returned as .userinfo[2]"); free(s);

		/**   another successful usermod   ************************/

		q = pdu_make(".usermod", 5,
			"test!user@host.tld",
			"mode", "+a",
			"away", "not here right now",
			"FOO",  "not a thing");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.usermode] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.usermod]");
		is_string(pdu_type(a), ".ok", "manager accepted our [.usermod]");

		q = pdu_make(".userinfo", 1,
			"test!user@host.tld");
		is_int(pdu_send_and_free(q, zocket), 0,
			"sent [.userinfo] to manager thread");
		a = pdu_recv(zocket);
		isnt_null(a, "received response to our [.userinfo]");
		is_string(pdu_type(a), ".user", "manager accepted our [.userinfo]");
		is_string(s = pdu_string(a, 1), "test!user@host.tld",
			"user identity returned as .userinfo[0]"); free(s);
		is_string(s = pdu_string(a, 3), "+aow",
			"updated mode string returned as .userinfo[2]"); free(s);
		is_string(s = pdu_string(a, 4), "not here right now",
			"updated away string returned as .userinfo[3]"); free(s);

		void *ret;
		pthread_cancel(tid);
		pthread_join(tid, &ret);
	}

	done_testing();
}
