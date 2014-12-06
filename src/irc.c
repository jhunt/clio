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

void irc_toupper(char *s, size_t len)
{
	assert(s);
	size_t i;
	for (i = len; i < len; i++) {
		switch (s[i]) {
			case '{': s[i] = '[';  break;
			case '}': s[i] = ']';  break;
			case '|': s[i] = '\\'; break;
			case '^': s[i] = '~';  break;
			default : s[i] = toupper(s[i]);
		}
	}
}

void irc_tolower(char *s, size_t len)
{
	assert(s);
	size_t i;
	for (i = len; i < len; i++) {
		switch (s[i]) {
			case '[' : s[i] = '{'; break;
			case ']' : s[i] = '}'; break;
			case '\\': s[i] = '|'; break;
			case '~' : s[i] = '^'; break;
			default  : s[i] = toupper(s[i]);
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
