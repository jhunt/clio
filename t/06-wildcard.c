#include "test.h"

TESTS {
	ok( wildcard("literal", "literal"), "literal =~ literal");
	ok( wildcard("", ""), "nil =~ nil");
	ok(!wildcard("literal", ""), "literal !~ nil");
	ok(!wildcard("", "literal"), "nil !~ literal");

	ok( wildcard("x", "?"), "x =~ ?");
	ok( wildcard("?", "?"), "? =~ ?");
	ok(!wildcard("?", "x"), "? !~ x");

	ok( wildcard("lol", "?o?"), "lol =~ ?o?");
	ok( wildcard("foo", "?o?"), "foo =~ ?o?");
	ok(!wildcard("f00", "?o?"), "f00 !~ ?o?");

	ok( wildcard("stuff", "st?ff"), "stuff =~ st?ff");
	ok( wildcard("staff", "st?ff"), "staff =~ st?ff");
	ok( wildcard("stiff", "st?ff"), "stiff =~ st?ff");
	ok(!wildcard("things", "st?ff"), "things !~ st?ff");

	ok( wildcard("anything", "*"), "anything =~ *");
	ok( wildcard("", "*"), "nil =~ *");

	ok( wildcard("foonly", "foo*"), "foonly =~ foo*");
	ok( wildcard("foobar", "foo*"), "foobar =~ foo*");
	ok( wildcard("foo", "foo*"), "foo =~ foo*");

	ok( wildcard("foobar", "f*bar"), "foobar =~ f*bar");
	ok( wildcard("fubar", "f*bar"), "fubar =~ f*bar");
	ok(!wildcard("foosball", "f*bar"), "foosball !~ f*bar");
	ok( wildcard("foosbacll bar", "f*bar"), "foosball bar =~ f*bar");

	ok( wildcard("*star*", "\\*star\\*"), "*star* =~ \\*star\\*");
	ok(!wildcard("ustart", "\\*star\\*"), "ustart !~ \\*star\\*");

	ok( wildcard("orly?", "orly\\?"), "orly? =~ orly\\?");
	ok(!wildcard("orlo",  "orly\\?"), "orlo !~ orly\\?");

	ok( wildcard("dipdeedoodah", "dip*da*"), "dipdeedoodah =~ dip*da*");
	ok(!wildcard("dipdeedoodah", "dip*dX*"), "dipdeedoodah !~ dip*dX*");
	ok( wildcard("dipdeedoodah", "*dee*"), "dipdeedoodah =~ *dee*");

	done_testing();
}
