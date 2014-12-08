AUTOMAKE_OPTIONS = foreign subdir-objects
ACLOCAL_AMFLAGS = -I build

AM_CFLAGS =
AM_CFLAGS += -Wall
AM_CFLAGS += -g
AM_CFLAGS += @GCOV_CFLAGS@

CORE_SRC =
CORE_SRC += src/irc.h
CORE_SRC += src/numerics.h
CORE_SRC += src/bits.h
CORE_SRC += src/irc.c
CORE_SRC += src/manager.h
CORE_SRC += src/manager.c

sbin_PROGRAMS = ircd
ircd_SOURCES = $(CORE_SRC) src/server.c

TEST_SRC =
TEST_SRC += t/test.h

CTAP_TESTS =
CTAP_TESTS += t/00-parse
CTAP_TESTS += t/01-memory
CTAP_TESTS += t/06-wildcard
CTAP_TESTS += t/11-manager

TESTS = $(CTAP_TESTS)

check_PROGRAMS = $(CTAP_TESTS)
t_00_parse_SOURCES    = $(CORE_SRC) $(TEST_SRC) t/00-parse.c
t_01_memory_SOURCES   = $(CORE_SRC) $(TEST_SRC) t/01-memory.c
t_06_wildcard_SOURCES = $(CORE_SRC) $(TEST_SRC) t/06-wildcard.c
t_11_manager_SOURCES  = $(CORE_SRC) $(TEST_SRC) t/11-manager.c
t_11_manager_LDADD    = -lpthread




.PHONY: coverage-clean coverage-report coverage clean-local-cov
clean-local: clean-local-gcda
clean-local-gcda:
	touch sac.gcda
	find . -name '*.gc??' | xargs rm

if GCOV_ENABLED
coverage-clean:
	@rm -fr coverage
	@find . -name "*.gcda" -exec rm {} \;
	@lcov --directory . --zerocounters

coverage-report:
	@mkdir -p coverage
	@lcov --compat-libtool --directory . --base-directory . --capture --output-file coverage/app.info
	@genhtml -o coverage/ coverage/app.info

coverage:
	@make coverage-report

clean-local-cov:
	@make coverage-clean

check:
	@make coverage

else
coverage:
	@echo >&2 "nothin doin"
clean-local-cov:
endif

# aliases
test: check
