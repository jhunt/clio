#include <stdio.h>
#include <stdlib.h>
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

msg_t* irc_parse_msg(const char *line, size_t len)
{
	assert(line);
	assert(len <= 510);
	assert(len >  0);

	msg_t *m = calloc(1, sizeof(msg_t));
	assert(m != NULL);

	memcpy(m->raw,    line, len);
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
