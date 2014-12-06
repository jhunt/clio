#include <stdio.h>
#include <string.h>
#include "irc.h"

#define strlen0(x) (int)((x) ? strlen(x) : 0)
#define str0(s) ((s) ? (s) : "")

int main (int argc, char **argv)
{
	char buf[8192];
	while (fgets(buf, 8192, stdin)) {
		size_t len = strlen(buf);

		if (len && buf[len - 1] == '\n')
			buf[--len] = '\0';

		if (len == 0)
			continue;

		msg_t *m = irc_parse_msg(buf, len);
		if (!m) {
			printf("!! parse failed\n%s\n", buf);

		} else {
			printf("++ parse ok\n");
			printf("raw: %4i [%s]\n", strlen0(m->raw), str0(m->raw));
			printf("pre: %4i [%s]\n", strlen0(m->prefix), str0(m->prefix));
			printf("cmd: %4i [%s]\n", strlen0(m->command), str0(m->command));
			size_t i;
			for (i = 0; i < MAX_ARGS; i++)
				printf(" %2i: %4i [%s]\n", (int)(i+1), strlen0(m->args[i]), str0(m->args[i]));
		}

		printf("\n\n");
	}

	return 0;
}
