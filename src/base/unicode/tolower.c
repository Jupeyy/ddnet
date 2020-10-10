#include <stdlib.h>

#include "tolower_data.h"

static int compul(const void *a, const void *b)
{
	const struct UPPER_LOWER *ul_a = (const struct UPPER_LOWER *)a;
	const struct UPPER_LOWER *ul_b = (const struct UPPER_LOWER *)b;
	return ul_a->upper - ul_b->upper;
}

int str_utf8_tolower(int code)
{
	struct UPPER_LOWER key;
	struct UPPER_LOWER *res;
	key.upper = code;
	res = bsearch(&key, tolower, NUM_TOLOWER, sizeof(struct UPPER_LOWER), compul);

	if(res == NULL)
		return code;
	return res->lower;
}
