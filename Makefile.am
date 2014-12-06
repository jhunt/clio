AUTOMAKE_OPTIONS = foreign subdir-objects
ACLOCAL_AMFLAGS = -I build

AM_CFLAGS =
AM_CFLAGS += -Wall
AM_CFLAGS += -g

CORE_SRC =
CORE_SRC += src/irc.h src/irc.c

sbin_PROGRAMS = ircd
ircd_SOURCES = $(CORE_SRC) src/core.c

TEST_SRC =
TEST_SRC += t/test.h

CTAP_TESTS =
CTAP_TESTS += t/05-msg
CTAP_TESTS += t/06-wildcard

check_PROGRAMS = $(CTAP_TESTS)
t_05_msg_SOURCES      = $(CORE_SRC) $(TEST_SRC) t/05-msg.c
t_06_wildcard_SOURCES = $(CORE_SRC) $(TEST_SRC) t/06-wildcard.c

