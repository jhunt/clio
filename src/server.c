#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <netinet/ip.h>
#include <vigor.h>
#include "irc.h"

#define CLIENT_RECV_BUF 8192

typedef struct {
	char *name;
	char *network;
} my_ident_t;

typedef struct {
	void *zmq;
	my_ident_t *self;

	struct {
		pool_t  users;
		pool_t  svcs;
		pool_t  sessions;
		pool_t  channels;
		pool_t  members;
	} all;

	struct {
		int users;
		int svcs;
		int sessions;
		int channels;
	} count;

	hash_t  users;
	hash_t  svcs;
	hash_t  sessions;
	hash_t  channels;

	char motd_file[MAX_PATH];
} manager_t;

typedef struct {
	void *zmq;
	my_ident_t *self;

	int fd;
	void *sub;
	void *dealer;

	int registered;
	char *nick;
	char *username;
	char *host;
	char *realname;
	char *password;

	int32_t deadline;
} client_handler_t;

typedef struct {
	void *zmq;
	my_ident_t *self;
} peer_handler_t;

static void creply(client_handler_t *c, int num, char *msg)
{
	reply(c->fd, string(":%s %03i %s %s\r\n", c->self->name, num, c->nick, msg));
	free(msg);
}
static void cerror(client_handler_t *c, char *msg)
{
	reply(c->fd, string("ERROR %s\r\n", msg));
	free(msg);
}
static void cnotice(client_handler_t *c, char *msg)
{
	reply(c->fd, string("NOTICE %s\r\n", msg));
	free(msg);
}

#define _s(x) ((x) ? (int)(strlen(x)) : 0), ((x) ? (x) : "")
static void s_dump_msg(msg_t *m)
{
	if (!m) {
		fprintf(stderr, "<null message>\n");
		return;
	}

	fprintf(stderr, "MSG %p:\n", m);
	fprintf(stderr, "  pre: %4i [%s]\n", _s(m->prefix));
	fprintf(stderr, "  cmd: %4i [%s]\n", _s(m->command));
	int i;
	for (i = 0; i < 10; i++)
		if (m->args[i]) fprintf(stderr, "   $%i: %4i [%s]\n", i, _s(m->args[i]));
	for (i = 10; i < 15; i++)
		if (m->args[i]) fprintf(stderr, "  $%i: %4i [%s]\n", i, _s(m->args[i]));
	fprintf(stderr, "\n");
}
#undef _s

static void* s_manager_thread(void *u)
{
	int rc;
	manager_t *mgr = (manager_t*)u;

	void *router = zmq_socket(mgr->zmq, ZMQ_ROUTER);
	assert(router);
	rc = zmq_bind(router, "inproc://irc.manager.1");
	assert(rc == 0);

	void *client_pub = zmq_socket(mgr->zmq, ZMQ_PUB);
	assert(client_pub);
	rc = zmq_bind(client_pub, "inproc://irc.clients.1");
	assert(rc == 0);

	void *peer_pub = zmq_socket(mgr->zmq, ZMQ_PUB);
	assert(peer_pub);
	rc = zmq_bind(peer_pub, "inproc://irc.peers.1");
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

			user_t *who = hash_get(&mgr->users, nick);
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

			who = pool_acq(&mgr->all.users);
			if (!who) {
				a = pdu_reply(q, ".capfail", 0);
				goto respond;
			}

			strncpy(who->nick, nick, MAX_NICK);
			strncpy(who->user, user, MAX_USER_NAME);
			strncpy(who->host, host, MAX_USER_HOST);

			mgr->count.users++;
			hash_set(&mgr->users, nick, who);
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

#define CMD_PASS   1
#define CMD_USER   2
#define CMD_NICK   3
#define CMD_QUIT   4
#define CMD_LUSERS 5
#define CMD_OPER   6
#define CMD_MODE   7

static char *COMMANDS[] = {
	"", /* UNKNOWN */
	"PASS",
	"USER",
	"NICK",
	"QUIT",
	"LUSERS",
	"OPER",
	"MODE",
	NULL
};

static int irc_client_command(const char *s)
{
	size_t i;
	for (i = 0; COMMANDS[i]; i++)
		if (strcmp(s, COMMANDS[i]) == 0)
			return i;
	return 0;
}

static int s_client_pass(client_handler_t *c, msg_t *m)
{
	if (c->registered) {
		creply(c, ERR_ALREADYREGISTRED,
			string(":Inappropriate command (already registered)"));
		return 0;
	}

	if (!m->args[0]) {
		creply(c, ERR_NEEDMOREPARAMS, string(":No password given"));
		return 0;
	}

	free(c->password);
	c->password = strdup(m->args[0]);
	return 1;
}

static int s_client_nick(client_handler_t *c, msg_t *m)
{
	if (!m->args[0]) {
		creply(c, ERR_NONICKNAMEGIVEN, string(":No nickname given"));
		return 0;
	}

	if (!nick_valid(m->args[0])) {
		char *oldnick = c->nick;
		c->nick = m->args[0];

		creply(c, ERR_ERRONEUSNICKNAME, string(":Invalid nickname"));
		c->nick = oldnick;
		return 0;
	}

	free(c->nick);
	c->nick = strdup(m->args[0]);
	return 1;
}

static int s_client_user(client_handler_t *c, msg_t *m)
{
	if (!m->args[3]) {
		creply(c, ERR_NEEDMOREPARAMS, string(":Not enough parameters"));
		return 0;
	}

	free(c->username);
	c->username = strdup(m->args[1]);
	free(c->realname);
	c->realname = strdup(m->args[3]);

	/* RFC 1459 clients often dupes the username in $3,
	   RFC 2812 says $3 should be a numeric bitflag.
	                             try to accomodate both */
	if (irc_isdigit(m->args[2][0])) {
		/* FIXME: process mode connect */
	}
	return 1;
}

static int s_client_register(client_handler_t *c, msg_t *m)
{
	pdu_t *q = pdu_make(".register", 4,
		c->nick, c->username, "HOSTNAME" /*c->host*/, c->password);
	int rc = pdu_send_and_free(q, c->dealer);
	assert(rc == 0);

	pdu_t *a = pdu_recv(c->dealer);
	if (strcmp(pdu_type(a), ".ok") == 0) {
		c->registered = 1;
		return 1;

	} else if (strcmp(pdu_type(a), ".conflict") == 0) {
		creply(c, ERR_NICKNAMEINUSE, string("* %s :Nickname is already in use.", c->nick));
		creply(c, ERR_NOTREGISTERED, string("* :You are not registered."));

	} else if (strcmp(pdu_type(a), ".authnfail") == 0) {
		cerror(c, string(":closing connection (authentication failed)"));
	} else {
		cerror(c, string(":closing connection (unknown error)"));
	}
	return 0;
}

static int s_client_welcome(client_handler_t *c, msg_t *m)
{
	creply(c, RPL_WELCOME,  string(":Welcome to the %s IRC Network, %s", c->self->network, c->nick));
	creply(c, RPL_YOURHOST, string(":Your host is %s, running version %s", c->self->name, SERVER_VERSION));
	creply(c, RPL_CREATED,  string(":This server was created %s", SERVER_BUILDDATE));
	creply(c, RPL_MYINFO,   string(":%s %s-%s %s %s", c->self->name,
		SERVER_SOFTWARE, SERVER_VERSION, CHAN_MODELIST, USER_MODELIST));
	return 1;
}

static int s_client_lusers(client_handler_t *c, msg_t *m)
{
	pdu_t *q = pdu_make(".lusers", 0);
	int rc = pdu_send_and_free(q, c->dealer);
	assert(rc == 0);

	pdu_t *a = pdu_recv(c->dealer);
	if (strcmp(pdu_type(a), ".ok") == 0) {
		/* 251 LUSERCLIENT */
		char *users = pdu_string(a, 1); assert(users);
		char *svcs  = pdu_string(a, 2); assert(svcs);
		char *srvrs = pdu_string(a, 3); assert(srvrs);
		/* 252 LUSEROP */
		char *opers = pdu_string(a, 4); assert(opers);
		/* 253 LUSERUNKNOWN */
		char *unkns = pdu_string(a, 5); assert(unkns);
		/* 254 LUSERCHANNELS */
		char *chans = pdu_string(a, 6); assert(chans);
		/* 255 LUSERME */
		char *luser = pdu_string(a, 7); assert(luser);
		char *lsrvr = pdu_string(a, 8); assert(lsrvr);

		creply(c, RPL_LUSERCLIENT,   string(":There are %s users and %s services on %s servers", users, svcs, srvrs));
		creply(c, RPL_LUSEROP,       string("%s :operators online", opers));
		creply(c, RPL_LUSERUNKNOWN,  string("%s :unknown connections", unkns));
		creply(c, RPL_LUSERCHANNELS, string("%s :channels", chans));
		creply(c, RPL_LUSERME,       string(":I have %s clients and %s servers", luser, lsrvr));
	}

	return 1;
}

static int s_client_quit(client_handler_t *c, msg_t *m)
{
	cerror(c, strdup(":closing connection"));
	return 1;
}

static int s_client_oper(client_handler_t *c, msg_t *m)
{
	if (!m->args[1]) {
		creply(c, ERR_NEEDMOREPARAMS, string(":Not enough parameters"));
		return 0;
	}

	/* FIXME: need real auth */
	if (strcmp(m->args[1], "secret") != 0) {
		creply(c, ERR_PASSWDMISMATCH, string(":Password incorrect"));
		return 0;
	}

	/* FIXME: need real whitelist */
	if (strcmp(m->args[0], "oper") != 0) {
		creply(c, ERR_NOOPERHOST, string(":Host not allowed to gain OPER status"));
		return 0;
	}

	/* FIXME: make an op */
	creply(c, RPL_YOUREOPER, string(":You are now an IRC operator"));
	return 1;
}

static int s_client_mode(client_handler_t *c, msg_t *m)
{
	if (!m->args[1]) {
		creply(c, ERR_NEEDMOREPARAMS, string(":Not enough parameters"));
		return 0;
	}

	pdu_t *q = pdu_make(".getmode", 1, m->args[0]);
	int rc = pdu_send_and_free(q, c->dealer);
	assert(rc == 0);

	pdu_t *a = pdu_recv(c->dealer);
	if (strcmp(pdu_type(a), ".ok") == 0) {
		uint8_t mode = atoi(pdu_string(a, 1));

		/* parse the mode string */

		q = pdu_make(".setmode", 2, m->args[0]
	}

	return 1;
}

static void* s_client_thread(void *u)
{
	int rc;
	client_handler_t *c = (client_handler_t*)u;

	c->dealer = zmq_socket(c->zmq, ZMQ_DEALER);
	assert(c->dealer);
	rc = zmq_connect(c->dealer, "inproc://irc.manager.1");
	assert(rc == 0);

	c->sub = zmq_socket(c->zmq, ZMQ_SUB);
	assert(c->sub);
	rc = zmq_setsockopt(c->sub, ZMQ_SUBSCRIBE, "", 0);
	assert(rc == 0);
	rc = zmq_connect(c->sub, "inproc://irc.clients.1");
	assert(rc == 0);

	printf("client handler started up on socket %i\n", c->fd);

	cnotice(c, string("AUTH :**** processing connection to %s ****", c->self->name));

	buffer_t *recvbuf = buffer_new(CLIENT_RECV_BUF);
	while (buffer_read(recvbuf, c->fd) > 0) {
		msg_t *m;
		while ((m = buffer_msg(recvbuf)) != NULL) {
			s_dump_msg(m);

			int cmd = irc_client_command(m->command);

			if (!c->registered) {
				switch (cmd) {
					case CMD_PASS: if (!s_client_pass(c, m)) continue; break;
					case CMD_NICK: if (!s_client_nick(c, m)) continue; break;
					case CMD_USER: if (!s_client_user(c, m)) continue; break;
					case CMD_QUIT: s_client_quit(c, m); break;
				}

				if (c->nick && c->username) {
					if (!s_client_register(c, m)) continue;
					s_client_welcome(c, m);
					s_client_lusers(c, m);
				}
			} else {

				switch (cmd) {
					case CMD_PASS:   s_client_pass(c, m); break;
					/* FIXME: NICK change */
					case CMD_USER:   s_client_user(c, m); break;
					case CMD_QUIT:   s_client_quit(c, m); break;
					case CMD_LUSERS: s_client_lusers(c, m); break;
					case CMD_OPER:   s_client_oper(c, m); break;
					case CMD_MODE:   s_client_mode(c, m); break;
					default:
						creply(c, ERR_UNKNOWNCOMMAND, string(":Unknown command"));
				}
			}
		}
	}

	printf("client handler on socket %i shutting down\n", c->fd);
	close(c->fd);
	return NULL;
}

#if 0
static void* static_peer_thread(void *u)
{
	int rc;
	peer_handler_t *p = (peer_handler_t*)u;

	void *req = zmq_socket(p->zmq, ZMQ_REQ);
	assert(req);
	rc = zmq_connect(req, "inproc://irc.manager.1");
	assert(rc == 0);

	void *sub = zmq_socket(p->zmq, ZMQ_SUB);
	assert(sub);
	rc = zmq_setsockopt(sub, ZMQ_SUBSCRIBE, "", 0);
	assert(rc == 0);
	rc = zmq_connect(sub, "inproc://irc.peers.1");
	assert(rc == 0);

	return NULL;
}
#endif

int main (int argc, char **argv)
{
	int sockfd, rc;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		return 2;
	}

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(6667);
	sa.sin_addr.s_addr = INADDR_ANY;

	rc = bind(sockfd, (struct sockaddr*)(&sa), sizeof(sa));
	if (rc < 0) {
		perror("bind *:6667");
		return 2;
	}

	rc = listen(sockfd, 64);
	if (rc < 0) {
		perror("listen");
		return 2;
	}

	void *zmq = zmq_ctx_new();
	assert(zmq);

	printf("ircd started up\n");
	printf("  mem sizes:\n");
	printf("   user: %ib\n", (int)sizeof(user_t));
	printf("   svc: %ib\n", (int)sizeof(svc_t));
	printf("   session: %ib\n", (int)sizeof(session_t));
	printf("   channel: %ib\n", (int)sizeof(channel_t));
	printf("   member: %ib\n", (int)sizeof(member_t));
	printf("\n\n");

	/******************************************************/

	my_ident_t me;
	me.name = fqdn();
	me.network = "NiftyLogic";

	/******************************************************/

	manager_t mgr;
	pool_init(&mgr.all.users,     100000, sizeof(user_t),    user_reset);
	pool_init(&mgr.all.svcs,         100, sizeof(svc_t),     svc_reset);
	pool_init(&mgr.all.sessions,  100100, sizeof(session_t), session_reset);
	pool_init(&mgr.all.channels,   30000, sizeof(channel_t), channel_reset);
	pool_init(&mgr.all.members,  1200000, sizeof(member_t),  member_reset);
	strncpy(mgr.motd_file, "/etc/motd", MAX_PATH);
	mgr.zmq = zmq;
	mgr.self = &me;

	pthread_t tid;
	rc = pthread_create(&tid, NULL, s_manager_thread, &mgr);
	assert(rc == 0);
	rc = pthread_detach(tid);
	assert(rc == 0);

	/******************************************************/

	int fd;
	struct sockaddr addr;
	socklen_t addr_len;

	printf("entering accept() loop\n");
accept: while ((fd = accept(sockfd, &addr, &addr_len)) >= 0) {
		printf("incoming connection\n");
		// FIXME: do something with the addr / addr_len

		client_handler_t *c = calloc(1, sizeof(client_handler_t));
		c->zmq = zmq;
		c->self = &me;
		c->fd = fd;

		rc = pthread_create(&tid, NULL, s_client_thread, c);
		assert(rc == 0);
		rc = pthread_detach(tid);
		assert(rc == 0);
	}
	printf("accept() loop terminated; fd=%i, errno=%i (%s)\n",
		fd, errno, strerror(errno));
	if (errno == EINTR) goto accept;

	printf("ircd shutting down\n");
	return 0;
}
