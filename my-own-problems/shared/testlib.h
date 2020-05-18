/* 
 * It is strictly recommended to include "testlib.h" before any other include 
 * in your code. In this case testlib overrides compiler specific "random()".
 *
 * If you can't compile your code and compiler outputs something about 
 * ambiguous call of "random_shuffle", "rand" or "srand" it means that 
 * you shouldn't use them. Use "shuffle", and "rnd.next()" instead of them
 * because these calls produce stable result for any C++ compiler. Read 
 * sample generator sources for clarification.
 *
 * Please read the documentation for class "random_t" and use "rnd" instance in
 * generators. Probably, these sample calls will be usefull for you:
 *              rnd.next(); rnd.next(100); rnd.next(1, 2); 
 *              rnd.next(3.14); rnd.next("[a-z]{1,100}").
 *
 * Also read about wnext() to generate off-center random distribution.
 *
 * See https://github.com/MikeMirzayanov/testlib/ to get latest version or bug tracker.
 */

#ifndef _TESTLIB_H_
#define _TESTLIB_H_

/*
 * Copyright (c) 2005-2019
 */

#define VERSION "0.9.23-SNAPSHOT"

/* 
 * Mike Mirzayanov
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted 
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

#include <iostream>
#include <climits>

const long long __TESTLIB_LONGLONG_MAX = 9223372036854775807LL;

static void __testlib_fail(const std::string& message)
{
    std::cerr << "Fatal Error: " << message << std::endl;
    assert(false);
}

template<typename T>
static inline T __testlib_min(const T& a, const T& b)
{
    return a < b ? a : b;
}

template<typename T>
static inline T __testlib_max(const T& a, const T& b)
{
    return a > b ? a : b;
}

/* 
 * Use random_t instances to generate random values. It is preffered
 * way to use randoms instead of rand() function or self-written 
 * randoms.
 *
 * Testlib defines global variable "rnd" of random_t class.
 * Use registerGen(argc, argv, 1) to setup random_t seed be command
 * line (to use latest random generator version).
 *
 * Random generates uniformly distributed values if another strategy is
 * not specified explicitly.
 */
class random_t
{
private:
    unsigned long long seed;
    static const unsigned long long multiplier;
    static const unsigned long long addend;
    static const unsigned long long mask;
    static const int lim;

    long long nextBits(int bits) 
    {
        if (bits <= 48)
        {
            seed = (seed * multiplier + addend) & mask;
            return (long long)(seed >> (48 - bits));
        }
        else
        {
            if (bits > 63)
                __testlib_fail("random_t::nextBits(int bits): n must be less than 64");

            int lowerBitCount = (random_t::version == 0 ? 31 : 32);
            
            long long left = (nextBits(31) << 32);
            long long right = nextBits(lowerBitCount);
            
            return left ^ right;
        }
    }

public:
    static int version;

    /* New random_t with fixed seed. */
    random_t()
        : seed(3905348978240129619LL)
    {
    }

    /* Sets seed by given value. */ 
    void setSeed(long long _seed)
    {
        _seed = (_seed ^ multiplier) & mask;
        seed = _seed;
    }

    /* Random value in range [0, n-1]. */
    int next(int n)
    {
        if (n <= 0)
            __testlib_fail("random_t::next(int n): n must be positive");

        if ((n & -n) == n)  // n is a power of 2
            return (int)((n * (long long)nextBits(31)) >> 31);

        const long long limit = INT_MAX / n * n;
        
        long long bits;
        do {
            bits = nextBits(31);
        } while (bits >= limit);

        return int(bits % n);
    }

    /* Random value in range [0, n-1]. */
    unsigned int next(unsigned int n)
    {
        if (n >= INT_MAX)
            __testlib_fail("random_t::next(unsigned int n): n must be less INT_MAX");
        return (unsigned int)next(int(n));
    }

    /* Random value in range [0, n-1]. */
    long long next(long long n) 
    {
        if (n <= 0)
            __testlib_fail("random_t::next(long long n): n must be positive");

        const long long limit = __TESTLIB_LONGLONG_MAX / n * n;
        
        long long bits;
        do {
            bits = nextBits(63);
        } while (bits >= limit);

        return bits % n;
    }

    /* Random value in range [0, n-1]. */
    unsigned long long next(unsigned long long n)
    {
        if (n >= (unsigned long long)(__TESTLIB_LONGLONG_MAX))
            __testlib_fail("random_t::next(unsigned long long n): n must be less LONGLONG_MAX");
        return (unsigned long long)next((long long)(n));
    }

    /* Random value in range [0, n-1]. */
    long next(long n)
    {
        return (long)next((long long)(n));
    }

    /* Random value in range [0, n-1]. */
    unsigned long next(unsigned long n)
    {
        if (n >= (unsigned long)(LONG_MAX))
            __testlib_fail("random_t::next(unsigned long n): n must be less LONG_MAX");
        return (unsigned long)next((unsigned long long)(n));
    }

    /* Returns random value in range [from,to]. */
    int next(int from, int to)
    {
        return int(next((long long)to - from + 1) + from);
    }

    /* Returns random value in range [from,to]. */
    unsigned int next(unsigned int from, unsigned int to)
    {
        return (unsigned int)(next((long long)to - from + 1) + from);
    }

    /* Returns random value in range [from,to]. */
    long long next(long long from, long long to)
    {
        return next(to - from + 1) + from;
    }

    /* Returns random value in range [from,to]. */
    unsigned long long next(unsigned long long from, unsigned long long to)
    {
        if (from > to)
            __testlib_fail("random_t::next(unsigned long long from, unsigned long long to): from can't not exceed to");
        return next(to - from + 1) + from;
    }

    /* Returns random value in range [from,to]. */
    long next(long from, long to)
    {
        return next(to - from + 1) + from;
    }

    /* Returns random value in range [from,to]. */
    unsigned long next(unsigned long from, unsigned long to)
    {
        if (from > to)
            __testlib_fail("random_t::next(unsigned long from, unsigned long to): from can't not exceed to");
        return next(to - from + 1) + from;
    }

    /* Random double value in range [0, 1). */
    double next() 
    {
        long long left = ((long long)(nextBits(26)) << 27);
        long long right = nextBits(27);
        return (double)(left + right) / (double)(1LL << 53);
    }

    /* Random double value in range [0, n). */
    double next(double n)
    {
        return n * next();
    }

    /* Random double value in range [from, to). */
    double next(double from, double to)
    {
        if (from > to)
            __testlib_fail("random_t::next(double from, double to): from can't not exceed to");
        return next(to - from) + from;
    }

    template <typename Container>
    typename Container::value_type nextFrom(const Container& c)
    {
        assert(!c.empty());

        return c[next(static_cast<int>(c.size()))];
    };

};

const int random_t::lim = 25;
const unsigned long long random_t::multiplier = 0x5DEECE66DLL;
const unsigned long long random_t::addend = 0xBLL;
const unsigned long long random_t::mask = (1LL << 48) - 1;
inline int random_t::version = -1;
inline random_t rnd;

/*
 * Do not use random_shuffle, because it will produce different result
 * for different C++ compilers.
 *
 * This implementation uses testlib random_t to produce random numbers, so
 * it is stable.
 */
template<typename _RandomAccessIter>
void shuffle(_RandomAccessIter __first, _RandomAccessIter __last)
{
    if (__first == __last) return;
    for (_RandomAccessIter __i = __first + 1; __i != __last; ++__i)
        std::iter_swap(__i, __first + rnd.next(int(__i - __first) + 1));
}


template<typename _RandomAccessIter>
#if defined(__GNUC__) && !defined(__clang__)
__attribute__ ((error("Don't use random_shuffle(), use shuffle() instead")))
#endif
void random_shuffle(_RandomAccessIter , _RandomAccessIter )
{
    assert("Don't use random_shuffle(), use shuffle() instead");
}




#endif
