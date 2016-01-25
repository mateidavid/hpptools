//
// logdiff -- inspired by Sean Eddy's fast table-driven log sum
//
// The MIT License (MIT)
//
// (C) Matei David, Ontario Institute for Cancer Research, 2015
//

#ifndef __LOGDIFF_HPP
#define __LOGDIFF_HPP

#include <algorithm>
#include <cassert>
#include <cmath>

/*
 * LOGDIFF_SCALE defines the precision of the calculation; the
 * default of 1000.0 means rounding differences to the nearest 0.001
 * nat. LOGDIFF_TBL defines the size of the lookup table; the
 * default of 16000 means entries are calculated for differences of 0
 * to 16.000 nats (when LOGDIFF_SCALE is 1000.0).
 */
#define LOGDIFF_TBL   16000
#define LOGDIFF_SCALE 1000.0f

namespace logdiff
{

namespace detail
{

struct LogDiff_Helper
{
    /* Function:  table()
     * Synposis:  Holds the main lookup table used in logspace diff computations.
     *
     * Purpose:   Encapsulate static array inside a function to avoid the need for a separate definition.
     */
    static inline float* table(void)
    {
        static float _table[LOGDIFF_TBL]; /* LOGDIFF_TBL=16000: (A-B) = 0..16 nats, steps of 0.001 */
        return _table;
    }

    /* Function:  init()
     * Synopsis:  Initialize the logdiff table.
     *
     * Purpose:   Initialize the logdiff lookup table for logdiff().
     *            This function must be called once before any
     *            call to logdiff().
     *
     * Note:      The precision of the lookup table is determined
     *            by the compile-time <LOGDIFF_TBL> constant.
     *
     * Returns:   true on success.
     */
    static bool init()
    {
        static bool first_time = true;
        if (not first_time)
        {
            return true;
        }
        first_time = false;

        for (unsigned i = 0; i < LOGDIFF_TBL; i++)
        {
            auto x = -((double) i / LOGDIFF_SCALE);
            auto e_x = std::exp(x);
            table()[i] = std::log(1.0 - e_x);
        }

        return true;
    }

    /* Function:  logdiff()
     * Synopsis:  Approximate $\log(e^a - e^b)$.
     *
     * Purpose:   Returns a fast table-driven approximation to
     *            $\log(e^a - e^b)$.
     *
     *            Either <a> or <b> (or both) may be $-\infty$,
     *            but neither may be $+\infty$ or <NaN>.
     *
     * Note:      This function is a critical optimization target, because
     *            it's in the inner loop of generic Forward() algorithms.
     */
    static inline float logdiff(float a, float b)
    {
        if (a < b) std::swap(a, b);
        return logdiff_nocomp(a, b);
    }
    static inline float logdiff_nocomp(float a, float b)
    {
        // static object whose constructor initializes the lookup table
        static Table_Initializer _init;
        (void)_init;

        unsigned idx = std::round((a - b) * LOGDIFF_SCALE);
        return (b == -INFINITY or idx >= LOGDIFF_TBL)
            ? a
            : a + table()[idx];
    }

    /* Function:  logdiff_error()
     * Synopsis:  Compute absolute error in probability from logdiff.
     *
     * Purpose:   Compute the absolute error in probability space
     *            resulting from logdiff()'s table lookup:
     *            approximation result - exact result.
     */
    static float logdiff_error(float a, float b)
    {
        if (a < b) std::swap(a, b);
        return logdiff_error_nocomp(a, b);
    }
    static float logdiff_error_nocomp(float a, float b)
    {
        assert(a >= b);
        if (a == b) return 0.0;
        float approx = logdiff_nocomp(a, b);
        float exact  = std::log(std::exp(a) - std::exp(b));
        return std::exp(approx) - std::exp(exact);
    }

    /* Struct:  Table_Initializer
     * Purpose: Initialize the lookup table on construction.
     */
    struct Table_Initializer
    {
        Table_Initializer()
        {
            init();
        }
    }; // struct Table_Initializer
}; // struct LogDiff_Helper

} // namespace detail

/*
 * Publish main methods outside of the helper struct.
 */
inline float LogDiff(float a, float b) { return detail::LogDiff_Helper::logdiff(a, b); }
inline float LogDiffError(float a, float b) { return detail::LogDiff_Helper::logdiff_error(a, b); }

} // namespace logdiff

#endif

#ifdef SAMPLE_LOGDIFF

/*

g++ -std=c++11 -DSAMPLE_LOGDIFF -x c++ logdiff.hpp -o sample-logdiff

*/

#include <iostream>
#include <sstream>

using namespace std;
using namespace logdiff;

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

    result = LogDiff(a, b);
    cout << "logdiff(" << a << ", " << b << ") = " << result << endl;

    result = log(exp(a) - exp(b));
    cout << "log(exp(" << a << ") - exp(" << b << ")) = " << result << endl;

    cout << "Absolute error in probability: " << LogDiffError(a, b) << endl;
}

#endif
