#include "test.h"

TESTS {
	msg_t *m;
	const char *c;
	size_t i;

	c = "COMMAND";
	m = irc_parse_msg(c, strlen(c));
	isnt_null(m, "'%s' - parsed successfully", c);
	is_string(m->raw, c, "'%s' - raw command retained", c);
	is_null(m->prefix, "'%s' - no prefix", c);
	is_string(m->command, "COMMAND", "'%s' - command string", c);
	for (i = 0; i < MAX_ARGS; i++)
		is_null(m->args[i], "'%s' - no arg $%i", c, i+1);


	c = "COMMAND ARG1";
	m = irc_parse_msg(c, strlen(c));
	isnt_null(m, "'%s' - parsed successfully", c);
	is_string(m->raw, c, "'%s' - raw command retained", c);
	is_null(m->prefix, "'%s' - no prefix", c);
	is_string(m->command, "COMMAND", "'%s' - command string", c);
	is_string(m->args[0], "ARG1", "'%s' - first argument", c);
	for (i = 1; i < MAX_ARGS; i++)
		is_null(m->args[i], "'%s' - no arg $%i", c, i+1);


	c = "COMMAND one two three four five six seven eight nine ten eleven twelve thirteen fourteen fifteen";
	m = irc_parse_msg(c, strlen(c));
	isnt_null(m, "'%s' - parsed successfully", c);
	is_string(m->raw, c, "'%s' - raw command retained", c);
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
	m = irc_parse_msg(c, strlen(c));
	isnt_null(m, "'%s' - parsed successfully", c);
	is_string(m->raw, c, "'%s' - raw command retained", c);
	is_string(m->prefix, "from", "'%s' - prefix", c);
	is_string(m->command, "COMMAND", "'%s' - command string", c);
	for (i = 0; i < MAX_ARGS; i++)
		is_null(m->args[i], "'%s' - no arg $%i", c, i+1);

	c = ":from COMMAND ARG1";
	m = irc_parse_msg(c, strlen(c));
	isnt_null(m, "'%s' - parsed successfully", c);
	is_string(m->raw, c, "'%s' - raw command retained", c);
	is_string(m->prefix, "from", "'%s' - prefix", c);
	is_string(m->command, "COMMAND", "'%s' - command string", c);
	is_string(m->args[0], "ARG1", "'%s' - first argument", c);
	is_null(m->args[1], "'%s' - no arg $2", c);


	c = ":from COMMAND one two three four five six seven eight nine ten eleven twelve thirteen fourteen fifteen";
	m = irc_parse_msg(c, strlen(c));
	isnt_null(m, "'%s' - parsed successfully", c);
	is_string(m->raw, c, "'%s' - raw command retained", c);
	is_string(m->prefix, "from", "'%s' - prefix", c);
	is_string(m->command, "COMMAND", "'%s' - command string", c);
	is_string(m->args[0], "one", "'%s' - first argument", c);
	is_string(m->args[14], "fifteen", "'%s' - fifteenth argument", c);


	c = "MSG nick :this is a really long message";
	m = irc_parse_msg(c, strlen(c));
	isnt_null(m, "'%s' - parsed successfully", c);
	is_string(m->raw, c, "'%s' - raw command retained", c);
	is_null(m->prefix, "'%s' - no prefix", c);
	is_string(m->command, "MSG", "'%s' - command string", c);
	is_string(m->args[0], "nick", "'%s' - first argument", c);
	is_string(m->args[1], "this is a really long message", "'%s' - second (last) argument", c);
	is_null(m->args[2], "'%s' - no arg $3", c);


	done_testing();
}
