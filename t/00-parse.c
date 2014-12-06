#include "test.h"

TESTS {
	subtest {
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
		is_null(m->args[1], "no arg $2 from buffer message");

		m = buffer_msg(buf);
		isnt_null(m, "got a message from the buffer");
		is_int(buf->seek, 29+2, "buf->seek adjusted to exclude parsed line");
		is_string(buf->buf, "USER johnny0 8 * :Johnny Zero\r\n",
		          "second line is removed from buffer");
		is_string(m->command, "NICK", "parsed NICK message from buffer");
		is_string(m->args[0], "johnny0", "parsed arg $1 from buffer");
		is_null(m->args[1], "no arg $2 from buffer message");

		m = buffer_msg(buf);
		isnt_null(m, "got a message from the buffer");
		is_int(buf->seek, 0, "buf->seek adjusted to exclude parsed line");
		is_string(buf->buf, "", "third (and final) line removed from buffer");
		is_string(m->command, "USER", "parsed USER message from buffer");
		is_string(m->args[0], "johnny0", "parsed arg $1 from buffer");
		is_string(m->args[1], "8", "parsed arg $2 from buffer");
		is_string(m->args[2], "*", "parsed arg $3 from buffer");
		is_string(m->args[3], "Johnny Zero", "parsed arg $4 from buffer");
		is_null(m->args[4], "no arg $4 from buffer message");

		m = buffer_msg(buf);
		is_null(m, "no more messages to parse from buffer");

		n = buffer_read(buf, fd);
		is_int(n, 0, "EOF reached on fd");
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
