#include "test.h"
#include <pthread.h>

#define MANAGER_SETUP(n) \
manager_t m = {0}; \
void *zocket; \
pthread_t tid; \
const char *msg = "unspecified test"; \
do { \
	pool_init(&m.all.users,    n ? n :  100000, sizeof(user_t),    user_reset); \
	pool_init(&m.all.svcs,     n ? n :     100, sizeof(svc_t),     svc_reset); \
	pool_init(&m.all.sessions, n ? n :  100100, sizeof(session_t), session_reset); \
	pool_init(&m.all.channels, n ? n :   30000, sizeof(channel_t), channel_reset); \
	pool_init(&m.all.members,  n ? n : 1200000, sizeof(member_t),  member_reset); \
	strncpy(m.motd_file, "/etc/motd", MAX_PATH); \
	m.zmq = zmq_ctx_new(); \
\
	is_int(pthread_create(&tid, NULL, manager_thread, &m), 0, \
		"spawned manager thread"); \
\
	zocket = zmq_socket(m.zmq, ZMQ_DEALER); \
	is_int(zmq_connect(zocket, MANAGER_ENDPOINT), 0, \
		"connected to manager thread"); \
	sleep_ms(150); \
} while (0)

#define MANAGER_TEARDOWN do { \
	void *ret; \
	pthread_cancel(tid); \
	pthread_join(tid, &ret); \
} while (0)

#define OK_SEND(m, pdu) \
msg = m; \
do { \
	q = (pdu); \
	is_int(pdu_send_and_free(q, zocket), 0, "%s: sent to manager", msg); \
	a = pdu_recv(zocket); \
	isnt_null(a, "%s: received response from manager", msg); \
} while (0)

#define REPLY_IS(t) is_string(pdu_type(a), t, "%s: manager replied [%s]", msg, t)
#define REPLY_X(n,f,v) is_string(s = pdu_string(a, n), v, "%s: %s in reply[%i]", msg, f, n)

#define REPLY_OK REPLY_IS(".ok")

#define REPLY_ERROR(e,h) do { \
	REPLY_IS(".error"); \
	is_string(s = pdu_string(a, 1), e, "%s: error #%s",    msg, e); free(s); \
	is_string(s = pdu_string(a, 2), h, "%s: message '%s'", msg, h); free(s); \
} while (0)

#define REPLY_TEMPFAIL(e,h) do { \
	REPLY_IS(".tempfail"); \
	is_string(s = pdu_string(a, 1), e, "%s: error #%s",    msg, e); free(s); \
	is_string(s = pdu_string(a, 2), h, "%s: message '%s'", msg, h); free(s); \
} while (0)



TESTS {
	subtest { /* basic login/logout */
		MANAGER_SETUP(0);

		char *s;
		pdu_t *q, *a;

		/**   successful login   **********************************/
		OK_SEND("successful .login",
			pdu_make(".login", 3,
				"test!user@host.tld",
				"letmein",
				"+isw"));
		REPLY_OK;

		/**   repeat login   **************************************/
		OK_SEND("repeat .login",
			pdu_make(".login", 3,
				"test!user@host.tld",
				"letmein",
				"+isw"));
		REPLY_ERROR("E401", "already authenticated");

		/**   unsuccessful login   ********************************/
		OK_SEND("failed .login",
			pdu_make(".login", 3,
				"test!badpass@host.tld",
				"incorrect-password",
				"+isw"));
		REPLY_ERROR("E403", "authentication failed");

		/**   invalid nickname   **********************************/
		OK_SEND("bad nickname",
			pdu_make(".login", 3,
				"@invalidnick",
				"password",
				"+isw"));
		REPLY_ERROR("E402", "invalid username");

		/**   logout / login   ************************************/
		OK_SEND(".logout",
			pdu_make(".logout", 1,
				"test!user@host.tld"));
		REPLY_OK;

		OK_SEND(".login again",
			pdu_make(".login", 3,
				"test!user@host.tld",
				"password",
				"+isw"));
		REPLY_OK;

		MANAGER_TEARDOWN;
	}

	subtest { /* userinfo */
		MANAGER_SETUP(0);

		char *s;
		pdu_t *q, *a;

		/**   failed userinfo   ***********************************/
		OK_SEND("failed .userinfo",
			pdu_make(".userinfo", 1,
				"test!user@host.tld"));
		REPLY_ERROR("E405", "no such user");

		/**   successful login   **********************************/
		OK_SEND("successful .login",
			pdu_make(".login", 3,
				"test!user@host.tld",
				"letmein",
				"+wsi"));
		REPLY_OK;

		OK_SEND(".userinfo",
			pdu_make(".userinfo", 1,
				"test!user@host.tld"));
		REPLY_IS(".user");
		REPLY_X(1, "identity", "test!user@host.tld");
		REPLY_X(2, "ip",       "0.0.0.0");
		REPLY_X(3, "mode",     "+isw");
		/* no segment 4? */
		REPLY_X(5, "away msg", "");

		/**   second login   **************************************/
		OK_SEND("second .login",
			pdu_make(".login", 3,
				"test!user2@host.tld",
				"letmein",
				"+ws"));
		REPLY_OK;

		OK_SEND(".userinfo",
			pdu_make(".userinfo", 1,
				"test!user2@host.tld"));
		REPLY_IS(".user");
		REPLY_X(1, "identity", "test!user2@host.tld");
		REPLY_X(2, "ip",       "0.0.0.0");
		REPLY_X(3, "mode",     "+sw");
		/* no segment 4? */
		REPLY_X(5, "away msg", "");

		MANAGER_TEARDOWN;
	}

	subtest { /* user updates */
		MANAGER_SETUP(0);

		char *s;
		pdu_t *q, *a;

		/**   successful login   **********************************/
		OK_SEND(".login",
			pdu_make(".login", 3,
				"test!user@host.tld",
				"letmein",
				"+wsi"));
		REPLY_OK;

		OK_SEND(".login",
			pdu_make(".userinfo", 1,
				"test!user@host.tld"));
		REPLY_IS(".user");
		REPLY_X(1, "identity", "test!user@host.tld");
		REPLY_X(2, "ip",       "0.0.0.0");
		REPLY_X(3, "mode",     "+isw");
		REPLY_X(5, "away msg", "");

		/**   failed usermod (no such user)   *********************/

		OK_SEND("failed .usermod",
			pdu_make(".usermod", 5,
				"test!NONEXISTENT@host.tld",
				"mode", "+o-si",
				"addr", "1.2.3.4"));
		REPLY_ERROR("E405", "no such user");

		/**   successful usermod   ********************************/

		OK_SEND(".usermod",
			pdu_make(".usermod", 5,
				"test!user@host.tld",
				"mode", "+o-si",
				"addr", "1.2.3.4"));
		REPLY_OK;

		OK_SEND(".userinfo",
			pdu_make(".userinfo", 1,
				"test!user@host.tld"));
		REPLY_IS(".user");
		REPLY_X(1, "identity",     "test!user@host.tld");
		REPLY_X(2, "updated ip",   "1.2.3.4");
		REPLY_X(3, "updated mode", "+ow");

		/**   another successful usermod   ************************/

		OK_SEND("another .usermod",
			pdu_make(".usermod", 5,
				"test!user@host.tld",
				"mode", "+a",
				"away", "not here right now",
				"FOO",  "not a thing"));
		REPLY_OK;

		OK_SEND(".userinfo",
			pdu_make(".userinfo", 1,
				"test!user@host.tld"));
		REPLY_IS(".user");
		REPLY_X(1, "identity",         "test!user@host.tld");
		REPLY_X(3, "updated mode",     "+aow");
		REPLY_X(5, "updated away msg", "not here right now");

		MANAGER_TEARDOWN;
	}

	subtest { /* user ping */
		MANAGER_SETUP(0);

		char *s;
		pdu_t *q, *a;

		/**   failed ping (non-existent user)   *******************/
		OK_SEND("failed .userping",
			pdu_make(".userping", 1,
				"test!NONEXISTENT@host.tld"));
		REPLY_ERROR("E405", "no such user");

		/**   successful login   **********************************/
		OK_SEND(".login [t1]",
			pdu_make(".login", 3,
				"test!user@host.tld",
				"letmein",
				"+wsi"));
		REPLY_OK;

		OK_SEND(".userinfo",
			pdu_make(".userinfo", 1,
				"test!user@host.tld"));
		REPLY_IS(".user");
		REPLY_X(1, "identity", "test!user@host.tld");

		char *t1 = pdu_string(a, 4);
		ok(strlen(t1) >= 10, "user last-active timestamp found at .userinfo[2]");

		sleep_ms(100); /* milliseconds */

		OK_SEND(".userping [t2]",
			pdu_make(".userping", 1,
				"test!user@host.tld"));
		REPLY_OK;

		OK_SEND(".userinfo [t2]",
			pdu_make(".userinfo", 1,
				"test!user@host.tld"));
		REPLY_IS(".user");
		REPLY_X(1, "identity", "test!user@host.tld");

		char *t2 = pdu_string(a, 4);
		ok(strlen(t2) >= 10, "user last-active timestamp found at .userinfo[2]");
		isnt_string(t2, t1, ".userping updated our timestamp");

		free(t1);
		free(t2);

		MANAGER_TEARDOWN;
	}

	subtest { /* user capacity testing */
		MANAGER_SETUP(1);

		char *s;
		pdu_t *q, *a;

		/**   successful login   **********************************/
		OK_SEND(".login",
			pdu_make(".login", 3,
				"test!user@host.tld",
				"letmein",
				"+isw"));
		REPLY_OK;

		/**   overloaded login   **********************************/
		OK_SEND(".login (over load)",
			pdu_make(".login", 3,
				"test!user2@host.tld",
				"letmein",
				"+isw"));
		REPLY_TEMPFAIL("E501", "server overloaded");

		/**   logout first   **************************************/
		OK_SEND(".logout to free slot",
			pdu_make(".logout", 1,
				"test!user@host.tld",
				"letmein",
				"+isw"));
		REPLY_OK;

		/**   successful login   **********************************/
		OK_SEND(".login again",
			pdu_make(".login", 3,
				"test!user@host.tld",
				"letmein",
				"+isw"));
		REPLY_OK;
	}

	/*******************************************************************/
	/*******************************************************************/

	subtest { /* channel add */
		MANAGER_SETUP(0);

		char *s;
		pdu_t *q, *a;

		/**   no such channel   ***********************************/
		OK_SEND("failed .chaninfo",
			pdu_make(".chaninfo", 1,
				"#testchan"));
		REPLY_ERROR("E405", "no such channel");

		/**   bad channel name   *********************************/
		OK_SEND(".chanadd with bad channel name",
			pdu_make(".chanadd", 5,
				"FOO",
				"nick!user@host.tld",
				"+m",
				"password!",
				"50"));
		REPLY_ERROR("E402", "invalid channel name");

		/**   add channel   **************************************/
		OK_SEND("successful .chanadd",
			pdu_make(".chanadd", 5,
				"#testchan",
				"nick!user@host.tld",
				"+slm",
				"",
				"50"));
		REPLY_OK;

		OK_SEND(".chaninfo for newly added channel",
			pdu_make(".chaninfo", 1,
				"#testchan"));
		REPLY_IS(".chan");
		REPLY_X(1, "chan name", "#testchan");
		REPLY_X(2, "chan mode", "+sml");
		REPLY_X(3, "topic",     "");
		REPLY_X(4, "password",  "");
		REPLY_X(5, "userlim",   ""); /* FIXME! */
		REPLY_X(6, "user list", "");

		/**   double-add   ***************************************/
		OK_SEND("double .chanadd",
			pdu_make(".chanadd", 5,
				"#testchan",
				"nick!user@host.tld",
				"+s",
				"",
				"50"));
		REPLY_ERROR("E406", "channel exists");

		/**   del channel   **************************************/
		OK_SEND(".chandel",
			pdu_make(".chandel", 1,
				"#testchan"));
		REPLY_OK;

		OK_SEND(".chaninfo for deleted channel",
			pdu_make(".chaninfo", 1,
				"#testchan"));
		REPLY_ERROR("E405", "no such channel");

		/**   re-add channel   ***********************************/
		OK_SEND(".chanadd for re-add",
			pdu_make(".chanadd", 5,
				"#testchan",
				"nick!user@host.tld",
				"+stkm",
				"secrets",
				"100"));
		REPLY_OK;

		OK_SEND(".chaninfo",
			pdu_make(".chaninfo", 1,
				"#testchan"));
		REPLY_IS(".chan");
		REPLY_X(1, "chan name", "#testchan");
		REPLY_X(2, "chan mode", "+stmk");
		REPLY_X(3, "topic",     "");
		REPLY_X(4, "password",  "secrets");
		REPLY_X(5, "userlim",   ""); /* FIXME! */
		REPLY_X(6, "user list", "");

		MANAGER_TEARDOWN;
	}

	subtest { /* channel modification */
		MANAGER_SETUP(1);

		MANAGER_TEARDOWN;
	}

	subtest { /* channel capacity testing */
		MANAGER_SETUP(1);

		char *s;
		pdu_t *q, *a;

		/**   chanadd   *******************************************/
		OK_SEND(".chanadd",
			pdu_make(".chanadd", 5,
				"#testchan",
				"nick!user@host.tld",
				"+slm",
				"",
				"50"));
		REPLY_OK;

		/**   overloaded chan add   *******************************/
		OK_SEND(".chanadd (over load)",
			pdu_make(".chanadd", 5,
				"#other",
				"nick!user@host.tld",
				"+slm",
				"",
				"50"));
		REPLY_TEMPFAIL("E501", "server overloaded");

		/**   now remove it   *************************************/
		OK_SEND(".chandel",
			pdu_make(".chandel", 1,
				"#testchan"));
		REPLY_OK;

		/**   re-add the channel   ********************************/
		OK_SEND(".chanadd",
			pdu_make(".chanadd", 5,
				"#testchan",
				"nick!user@host.tld",
				"+slm",
				"",
				"50"));
		REPLY_OK;

		MANAGER_TEARDOWN;
	}

	done_testing();
}
