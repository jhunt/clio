#ifndef IRC_H
#define IRC_H

#include "../config.h"
#define SERVER_SOFTWARE   PACKAGE_NAME
#define SERVER_VERSION    PACKAGE_VERSION
#define SERVER_BUILDDATE  __DATE__ " " __TIME__

#include <stdint.h>
#include <stdarg.h>
#include <vigor.h>
#include "numerics.h"
#include "bits.h"

#define chan_special(c)  ((c) == CHAN_TYPE_SPECIAL)
#define chan_standard(c) (!chan_special(c))

#define MANAGER_ENDPOINT "inproc://irc.manager.1"
#define CLIENTS_ENDPOINT "inproc://irc.clients.1"
#define PEERS_ENDPOINT   "inproc://irc.peers.1"

typedef struct {
	size_t   total;
	size_t   each;

	void    *data;
	int     *used;

	void (*reset)(void*);
} pool_t;

typedef struct {
	size_t size;
	size_t seek;
	char   buf[1];
} buffer_t;

typedef struct {
	char  buffer[MAX_CMD+1];
	char *prefix;
	char *command;
	char *args[MAX_ARGS];
} msg_t;

typedef struct {
	char handle[MAX_HANDLE+1];  /* nick!user@host */
	char nick[MAX_NICK+1];
	char user[MAX_USER_NAME+1];
	char host[MAX_USER_HOST+1];

	char real[MAX_USER_NAME+1];
	char addr[MAX_USER_ADDR+1];
	uint8_t mode;
} user_t;

typedef struct {
	char nick[MAX_NICK+1];
	char dist[MAX_SVC_DIST+1];
	char info[MAX_SVC_INFO+1];
} svc_t;

typedef struct {
	int  sockfd;
	char pass[MAX_PASSWD+1];

	int32_t connected_at;
	int32_t active_at;

	user_t *user;
	svc_t  *svc;
} session_t;

typedef struct {
	char type;
	char name[MAX_CHAN_NAME+1];
	char topic[MAX_CHAN_TOPIC+1];

	char key[MAX_CHAN_KEY+1];
	size_t userlim;
	/* FIXME: ban/exclude/invite masks */

	uint8_t mode;
} channel_t;

typedef struct {
	session_t *sess;
	channel_t *chan;
	int32_t joined;

	uint8_t mode;
} member_t;

/*********************************/

int irc_isletter(char c);
int irc_isdigit(char c);
int irc_ishexdigit(char c);
int irc_isspecial(char c);
void irc_toupper(void *_, size_t len);
void irc_tolower(void *_, size_t len);

buffer_t* buffer_new(size_t size);
int buffer_read(buffer_t *buf, int fd);
msg_t* buffer_msg(buffer_t *buf);

msg_t* msg_parse(const char *line, size_t len);
void reply(int fd, char *msg);

int wildcard(const char *str, const char *pat);

int pool_init(pool_t *p, size_t n, size_t each, void (*reset)(void*));
void *pool_acq(pool_t *p);
void  pool_rel(pool_t *p, void *data);

int nick_valid(const char *n);
int username_valid(const char *n);

user_t* user_parse(const char *s, user_t *u);
uint8_t umode_f(uint8_t m, const char *s);
const char *umode_s(uint8_t m);
void user_reset(void*);

void svc_reset(void*);

void session_reset(void*);

void channel_reset(void*);

void member_reset(void*);

#endif
