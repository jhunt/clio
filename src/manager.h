#ifndef MANAGER_H
#define MANAGER_H

#include "irc.h"
#include <zmq.h>

typedef struct {
	void *zmq;
	void *zrouter;  /* ROUTER */
	void *zclients; /* PUB for client handlers */
	void *zpeers;   /* PUB for peer (server) handlers */

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

void* manager_thread(void *m);

#endif
