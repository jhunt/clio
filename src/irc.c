#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "irc.h"

#define TOKEN(x) s_token(&x)
static inline char* s_token(char **src)
{
	assert(src && *src);

	char *s, *t; s = t = *src;

	while (*s && !isspace(*s)) s++;
	*s++ = '\0';
	while (*s &&  isspace(*s)) s++;
	*src = s;
	return t;
}

/**************************************************************************/

int irc_isletter(char c)
{
	return (c >= 'A' && c <= 'Z')
	    || (c >= 'a' && c <= 'z');
}

int irc_isdigit(char c)
{
	return (c >= '0' && c <= '9');
}

int irc_ishexdigit(char c)
{
	return (c >= '0' && c <= '9')
	    || (c >= 'a' && c <= 'f')
	    || (c >= 'A' && c <= 'F');
}

int irc_isspecial(char c)
{
	return (c >= 0x5b && c <= 0x5f)
	    || (c >= 0x7b && c <= 0x7d);
}

void irc_toupper(void *_, size_t len)
{
	assert(_);
	char *s = (char *)_;
	size_t i;
	for (i = 0; i < len; i++) {
		switch (s[i]) {
		case '{': s[i] = '[';  break;
		case '}': s[i] = ']';  break;
		case '|': s[i] = '\\'; break;
		case '^': s[i] = '~';  break;
		default : s[i] = toupper(s[i]);
		}
	}
}

void irc_tolower(void *_, size_t len)
{
	assert(_);
	char *s = (char *)_;
	size_t i;
	for (i = 0; i < len; i++) {
		switch (s[i]) {
		case '[' : s[i] = '{'; break;
		case ']' : s[i] = '}'; break;
		case '\\': s[i] = '|'; break;
		case '~' : s[i] = '^'; break;
		default  : s[i] = tolower(s[i]);
		}
	}
}

buffer_t* buffer_new(size_t size)
{
	buffer_t *buf = calloc(1, sizeof(buffer_t) + size);
	assert(buf);

	buf->size = size;
	buf->seek = 0;

	return buf;
}

int buffer_read(buffer_t *buf, int fd)
{
	ssize_t nread = read(fd, buf->buf + buf->seek, buf->size - buf->seek - 1);
	if (nread == 0) return 0;
	if (nread < 0) return -1;

	buf->seek += nread;
	buf->buf[buf->seek] = '\0';
	return buf->seek;
}

msg_t* buffer_msg(buffer_t *buf)
{
	char *END = buf->buf + buf->seek;
	char *p = buf->buf;
	while (p < END && *p != '\r' && *p != '\n') p++;
	if (p == END) return NULL;

	if (*p == '\r') *p++ = '\0';
	if (*p == '\n') *p++ = '\0';

	msg_t *msg = msg_parse(buf->buf, p - buf->buf);

	memmove(buf->buf, p, END - p + 1u);
	buf->seek = END - p;
	buf->buf[buf->seek] = '\0';

	return msg;
}

msg_t* msg_parse(const char *line, size_t len)
{
	assert(line);
	assert(len <= 510);
	assert(len >  0);

	msg_t *m = calloc(1, sizeof(msg_t));
	assert(m != NULL);

	memcpy(m->buffer, line, len);

	char *s = m->buffer;
	if (s[0] == ':') {
		s++;
		m->prefix = TOKEN(s);

	} else {
		m->prefix = NULL;
	}

	m->command = TOKEN(s);

	size_t i = 0;
	while (*s && i < MAX_ARGS) {
		if (*s == ':') { /* trailer */
			m->args[i++] = ++s;
			break;
		} else {
			m->args[i++] = TOKEN(s);
		}
	}

	return m;
}

void reply(int fd, char *msg)
{
	fprintf(stderr, "<< [%i] %s\n", fd, msg);
	write(fd, msg, strlen(msg));
	free(msg);
}

int wildcard(const char *str, const char *pat)
{
	const char *s = str;
	const char *p = pat;

	/* '*' matches everything */
	if (*p == '*' && !*(p+1)) return 1;

	while (*p && *s) {
		if (*p == '*') {
			if (!*++p) return 1; /* trailing * == implicit match */

			/* find all of the look aheads and try each
			   in turn, until we get a positive match or
			   run out of substring remainders to try. */
			while (*s) {
				if (*p == *s && wildcard(s, p)) return 1;
				s++;
			}
			return 0;
		}
		if (*p == '?') { p++; s++; continue; }
		if (*p == '\\' && !*++p) return 0;
		if (*p != *s) return 0;
		p++; s++;
	}

	if (*p == '*' && !*(p+1)) return 1;
	return !*p && !*s;
}

int pool_init(pool_t *p, size_t n, size_t each, void (*reset)(void*))
{
	assert(p);
	assert(n > 0);
	assert(each > 0);

	p->total = n;
	p->each  = each;
	p->reset = reset;

	p->used = calloc(n, sizeof(int));
	assert(p->used);

	p->data = malloc(n * each);
	assert(p->data);

	return 0;
}

#define pool_nth(p,i) ((p)->data + ((i) * (p)->each))
void *pool_acq(pool_t *p)
{
	assert(p);

	size_t i;
	for (i = 0; i < p->total; i++) {
		if (p->used[i]) continue;
		p->used[i] = 1;
		return pool_nth(p, i);
	}
	return NULL;
}

void pool_rel(pool_t *p, void *data)
{
	assert(p);
	assert(data);
	assert(data >= pool_nth(p,0) && data <= pool_nth(p, p->total-1));
	assert((data - p->data) % p->each == 0);

	size_t i = (data - p->data) / p->each;
	if (p->reset) (*p->reset)(data);
	p->used[i] = 0;
}

int nick_valid(const char *n)
{
	if (!n || !*n) return 0;
	if (!irc_isletter(*n)) return 0;

	size_t i, l = strlen(n);
	if (l > MAX_NICK) return 0;

	for (i = 1; i < l; i++)
		if (!irc_isletter(n[i])
		 && !irc_isdigit(n[i])
		 && !irc_isspecial(n[i])
		 && n[i] != '-')
			return 0;
	return 1;
}

int username_valid(const char *n)
{
	if (!n || !*n) return 0;

	size_t i, l = strlen(n);
	for (i = 0; i < l; i++)
		if (n[i] == '\0'
		 || n[i] == '\r'
		 || n[i] == '\n'
		 || n[i] == ' '
		 || n[i] == '@') return 0;
	return 1;
}

user_t* user_parse(const char *s_, user_t *u)
{
	if (!s_) return NULL;

	char *nick = NULL, *user = NULL, *host = NULL;
	char *str = strdup(s_);
	char *s = str, *p = s;
	while (*p && *p != '!') p++;
	if (!*p) return NULL;
	*p = '\0';

	if (!nick_valid(s)) return NULL;
	nick = s;
	s = ++p;

	while (*p && *p != '@') p++;
	if (!*p) return NULL;
	*p = '\0';

	if (!username_valid(s)) return NULL;
	user = s;
	s = ++p;

	if (!*s) return NULL; /* no hostname */
	host = s;

	if (!u) u = calloc(1, sizeof(user_t));
	if (!u) return NULL;

	strncpy(u->nick, nick, MAX_NICK);
	strncpy(u->user, user, MAX_USER_NAME);
	strncpy(u->host, host, MAX_USER_HOST);
	free(str);
	return u;
}

void user_reset(void *u)
{
}

void svc_reset(void *s)
{
}

void session_reset(void *s)
{
}

void channel_reset(void *c)
{
}

void member_reset(void *m)
{
}
