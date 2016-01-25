//
// logsum -- a port of Sean Eddy's fast table-driven log sum
// This code was originally part of HMMER. This version is used with 
// Sean Eddy's permission as public domain code.
//
// Modification Notice
//
//   By: Matei David, Ontario Institute for Cancer Research
//   When: 2015
//   Reason:
//     Reorganized as C++ header-only library,
//     with implicit lookup table initialization.
//

/* p7_FLogsum() function used in the Forward() algorithm.
 * 
 * Contents:
 *    1. Floating point log sum.
 *    2. Benchmark driver.
 *    3. Unit tests.
 *    4. Test driver.
 *    5. Example.
 *    6. Copyright and license information.
 *
 * Exegesis:
 * 
 * Internally, HMMER3 profile scores are in nats: floating point
 * log-odds probabilities, with the log odds taken relative to
 * background residue frequencies, and the log to the base e.
 * 
 * The Forward algorithm needs to calculate sums of probabilities.
 * Given two log probabilities A and B, where s1 = \log
 * \frac{a}{f}, and s2 = \log \frac{b}{g}, we need to
 * calculate C = \log \frac{a + b}{h}.
 * 
 * The Forward algorithm guarantees that the null model denominator
 * terms f = g = h, because it is always concerned with summing terms
 * that describe different parses of the same target sequence prefix,
 * and the product of the background frequencies for the same sequence
 * prefix is a constant.
 * 
 * The naive solution is C = log(e^{A} + e^{B}), but this requires
 * expensive calls to log() and exp().
 * 
 * A better solution is C = A + log(1 + e^{-(A-B)}), for A >= B.  For
 * sufficiently small B << A, e^-{A-B} becomes less than the
 * machine's FLT_EPSILON, and C ~= A. (This is at about (A-B) >
 * -15.9, for the typical FLT_EPSILON of 1.2e-7.)
 * 
 * With some loss of accuracy [1], we can precalculate log(1 +
 * e^{-(A-B)}) for a discretized range of differences (A-B), and
 * compute C = A + table_lookup(A-B). This is what HMMER's
 * p7_FLogsum() function does.
 *
 * This only applies to the generic (serial) implementation.
 * See footnote [2] for discussion of why we remain unable to 
 * implement an efficient log-space SIMD vector implementation of
 * Forward.
 */

#ifndef __LOGSUM_HPP
#define __LOGSUM_HPP

#include <cassert>
#include <cmath>

/* p7_LOGSUM_SCALE defines the precision of the calculation; the
 * default of 1000.0 means rounding differences to the nearest 0.001
 * nat. p7_LOGSUM_TBL defines the size of the lookup table; the
 * default of 16000 means entries are calculated for differences of 0
 * to 16.000 nats (when p7_LOGSUM_SCALE is 1000.0).  e^{-p7_LOGSUM_TBL /
 * p7_LOGSUM_SCALE} should be on the order of the machine FLT_EPSILON,
 * typically 1.2e-7.
 */
#define p7_LOGSUM_TBL   16000
#define p7_LOGSUM_SCALE 1000.f
#define ESL_MAX(a,b)    (((a)>(b))?(a):(b))
#define ESL_MIN(a,b)    (((a)<(b))?(a):(b))
#define eslINFINITY     INFINITY
#define TRUE            1
#define FALSE           0
#define eslOK           1

namespace logsum
{

struct p7_FLogsum_Helper
{
    /* Function:  flogsum_lookup()
     * Synposis:  Holds the main lookup table used in logspace sum computations.
     *
     * Purpose:   Encapsulate static array inside a function to avoid the need for a separate definition.
     */
    static inline float*
    flogsum_lookup(void)
    {
        static float _flogsum_lookup[p7_LOGSUM_TBL]; /* p7_LOGSUM_TBL=16000: (A-B) = 0..16 nats, steps of 0.001 */
        return _flogsum_lookup;
    }

    /* Function:  p7_FLogsumInit()
     * Synopsis:  Initialize the p7_Logsum() function.
     *
     * Purpose:   Initialize the lookup table for <p7_FLogsum()>. 
     *            This function must be called once before any
     *            call to <p7_FLogsum()>.
     *            
     *            The precision of the lookup table is determined
     *            by the compile-time <p7_LOGSUM_TBL> constant.
     *
     * Returns:   <eslOK> on success.
     */
    static int
    p7_FLogsumInit(void)
    {
        static int firsttime = TRUE;
        if (!firsttime) return eslOK;
        firsttime = FALSE;

        int i;
        for (i = 0; i < p7_LOGSUM_TBL; i++)
        {
            flogsum_lookup()[i] = std::log(1. + std::exp((double) -i / p7_LOGSUM_SCALE));
        }

        return eslOK;
    }

    /* Function:  p7_FLogsum()
     * Synopsis:  Approximate $\log(e^a + e^b)$.
     *
     * Purpose:   Returns a fast table-driven approximation to
     *            $\log(e^a + e^b)$.
     *            
     *            Either <a> or <b> (or both) may be $-\infty$,
     *            but neither may be $+\infty$ or <NaN>.
     *
     * Note:      This function is a critical optimization target, because
     *            it's in the inner loop of generic Forward() algorithms.
     */
    static inline float
    p7_FLogsum(float a, float b)
    {
        // static object whose constructor initializes the lookup table
        static Table_Initializer _init;
        (void)_init;

        const float max = ESL_MAX(a, b);
        const float min = ESL_MIN(a, b);

        int idx = std::round((max-min)*p7_LOGSUM_SCALE);
        return (min == -eslINFINITY || idx >= p7_LOGSUM_TBL)
            ? max
            : max + flogsum_lookup()[idx];
    }

    /* Function:  p7_FLogsumError()
     * Synopsis:  Compute absolute error in probability from Logsum.
     *
     * Purpose:   Compute the absolute error in probability space
     *            resulting from <p7_FLogsum()>'s table lookup 
     *            approximation: approximation result - exact result.
     *                                                  
     *            This is of course computable analytically for
     *            any <a,b> given <p7_LOGSUM_TBL>; but the function
     *            is useful for some routines that want to determine
     *            if <p7_FLogsum()> has been compiled in its
     *            exact slow mode for debugging purposes. Testing
     *            <p7_FLogsumError(-0.4, -0.5) > 0.0001>
     *            for example, suffices to detect that the function
     *            is compiled in its fast approximation mode given
     *            the defaults. 
     */
    static float
    p7_FLogsumError(float a, float b)
    {
        float approx = p7_FLogsum(a,b);
        float exact  = std::log(std::exp(a) + std::exp(b));
        return (std::exp(approx) - std::exp(exact));
    }

    /* Struct:  Table_Initializer
     * Purpose: Initialize the lookup table on construction.
     *
     */
    struct Table_Initializer
    {
        Table_Initializer()
        {
            p7_FLogsumInit();
        }
    }; // struct Table_Initializer
}; // struct p7_FLogsum_Helper

/*
 * Publish main methods outside of the helper struct.
 */
inline float p7_FLogsum(float a, float b) { return p7_FLogsum_Helper::p7_FLogsum(a, b); }
inline float p7_FLogsumError(float a, float b) { return p7_FLogsum_Helper::p7_FLogsumError(a, b); }

} // namespace logsum

#endif

#ifdef SAMPLE_p7LOGSUM

/*

g++ -std=c++11 -DSAMPLE_p7LOGSUM -x c++ logsum.hpp -o sample-logsum

*/

#include <iostream>
#include <sstream>

using namespace std;
using namespace logsum;

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cerr << "use: " << argv[0] << " <a> <b>" << endl;
        return EXIT_FAILURE;
    }

    float a;
    float b;
    float result;

    istringstream(argv[1]) >> a;
    istringstream(argv[2]) >> b;

    result = p7_FLogsum(a, b);
    cout << "p7_FLogsum(" << a << ", " << b << ") = " << result << endl;

    result = log(exp(a) + exp(b));
    cout << "log(exp(" << a << ") + exp(" << b << ")) = " << result << endl;

    cout << "Absolute error in probability: " << p7_FLogsumError(a, b) << endl;
}

#endif

#ifdef BENCHMARK_p7LOGSUM

/*

Compare with the original logsum.{h,cpp} (not included in this repo).
Assuming they are in /somedir.

Compile:
DIR=/somedir; g++ -std=c++11 -DBENCHMARK_p7LOGSUM -x c++ -I${DIR} logsum.hpp ${DIR}/logsum.cpp -o benchmark-logsum

Run:
./benchmark-logsum 42 0
./benchmark-logsum 42 1

*/

#include <chrono>
#include <iostream>
#include <random>
#include <deque>
#include <sstream>
#include <vector>

using namespace std;
using namespace logsum;

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cerr << "use: " << argv[0] << " <seed> <version>" << endl
             << "where <version> means:" << endl
             << "  0: use exp&log" << endl
             << "  1: use table lookup" << endl;
        return EXIT_FAILURE;
    }
    size_t seed = 0;
    unsigned version = 0;
    istringstream(argv[1]) >> seed;
    istringstream(argv[2]) >> version;
    if (seed == 0)
    {
        seed = chrono::high_resolution_clock::now().time_since_epoch().count();
    }
    clog << "seed: " << seed << endl;
    clog << "version: " << version << endl;
    const unsigned n = 100000000;
    std::deque< std::pair< float, float > > s;
    mt19937 rg(seed);
    uniform_real_distribution< float > unif;
    for (unsigned i = 0; i < n; ++i)
    {
        s.emplace_back(unif(rg), unif(rg));
    }
    float res = 0.0;

    // start timer
    auto start_time = chrono::high_resolution_clock::now();

    if (version == 0)
    {
        for (unsigned i = 0; i < s.size(); ++i)
        {
            float& a = s[i].first;
            float& b = s[i].second;
            res = std::log(std::exp(a) + std::exp(b));
            (void)res;
        }
    }
    else if (version == 1)
    {
        for (unsigned i = 0; i < s.size(); ++i)
        {
            float& a = s[i].first;
            float& b = s[i].second;
            res = logsum::p7_FLogsum(a, b);
            (void)res;
        }
    }

    // end timer
    auto end_time = chrono::high_resolution_clock::now();

    cout << "time: " << chrono::duration_cast< chrono::milliseconds >(end_time - start_time).count() << endl;
}

#endif
