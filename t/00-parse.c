#include "test.h"

#define YES 1

TESTS {
	subtest { /* character classes */
		struct {
			int letter;
			int digit;
			int hexdigit;
			int special;
		} c[256] = {
			{   0,   0,   0,   0 }, /* 0x00 */
			{   0,   0,   0,   0 }, /* 0x01 */
			{   0,   0,   0,   0 }, /* 0x02 */
			{   0,   0,   0,   0 }, /* 0x03 */
			{   0,   0,   0,   0 }, /* 0x04 */
			{   0,   0,   0,   0 }, /* 0x05 */
			{   0,   0,   0,   0 }, /* 0x06 */
			{   0,   0,   0,   0 }, /* 0x07 */
			{   0,   0,   0,   0 }, /* 0x08 */
			{   0,   0,   0,   0 }, /* 0x09 */
			{   0,   0,   0,   0 }, /* 0x0a */
			{   0,   0,   0,   0 }, /* 0x0b */
			{   0,   0,   0,   0 }, /* 0x0c */
			{   0,   0,   0,   0 }, /* 0x0d */
			{   0,   0,   0,   0 }, /* 0x0e */
			{   0,   0,   0,   0 }, /* 0x0f */
			{   0,   0,   0,   0 }, /* 0x10 */
			{   0,   0,   0,   0 }, /* 0x11 */
			{   0,   0,   0,   0 }, /* 0x12 */
			{   0,   0,   0,   0 }, /* 0x13 */
			{   0,   0,   0,   0 }, /* 0x14 */
			{   0,   0,   0,   0 }, /* 0x15 */
			{   0,   0,   0,   0 }, /* 0x16 */
			{   0,   0,   0,   0 }, /* 0x17 */
			{   0,   0,   0,   0 }, /* 0x18 */
			{   0,   0,   0,   0 }, /* 0x19 */
			{   0,   0,   0,   0 }, /* 0x1a */
			{   0,   0,   0,   0 }, /* 0x1b */
			{   0,   0,   0,   0 }, /* 0x1c */
			{   0,   0,   0,   0 }, /* 0x1d */
			{   0,   0,   0,   0 }, /* 0x1e */
			{   0,   0,   0,   0 }, /* 0x1f */
			{   0,   0,   0,   0 }, /* 0x20 */
			{   0,   0,   0,   0 }, /* 0x21 */
			{   0,   0,   0,   0 }, /* 0x22 */
			{   0,   0,   0,   0 }, /* 0x23 */
			{   0,   0,   0,   0 }, /* 0x24 */
			{   0,   0,   0,   0 }, /* 0x25 */
			{   0,   0,   0,   0 }, /* 0x26 */
			{   0,   0,   0,   0 }, /* 0x27 */
			{   0,   0,   0,   0 }, /* 0x28 */
			{   0,   0,   0,   0 }, /* 0x29 */
			{   0,   0,   0,   0 }, /* 0x2a */
			{   0,   0,   0,   0 }, /* 0x2b */
			{   0,   0,   0,   0 }, /* 0x2c */
			{   0,   0,   0,   0 }, /* 0x2d */
			{   0,   0,   0,   0 }, /* 0x2e */
			{   0,   0,   0,   0 }, /* 0x2f */

			/* 0 - 9 */
			{   0, YES, YES,   0 }, /* 0x30 */
			{   0, YES, YES,   0 }, /* 0x31 */
			{   0, YES, YES,   0 }, /* 0x32 */
			{   0, YES, YES,   0 }, /* 0x33 */
			{   0, YES, YES,   0 }, /* 0x34 */
			{   0, YES, YES,   0 }, /* 0x35 */
			{   0, YES, YES,   0 }, /* 0x36 */
			{   0, YES, YES,   0 }, /* 0x37 */
			{   0, YES, YES,   0 }, /* 0x38 */
			{   0, YES, YES,   0 }, /* 0x39 */

			{   0,   0,   0,   0 }, /* 0x3a */
			{   0,   0,   0,   0 }, /* 0x3b */
			{   0,   0,   0,   0 }, /* 0x3c */
			{   0,   0,   0,   0 }, /* 0x3d */
			{   0,   0,   0,   0 }, /* 0x3e */
			{   0,   0,   0,   0 }, /* 0x3f */
			{   0,   0,   0,   0 }, /* 0x40 */

			/* A-Z */
			{ YES,   0, YES,   0 }, /* 0x41 */
			{ YES,   0, YES,   0 }, /* 0x42 */
			{ YES,   0, YES,   0 }, /* 0x43 */
			{ YES,   0, YES,   0 }, /* 0x44 */
			{ YES,   0, YES,   0 }, /* 0x45 */
			{ YES,   0, YES,   0 }, /* 0x46 */
			{ YES,   0,   0,   0 }, /* 0x47 */
			{ YES,   0,   0,   0 }, /* 0x48 */
			{ YES,   0,   0,   0 }, /* 0x49 */
			{ YES,   0,   0,   0 }, /* 0x4a */
			{ YES,   0,   0,   0 }, /* 0x4b */
			{ YES,   0,   0,   0 }, /* 0x4c */
			{ YES,   0,   0,   0 }, /* 0x4d */
			{ YES,   0,   0,   0 }, /* 0x4e */
			{ YES,   0,   0,   0 }, /* 0x4f */
			{ YES,   0,   0,   0 }, /* 0x50 */
			{ YES,   0,   0,   0 }, /* 0x51 */
			{ YES,   0,   0,   0 }, /* 0x52 */
			{ YES,   0,   0,   0 }, /* 0x53 */
			{ YES,   0,   0,   0 }, /* 0x54 */
			{ YES,   0,   0,   0 }, /* 0x55 */
			{ YES,   0,   0,   0 }, /* 0x56 */
			{ YES,   0,   0,   0 }, /* 0x57 */
			{ YES,   0,   0,   0 }, /* 0x58 */
			{ YES,   0,   0,   0 }, /* 0x59 */
			{ YES,   0,   0,   0 }, /* 0x5a */

			/* [ \ ] */
			{   0,   0,   0, YES }, /* 0x5b */
			{   0,   0,   0, YES }, /* 0x5c */
			{   0,   0,   0, YES }, /* 0x5d */

			/* ^ _ */
			{   0,   0,   0, YES }, /* 0x5e */
			{   0,   0,   0, YES }, /* 0x5f */

			{   0,   0,   0,   0 }, /* 0x60 */

			/* a-z */
			{ YES,   0, YES,   0 }, /* 0x61 */
			{ YES,   0, YES,   0 }, /* 0x62 */
			{ YES,   0, YES,   0 }, /* 0x63 */
			{ YES,   0, YES,   0 }, /* 0x64 */
			{ YES,   0, YES,   0 }, /* 0x65 */
			{ YES,   0, YES,   0 }, /* 0x66 */
			{ YES,   0,   0,   0 }, /* 0x67 */
			{ YES,   0,   0,   0 }, /* 0x68 */
			{ YES,   0,   0,   0 }, /* 0x69 */
			{ YES,   0,   0,   0 }, /* 0x6a */
			{ YES,   0,   0,   0 }, /* 0x6b */
			{ YES,   0,   0,   0 }, /* 0x6c */
			{ YES,   0,   0,   0 }, /* 0x6d */
			{ YES,   0,   0,   0 }, /* 0x6e */
			{ YES,   0,   0,   0 }, /* 0x6f */
			{ YES,   0,   0,   0 }, /* 0x70 */
			{ YES,   0,   0,   0 }, /* 0x71 */
			{ YES,   0,   0,   0 }, /* 0x72 */
			{ YES,   0,   0,   0 }, /* 0x73 */
			{ YES,   0,   0,   0 }, /* 0x74 */
			{ YES,   0,   0,   0 }, /* 0x75 */
			{ YES,   0,   0,   0 }, /* 0x76 */
			{ YES,   0,   0,   0 }, /* 0x77 */
			{ YES,   0,   0,   0 }, /* 0x78 */
			{ YES,   0,   0,   0 }, /* 0x79 */
			{ YES,   0,   0,   0 }, /* 0x7a */

			/* { | } */
			{   0,   0,   0, YES }, /* 0x7b */
			{   0,   0,   0, YES }, /* 0x7c */
			{   0,   0,   0, YES }, /* 0x7d */

			{   0,   0,   0,   0 }, /* 0x7e */
			{   0,   0,   0,   0 }, /* 0x7f */
			{   0,   0,   0,   0 }, /* 0x80 */
			{   0,   0,   0,   0 }, /* 0x81 */
			{   0,   0,   0,   0 }, /* 0x82 */
			{   0,   0,   0,   0 }, /* 0x83 */
			{   0,   0,   0,   0 }, /* 0x84 */
			{   0,   0,   0,   0 }, /* 0x85 */
			{   0,   0,   0,   0 }, /* 0x86 */
			{   0,   0,   0,   0 }, /* 0x87 */
			{   0,   0,   0,   0 }, /* 0x88 */
			{   0,   0,   0,   0 }, /* 0x89 */
			{   0,   0,   0,   0 }, /* 0x8a */
			{   0,   0,   0,   0 }, /* 0x8b */
			{   0,   0,   0,   0 }, /* 0x8c */
			{   0,   0,   0,   0 }, /* 0x8d */
			{   0,   0,   0,   0 }, /* 0x8e */
			{   0,   0,   0,   0 }, /* 0x8f */
			{   0,   0,   0,   0 }, /* 0x90 */
			{   0,   0,   0,   0 }, /* 0x91 */
			{   0,   0,   0,   0 }, /* 0x92 */
			{   0,   0,   0,   0 }, /* 0x93 */
			{   0,   0,   0,   0 }, /* 0x94 */
			{   0,   0,   0,   0 }, /* 0x95 */
			{   0,   0,   0,   0 }, /* 0x96 */
			{   0,   0,   0,   0 }, /* 0x97 */
			{   0,   0,   0,   0 }, /* 0x98 */
			{   0,   0,   0,   0 }, /* 0x99 */
			{   0,   0,   0,   0 }, /* 0x9a */
			{   0,   0,   0,   0 }, /* 0x9b */
			{   0,   0,   0,   0 }, /* 0x9c */
			{   0,   0,   0,   0 }, /* 0x9d */
			{   0,   0,   0,   0 }, /* 0x9e */
			{   0,   0,   0,   0 }, /* 0x9f */
			{   0,   0,   0,   0 }, /* 0xa0 */
			{   0,   0,   0,   0 }, /* 0xa1 */
			{   0,   0,   0,   0 }, /* 0xa2 */
			{   0,   0,   0,   0 }, /* 0xa3 */
			{   0,   0,   0,   0 }, /* 0xa4 */
			{   0,   0,   0,   0 }, /* 0xa5 */
			{   0,   0,   0,   0 }, /* 0xa6 */
			{   0,   0,   0,   0 }, /* 0xa7 */
			{   0,   0,   0,   0 }, /* 0xa8 */
			{   0,   0,   0,   0 }, /* 0xa9 */
			{   0,   0,   0,   0 }, /* 0xaa */
			{   0,   0,   0,   0 }, /* 0xab */
			{   0,   0,   0,   0 }, /* 0xac */
			{   0,   0,   0,   0 }, /* 0xad */
			{   0,   0,   0,   0 }, /* 0xae */
			{   0,   0,   0,   0 }, /* 0xaf */
			{   0,   0,   0,   0 }, /* 0xb0 */
			{   0,   0,   0,   0 }, /* 0xb1 */
			{   0,   0,   0,   0 }, /* 0xb2 */
			{   0,   0,   0,   0 }, /* 0xb3 */
			{   0,   0,   0,   0 }, /* 0xb4 */
			{   0,   0,   0,   0 }, /* 0xb5 */
			{   0,   0,   0,   0 }, /* 0xb6 */
			{   0,   0,   0,   0 }, /* 0xb7 */
			{   0,   0,   0,   0 }, /* 0xb8 */
			{   0,   0,   0,   0 }, /* 0xb9 */
			{   0,   0,   0,   0 }, /* 0xba */
			{   0,   0,   0,   0 }, /* 0xbb */
			{   0,   0,   0,   0 }, /* 0xbc */
			{   0,   0,   0,   0 }, /* 0xbd */
			{   0,   0,   0,   0 }, /* 0xbe */
			{   0,   0,   0,   0 }, /* 0xbf */
			{   0,   0,   0,   0 }, /* 0xc0 */
			{   0,   0,   0,   0 }, /* 0xc1 */
			{   0,   0,   0,   0 }, /* 0xc2 */
			{   0,   0,   0,   0 }, /* 0xc3 */
			{   0,   0,   0,   0 }, /* 0xc4 */
			{   0,   0,   0,   0 }, /* 0xc5 */
			{   0,   0,   0,   0 }, /* 0xc6 */
			{   0,   0,   0,   0 }, /* 0xc7 */
			{   0,   0,   0,   0 }, /* 0xc8 */
			{   0,   0,   0,   0 }, /* 0xc9 */
			{   0,   0,   0,   0 }, /* 0xca */
			{   0,   0,   0,   0 }, /* 0xcb */
			{   0,   0,   0,   0 }, /* 0xcc */
			{   0,   0,   0,   0 }, /* 0xcd */
			{   0,   0,   0,   0 }, /* 0xce */
			{   0,   0,   0,   0 }, /* 0xcf */
			{   0,   0,   0,   0 }, /* 0xd0 */
			{   0,   0,   0,   0 }, /* 0xd1 */
			{   0,   0,   0,   0 }, /* 0xd2 */
			{   0,   0,   0,   0 }, /* 0xd3 */
			{   0,   0,   0,   0 }, /* 0xd4 */
			{   0,   0,   0,   0 }, /* 0xd5 */
			{   0,   0,   0,   0 }, /* 0xd6 */
			{   0,   0,   0,   0 }, /* 0xd7 */
			{   0,   0,   0,   0 }, /* 0xd8 */
			{   0,   0,   0,   0 }, /* 0xd9 */
			{   0,   0,   0,   0 }, /* 0xda */
			{   0,   0,   0,   0 }, /* 0xdb */
			{   0,   0,   0,   0 }, /* 0xdc */
			{   0,   0,   0,   0 }, /* 0xdd */
			{   0,   0,   0,   0 }, /* 0xde */
			{   0,   0,   0,   0 }, /* 0xdf */
			{   0,   0,   0,   0 }, /* 0xe0 */
			{   0,   0,   0,   0 }, /* 0xe1 */
			{   0,   0,   0,   0 }, /* 0xe2 */
			{   0,   0,   0,   0 }, /* 0xe3 */
			{   0,   0,   0,   0 }, /* 0xe4 */
			{   0,   0,   0,   0 }, /* 0xe5 */
			{   0,   0,   0,   0 }, /* 0xe6 */
			{   0,   0,   0,   0 }, /* 0xe7 */
			{   0,   0,   0,   0 }, /* 0xe8 */
			{   0,   0,   0,   0 }, /* 0xe9 */
			{   0,   0,   0,   0 }, /* 0xea */
			{   0,   0,   0,   0 }, /* 0xeb */
			{   0,   0,   0,   0 }, /* 0xec */
			{   0,   0,   0,   0 }, /* 0xed */
			{   0,   0,   0,   0 }, /* 0xee */
			{   0,   0,   0,   0 }, /* 0xef */
			{   0,   0,   0,   0 }, /* 0xf0 */
			{   0,   0,   0,   0 }, /* 0xf1 */
			{   0,   0,   0,   0 }, /* 0xf2 */
			{   0,   0,   0,   0 }, /* 0xf3 */
			{   0,   0,   0,   0 }, /* 0xf4 */
			{   0,   0,   0,   0 }, /* 0xf5 */
			{   0,   0,   0,   0 }, /* 0xf6 */
			{   0,   0,   0,   0 }, /* 0xf7 */
			{   0,   0,   0,   0 }, /* 0xf8 */
			{   0,   0,   0,   0 }, /* 0xf9 */
			{   0,   0,   0,   0 }, /* 0xfa */
			{   0,   0,   0,   0 }, /* 0xfb */
			{   0,   0,   0,   0 }, /* 0xfc */
			{   0,   0,   0,   0 }, /* 0xfd */
			{   0,   0,   0,   0 }, /* 0xfe */
			{   0,   0,   0,   0 }, /* 0xff */
		};

		size_t i;
		for (i = 0; i < 255; i++) {
			is_int(irc_isletter(i), c[i].letter,
				"%0x %s a letter", i, (c[i].letter ? "is" : "is not"));
			is_int(irc_isdigit(i), c[i].digit,
				"%0x %s a digit", i, (c[i].digit ? "is" : "is not"));
			is_int(irc_ishexdigit(i), c[i].hexdigit,
				"%0x %s a hexdigit", i, (c[i].hexdigit ? "is" : "is not"));
			is_int(irc_isspecial(i), c[i].special,
				"%0x %s special", i, (c[i].special ? "is" : "is not"));
		}
	}

	subtest { /* token validation */

		/* nick */
		ok(nick_valid("foo"), "foo is a valid nick");
		ok(nick_valid("foo12"), "foo12 is a valid nick");
		ok(nick_valid("foo{|}"), "foo{|} is a valid nick");
		ok(nick_valid("foo[\\]"), "foo[\\] is a valid nick");
		ok(nick_valid("fo{0}_bar"), "fo{0}_bar is a valid nick");

		ok(!nick_valid(NULL),   "(nil) is not a valid nick");
		ok(!nick_valid(""),     "'' is not a valid nick");
		ok(!nick_valid("0"),    "'0' is not a valid nick - starts with a digit");
		ok(!nick_valid("0per"), "'0per' is not a valid nick - starts with a digit");
		ok(!nick_valid("_me"),  "'_me' is not a valid nick - start with a non-letter");
		ok(!nick_valid("me _"), "'me _' is not a valid nick - contains whitespace");
		ok(!nick_valid("t\tt"), "'t\\tt' is not a valid nick - contains whitespace");
		ok(!nick_valid("fo{0}_bar_"), "fo{0}_bar_ is not a valid nick - too long");
		ok(!nick_valid("fo\1bar"), "fo\\1bar is not a valid nick - contains SOH");

		/* username */
		ok(username_valid("foo"), "foo is a valid username");
		ok(username_valid("99probs"), "99probs is a valid username");
		ok(username_valid("*&^%$#_+_!"), "*&^%$#_+_! is a valid username");
		ok(username_valid("tab\ttab"),   "tab\\ttab is a valid username");

		ok(!username_valid(NULL), "(nil) is not a valid username");
		ok(!username_valid(""), "'' is not a valid username");
		ok(!username_valid("user@domain"), "user@domain is not a valid username (@)");
		ok(!username_valid("user name"),   "user\\ name is not a valid username (sp)");
		ok(!username_valid("user\rname"),  "user\\rname is not a valid username (cr)");
		ok(!username_valid("user\nname"),  "user\\nname is not a valid username (lf)");
	}

	subtest { /* upper/lower conversion */
		char *s = NULL;
		size_t i = 0;

		free(s); s = strdup("a simple string");
		irc_toupper(s, strlen(s));
		is_string(s, "A SIMPLE STRING", "ascii 7-bit upcase");
		irc_tolower(s, strlen(s));
		is_string(s, "a simple string", "ascii 7-bit downcase");

		free(s); s = strdup("MiXeD cAsE\r\n");
		irc_toupper(s, strlen(s));
		is_string(s, "MIXED CASE\r\n", "mixed -> upper");

		free(s); s = strdup("MiXeD cAsE\r\n");
		irc_tolower(s, strlen(s));
		is_string(s, "mixed case\r\n", "mixed -> lower");

		free(s); s = strdup("scandinavia {}|^[]\\~ case");
		irc_tolower(s, strlen(s));
		is_string(s, "scandinavia {}|^{}|^ case", "scandinavian -> lower");

		free(s); s = strdup("scandinavia {}|^[]\\~ case");
		irc_toupper(s, strlen(s));
		is_string(s, "SCANDINAVIA []\\~[]\\~ CASE", "scandinavian -> upper");

		unsigned char u[2] = {0};
		for (i = 0; i <= 255; i++) {
			if (i > 0x40 && i < 0x5f) continue; /* A-Z (+scandinavia) */
			if (i > 0x60 && i < 0x7f) continue; /* a-z (+scandinavia) */

			u[0] = i; irc_toupper(u, 1);
			is_int(u[0], i, "upper[%i] == %i (%x)", i, i, i);
			u[0] = i; irc_tolower(u, 1);
			is_int(u[0], i, "lower[%i] == %i (%x)", i, i, i);
		}

		free(s);
	}

	/****************************************************************************/

	subtest { /* buffer with small messages */
		FILE *t = tmpfile();
		int fd = fileno(t);

		fprintf(t, "PASS supersecret"              /* 16 */ "\r\n");
		fprintf(t, "NICK johnny0"                  /* 12 */ "\r\n");
		fprintf(t, "USER johnny0 8 * :Johnny Zero" /* 29 */ "\r\n");
		rewind(t);

		buffer_t *buf = buffer_new(512);
		isnt_null(buf, "buffer_new returned a new buffer");
		is_int(buf->size, 512, "new buffer size is 512 bytes");
		is_int(buf->seek, 0, "initial buffer seek offset is 0");
		is_string(buf->buf, "", "buffer is initially empty");

		int n = buffer_read(buf, fd);
		is_int(n, buf->seek, "buffer_read returns the seek offset");
		is_int(buf->seek, 16+2+12+2+29+2, "read all characters from the tempfile");
		is_string(buf->buf, "PASS supersecret\r\n"
		                    "NICK johnny0\r\n"
		                    "USER johnny0 8 * :Johnny Zero\r\n",
		          "read entirety of tempfile into buffer");

		msg_t *m = buffer_msg(buf);
		isnt_null(m, "got a message from the buffer");
		is_int(buf->seek, 12+2+29+2, "buf->seek adjusted to exclude parsed line");
		is_string(buf->buf, "NICK johnny0\r\n"
		                    "USER johnny0 8 * :Johnny Zero\r\n",
		          "first line is removed from buffer");
		is_string(m->command, "PASS", "parsed PASS message from buffer");
		is_string(m->args[0], "supersecret", "parsed arg $1 from buffer");
		is_null(m->args[1], "no arg $2 from buffer");

		m = buffer_msg(buf);
		isnt_null(m, "got a message from the buffer");
		is_int(buf->seek, 29+2, "buf->seek adjusted to exclude parsed line");
		is_string(buf->buf, "USER johnny0 8 * :Johnny Zero\r\n",
		          "second line is removed from buffer");
		is_string(m->command, "NICK", "parsed NICK message from buffer");
		is_string(m->args[0], "johnny0", "parsed arg $1 from buffer");
		is_null(m->args[1], "no arg $2 from buffer");

		m = buffer_msg(buf);
		isnt_null(m, "got a message from the buffer");
		is_int(buf->seek, 0, "buf->seek adjusted to exclude parsed line");
		is_string(buf->buf, "", "third (and final) line removed from buffer");
		is_string(m->command, "USER", "parsed USER message from buffer");
		is_string(m->args[0], "johnny0", "parsed arg $1 from buffer");
		is_string(m->args[1], "8", "parsed arg $2 from buffer");
		is_string(m->args[2], "*", "parsed arg $3 from buffer");
		is_string(m->args[3], "Johnny Zero", "parsed arg $4 from buffer");
		is_null(m->args[4], "no arg $4 from buffer");

		m = buffer_msg(buf);
		is_null(m, "no more messages to parse from buffer");

		n = buffer_read(buf, fd);
		is_int(n, 0, "EOF reached on fd");

		fclose(t);
	}

	subtest { /* buffer with large messages */
		FILE *t = tmpfile();
		int fd = fileno(t);

		fprintf(t, "PRIVMSG foo :this is a long message (<64 chars)" /* 47 */ "\r\n");
		fprintf(t, "PRIVMSG foo :another long message (>32 chars)"   /* 45 */ "\r\n");
		fprintf(t, "NICK renamed"                                    /* 12 */ "\r\n");
		rewind(t);

		buffer_t *buf = buffer_new(64);
		isnt_null(buf, "buffer_new returned a new buffer");
		is_int(buf->size, 64, "new buffer size is 64 bytes");
		is_int(buf->seek, 0, "initial buffer seek offset is 0");
		is_string(buf->buf, "", "buffer is initially empty");

		int n = buffer_read(buf, fd);
		is_int(n, buf->seek, "buffer_read returns the seek offset");
		is_int(buf->seek, 63, "filled the buffer with 63 (+\\0) read characters");
		is_string(buf->buf, "PRIVMSG foo :this is a long message (<64 chars)\r\n"
		                    "PRIVMSG foo :a", /* didn't get all of the next line */
		          "read first full line and part of the second");

		msg_t *m = buffer_msg(buf);
		isnt_null(m, "got a message from the buffer");
		is_int(buf->seek, 14, "buf->seek adjusted to exclude parsed line");
		is_string(buf->buf, "PRIVMSG foo :a", /* didn't get all of the next line */
		          "first line is removed from buffer");
		is_string(m->command, "PRIVMSG", "parsed PRIVMSG message from buffer");
		is_string(m->args[0], "foo", "parsed arg $1 from buffer");
		is_string(m->args[1], "this is a long message (<64 chars)",
				"parsed arg $2 from buffer");
		is_null(m->args[2], "no arg $3 from buffer");

		m = buffer_msg(buf);
		is_null(m, "buffer doesn't contain a full message");

		n = buffer_read(buf, fd);
		is_int(n, buf->seek, "buffer_read returns the seek offset");
		is_int(buf->seek, 45+2+12+2, "read the rest of the tempfile");
		is_string(buf->buf, "PRIVMSG foo :another long message (>32 chars)\r\n"
		                    "NICK renamed\r\n",
		          "preserved the previous partial message string in the buffer");

		m = buffer_msg(buf);
		isnt_null(m, "got a message from the buffer");
		is_int(buf->seek, 12+2, "buf->seek adjusted to exclude parsed line");
		is_string(buf->buf, "NICK renamed\r\n",
		          "second line is removed from the buffer");
		is_string(m->command, "PRIVMSG", "parsed PRIVMSG message from buffer");
		is_string(m->args[0], "foo", "parsed arg $1 from buffer");
		is_string(m->args[1], "another long message (>32 chars)",
				"parsed arg $2 from buffer");
		is_null(m->args[2], "no arg $3 from buffer");

		m = buffer_msg(buf);
		isnt_null(m, "got a message from the buffer");
		is_int(buf->seek, 0, "buf->seek adjusted to exclude parsed line");
		is_string(buf->buf, "", "third (and final) line is removed from the buffer");
		is_string(m->command, "NICK", "parsed NICK message from buffer");
		is_string(m->args[0], "renamed", "parsed arg $1 from buffer");
		is_null(m->args[1], "no arg $2 from buffer");

		m = buffer_msg(buf);
		is_null(m, "no more messages in buffer");

		n = buffer_read(buf, fd);
		is_int(n, 0, "EOF reached on fd");

		fclose(t);
	}

	/****************************************************************************/

	subtest {
		msg_t *m;
		const char *c;
		size_t i;

		c = "COMMAND";
		m = msg_parse(c, strlen(c));
		isnt_null(m, "'%s' - parsed successfully", c);
		is_null(m->prefix, "'%s' - no prefix", c);
		is_string(m->command, "COMMAND", "'%s' - command string", c);
		for (i = 0; i < MAX_ARGS; i++)
			is_null(m->args[i], "'%s' - no arg $%i", c, i+1);


		c = "COMMAND ARG1";
		m = msg_parse(c, strlen(c));
		isnt_null(m, "'%s' - parsed successfully", c);
		is_null(m->prefix, "'%s' - no prefix", c);
		is_string(m->command, "COMMAND", "'%s' - command string", c);
		is_string(m->args[0], "ARG1", "'%s' - first argument", c);
		for (i = 1; i < MAX_ARGS; i++)
			is_null(m->args[i], "'%s' - no arg $%i", c, i+1);


		c = "COMMAND one two three four five six seven eight nine ten eleven twelve thirteen fourteen fifteen";
		m = msg_parse(c, strlen(c));
		isnt_null(m, "'%s' - parsed successfully", c);
		is_null(m->prefix, "'%s' - no prefix", c);
		is_string(m->command, "COMMAND", "'%s' - command string", c);
		is_string(m->args[0], "one", "'%s' - first argument", c);
		is_string(m->args[1], "two", "'%s' - second argument", c);
		is_string(m->args[2], "three", "'%s' - third argument", c);
		is_string(m->args[3], "four", "'%s' - fourth argument", c);
		is_string(m->args[4], "five", "'%s' - fifth argument", c);
		is_string(m->args[5], "six", "'%s' - sixth argument", c);
		is_string(m->args[6], "seven", "'%s' - seventh argument", c);
		is_string(m->args[7], "eight", "'%s' - eighth argument", c);
		is_string(m->args[8], "nine", "'%s' - ninth argument", c);
		is_string(m->args[9], "ten", "'%s' - tenth argument", c);
		is_string(m->args[10], "eleven", "'%s' - eleventh argument", c);
		is_string(m->args[11], "twelve", "'%s' - twelfth argument", c);
		is_string(m->args[12], "thirteen", "'%s' - thirteenth argument", c);
		is_string(m->args[13], "fourteen", "'%s' - fourteenth argument", c);
		is_string(m->args[14], "fifteen", "'%s' - fifteenth argument", c);


		c = ":from COMMAND";
		m = msg_parse(c, strlen(c));
		isnt_null(m, "'%s' - parsed successfully", c);
		is_string(m->prefix, "from", "'%s' - prefix", c);
		is_string(m->command, "COMMAND", "'%s' - command string", c);
		for (i = 0; i < MAX_ARGS; i++)
			is_null(m->args[i], "'%s' - no arg $%i", c, i+1);

		c = ":from COMMAND ARG1";
		m = msg_parse(c, strlen(c));
		isnt_null(m, "'%s' - parsed successfully", c);
		is_string(m->prefix, "from", "'%s' - prefix", c);
		is_string(m->command, "COMMAND", "'%s' - command string", c);
		is_string(m->args[0], "ARG1", "'%s' - first argument", c);
		is_null(m->args[1], "'%s' - no arg $2", c);


		c = ":from COMMAND one two three four five six seven eight nine ten eleven twelve thirteen fourteen fifteen";
		m = msg_parse(c, strlen(c));
		isnt_null(m, "'%s' - parsed successfully", c);
		is_string(m->prefix, "from", "'%s' - prefix", c);
		is_string(m->command, "COMMAND", "'%s' - command string", c);
		is_string(m->args[0], "one", "'%s' - first argument", c);
		is_string(m->args[14], "fifteen", "'%s' - fifteenth argument", c);


		c = "MSG nick :this is a really long message";
		m = msg_parse(c, strlen(c));
		isnt_null(m, "'%s' - parsed successfully", c);
		is_null(m->prefix, "'%s' - no prefix", c);
		is_string(m->command, "MSG", "'%s' - command string", c);
		is_string(m->args[0], "nick", "'%s' - first argument", c);
		is_string(m->args[1], "this is a really long message", "'%s' - second (last) argument", c);
		is_null(m->args[2], "'%s' - no arg $3", c);
	}

	done_testing();
}
