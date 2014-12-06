#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <netinet/ip.h>
#include "irc.h"

#define CLIENT_RECV_BUF 8192

typedef struct {
	void *zmq;

	struct {
		pool_t  users;
		pool_t  svcs;
		pool_t  sessions;
		pool_t  channels;
		pool_t  members;
	} all;

	hash_t  users;
	hash_t  svcs;
	hash_t  sessions;
	hash_t  channels;

	char motd_file[MAX_PATH];
} manager_t;

typedef struct {
	void *zmq;
	int fd;
} client_handler_t;

typedef struct {
	void *zmq;
} peer_handler_t;

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

	return NULL;
}

static void* s_client_thread(void *u)
{
	int rc;
	client_handler_t *c = (client_handler_t*)u;

	void *req = zmq_socket(c->zmq, ZMQ_REQ);
	assert(req);
	rc = zmq_connect(req, "inproc://irc.manager.1");
	assert(rc == 0);

	void *sub = zmq_socket(c->zmq, ZMQ_SUB);
	assert(sub);
	rc = zmq_setsockopt(sub, ZMQ_SUBSCRIBE, "", 0);
	assert(rc == 0);
	rc = zmq_connect(sub, "inproc://irc.clients.1");
	assert(rc == 0);

	printf("client handler started up on socket %i\n", c->fd);

	char recvbuf[CLIENT_RECV_BUF];
	size_t nread;

	while ((nread = read(c->fd, recvbuf, CLIENT_RECV_BUF - 1)) > 0) {
		recvbuf[nread] = '\0';
		printf(">> [%i] '%s'\n", c->fd, recvbuf);
	}

	printf("client handler on socket %i shutting down\n", c->fd);
	close(c->fd);
	return NULL;
}

static void* s_peer_thread(void *u)
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

	manager_t mgr;
	pool_init(&mgr.all.users,     100000, sizeof(user_t),    user_reset);
	pool_init(&mgr.all.svcs,         100, sizeof(svc_t),     svc_reset);
	pool_init(&mgr.all.sessions,  100100, sizeof(session_t), session_reset);
	pool_init(&mgr.all.channels,   30000, sizeof(channel_t), channel_reset);
	pool_init(&mgr.all.members,  1200000, sizeof(member_t),  member_reset);
	strncpy(mgr.motd_file, "/etc/motd", MAX_PATH);
	mgr.zmq = zmq;

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
