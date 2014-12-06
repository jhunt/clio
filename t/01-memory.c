#include "test.h"

typedef struct {
	char *name;
	int   flags;
} thing_t;

static int RESETS = 0;
void reset_thing(void *d)
{
	thing_t *t = (thing_t*)d;
	t->name = NULL; /* only expecting const strings */
	t->flags = 0;

	RESETS++;
}

TESTS {
	subtest {
		pool_t pool;
		size_t i;

		pool_init(&pool, 5, sizeof(thing_t), NULL);
		is_int(pool.total, 5, "pool now has 5 slots");
		for (i = 0; i < 5; i++)
			is_int(pool.used[i], 0, "pool slot %i is not in use", i);

		thing_t *x0 = pool_acq(&pool);
		isnt_null(x0, "retrieved a pointer from the pool");
		is_pointer(x0, pool.data, "new pointer is the first cell");
		for (i = 0; i < 1; i++)
			is_int(pool.used[i], 1, "pool slot %i is in use", i);
		for (i = 1; i < 5; i++)
			is_int(pool.used[i], 0, "pool slot %i is not in use", i);

		thing_t *x1 = pool_acq(&pool);
		isnt_null(x1, "retrieved a pointer from the pool");
		is_pointer(x1, pool.data + (sizeof(thing_t) * 1), "new pointer is the second cell");
		for (i = 0; i < 2; i++)
			is_int(pool.used[i], 1, "pool slot %i is in use", i);
		for (i = 2; i < 5; i++)
			is_int(pool.used[i], 0, "pool slot %i is not in use", i);

		thing_t *x2 = pool_acq(&pool);
		isnt_null(x2, "retrieved a pointer from the pool");
		is_pointer(x2, pool.data + (sizeof(thing_t) * 2), "new pointer is the third cell");
		for (i = 0; i < 3; i++)
			is_int(pool.used[i], 1, "pool slot %i is in use", i);
		for (i = 3; i < 5; i++)
			is_int(pool.used[i], 0, "pool slot %i is not in use", i);

		thing_t *x3 = pool_acq(&pool);
		isnt_null(x3, "retrieved a pointer from the pool");
		is_pointer(x3, pool.data + (sizeof(thing_t) * 3), "new pointer is the fourth cell");
		for (i = 0; i < 4; i++)
			is_int(pool.used[i], 1, "pool slot %i is in use", i);
		for (i = 4; i < 5; i++)
			is_int(pool.used[i], 0, "pool slot %i is not in use", i);

		thing_t *x4 = pool_acq(&pool);
		isnt_null(x4, "retrieved a pointer from the pool");
		is_pointer(x4, pool.data + (sizeof(thing_t) * 4), "new pointer is the fifth cell");
		for (i = 0; i < 5; i++)
			is_int(pool.used[i], 1, "pool slot %i is in use", i);

		thing_t *x5 = pool_acq(&pool);
		is_null(x5, "failed to retrieve pointer from fully-allocated pool");
		for (i = 0; i < 5; i++)
			is_int(pool.used[i], 1, "pool slot %i is in use", i);
	}

	subtest { /* pool without reset function leaves memory intact */
		pool_t pool;
		pool_init(&pool, 1, sizeof(thing_t), NULL);
		is_int(pool.used[0], 0, "slot 0 not in use");

		thing_t *first = pool_acq(&pool);
		isnt_null(first, "got a thing from the pool");
		is_int(pool.used[0], 1, "slot 0 is in use");
		is_null(first->name, "new thing initially has no name");
		is_int(first->flags, 0, "new thing initially has no flags");

		first->name = "a thing";
		first->flags = 42;

		is_null(pool_acq(&pool), "pool is full");
		pool_rel(&pool, first);
		is_int(pool.used[0], 0, "slot 0 no longer in use (released)");

		thing_t *second = pool_acq(&pool);
		is_pointer(first, second, "pool_acq can re-use pointers");
		is_string(second->name, "a thing",
				"cell memory (name) persists across release/acquire (no callback)");
		is_int(second->flags, 42,
				"cell memory (flags) persists across release/acquire (no callback)");
	}

	subtest { /* pool with reset function */
		pool_t pool;
		pool_init(&pool, 1, sizeof(thing_t), reset_thing);
		is_int(pool.used[0], 0, "slot 0 not in use");

		thing_t *first = pool_acq(&pool);
		isnt_null(first, "got a thing from the pool");
		is_int(pool.used[0], 1, "slot 0 is in use");
		is_null(first->name, "new thing initially has no name");
		is_int(first->flags, 0, "new thing initially has no flags");

		first->name = "a thing";
		first->flags = 42;

		is_null(pool_acq(&pool), "pool is full");
		RESETS = 0;
		pool_rel(&pool, first);
		is_int(pool.used[0], 0, "slot 0 no longer in use (released)");
		is_int(RESETS, 1, "reset_thing called exactly once");

		thing_t *second = pool_acq(&pool);
		is_pointer(first, second, "pool_acq can re-use pointers");
		is_null(second->name, "refurbed thing has no name");
		is_int(second->flags, 0, "refurbed thing has no flags");
	}

	done_testing();
}
