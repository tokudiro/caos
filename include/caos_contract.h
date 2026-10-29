#ifndef CAOS_CONTRACT_H__
#define CAOS_CONTRACT_H__
/*
 * The macros used by the code that caos generates for #require / #ensure.
 * Define CAOS_REQUIRE and CAOS_ENSURE before this header (or write your own
 * caos_contract.h) to use your own handler; otherwise assert() is used, so
 * NDEBUG removes the checks.
 */
#if !defined(CAOS_REQUIRE) || !defined(CAOS_ENSURE)
#include <assert.h>
#endif

#ifndef CAOS_REQUIRE
#define CAOS_REQUIRE(cond) assert(cond)
#endif
#ifndef CAOS_ENSURE
#define CAOS_ENSURE(cond) assert(cond)
#endif

#endif /* CAOS_CONTRACT_H__ */
