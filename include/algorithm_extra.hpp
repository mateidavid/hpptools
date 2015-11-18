/// @author    Matei David, Ontario Institute for Cancer Research
/// @version   1.0
/// @date      2015
/// @copyright MIT Public License
///
/// Extensions of various standard algorithms.

#ifndef __ALGORITHM_EXTRA_HPP
#define __ALGORITHM_EXTRA_HPP

#include <algorithm>
#include <type_traits>

namespace detail
{

struct Identity {
    //template < typename U >
    //constexpr auto operator () (U&& v) const noexcept -> decltype(std::forward< U >(v))
    //{
    //    return std::forward< U >(v);
    //}
    template < typename U >
    U operator () (const U& v) const { return v; }
}; // struct Identity

} //namespace detail

/**
 * Apply function to elements in range, but advance iterator first; iterator version.
 * With this, it is safe to apply a function which might indirectly remove
 * the pointed element from a list.
 * @param first Start of range.
 * @param last End of range.
 * @param f Unary function taking an element as argument.
 * @return f
 */
template < class Input_Iterator,
           class Unary_Function >
Unary_Function
for_each_advance(Input_Iterator first, Input_Iterator last,
                 Unary_Function&& fn)
{
    while (first != last)
    {
        fn(*(first++));
    }
    return fn;
}

/**
 * Apply function to elements in range, but advance iterator first; range version.
 * With this, it is safe to apply a function which might indirectly remove
 * the pointed element from a list.
 * @param rg Range.
 * @param f Unary function taking an element as argument.
 * @return f
 */
template < class Forward_Range,
           class Unary_Function >
Unary_Function
for_each_advance(Forward_Range&& rg,
                 Unary_Function&& fn)
{
    return for_each_advance(rg.begin(), rg.end(), fn);
}

/**
 * Apply function to iterators in range; iterator version.
 * @param first Start of range.
 * @param last End of range.
 * @param f Unary function taking an iterator as argument.
 * @return f
 */
template < class Input_Iterator,
           class Unary_Function >
Unary_Function
for_each_it(Input_Iterator first, Input_Iterator last,
            Unary_Function&& fn)
{
    while (first != last)
    {
        fn(first);
        ++first;
    }
    return fn;
}

/**
 * Apply function to iterators in range; range version.
 * @param first Start of range.
 * @param last End of range.
 * @param f Unary function taking an iterator as argument.
 * @return f
 */
template < class Forward_Range,
           class Unary_Function >
Unary_Function
for_each_it(Forward_Range&& rg,
            Unary_Function&& fn)
{
    return for_each_it(rg.begin(), rg.end(), fn);
}

/**
 * Apply function to iterators in range, but advance iterator first; iterator version.
 * With this, it is safe to remove elements from a list on the go.
 * @param first Start of range.
 * @param last End of range.
 * @param f Unary function taking an element as argument.
 * @return f
 */
template < class Input_Iterator,
           class Unary_Function >
Unary_Function
for_each_it_advance(Input_Iterator first, Input_Iterator last,
                    Unary_Function&& fn)
{
    while (first != last)
    {
        fn(first++);
    }
    return fn;
}

/**
 * Apply function to iterators in range, but advance iterator first; iterator version.
 * With this, it is safe to remove elements from a list on the go.
 * @param first Start of range.
 * @param last End of range.
 * @param f Unary function taking an element as argument.
 * @return f
 */
template < class Forward_Range,
           class Unary_Function >
Unary_Function
for_each_it_advance(Forward_Range&& rg,
                    Unary_Function&& fn)
{
    return for_each_it_advance(rg.begin(), rg.end(), fn);
}

/**
 * Return iterator to minimum element in range; iterator version.
 * @param first Range begin
 * @param last Range end
 * @param f Use f to obtain key from value.
 */
template < class Forward_Iterator,
           class Unary_Function = detail::Identity >
Forward_Iterator
min_of(Forward_Iterator first, Forward_Iterator last,
       Unary_Function&& f = Unary_Function())
{
    if (first == last) return last;
    auto min_it = first++;
    for (auto it = first; it != last; ++it)
    {
        if (f(*it) < f(*min_it))
        {
            min_it = it;
        }
    }
    return min_it;
}

/**
 * Return iterator to minimum element in range; range version.
 * @param rg Range
 * @param f Use f to obtain key from value.
 */
template < class Forward_Range,
           class Unary_Function = detail::Identity >
auto
min_of(Forward_Range&& rg,
       Unary_Function&& f = Unary_Function())
    -> decltype(rg.end())
{
    static_assert(not std::is_rvalue_reference< Forward_Range >::value,
                  "rvalue reference to range not allowed: returned iterator might not exist");
    return min_of(rg.begin(), rg.end(), f);
}

/**
 * Return minimum element in range; iterator version.
 * @param first Range begin
 * @param last Range end
 * @param f Use f to obtain key from value.
 */
template < class Forward_Iterator,
           class Unary_Function = detail::Identity >
typename std::result_of< Unary_Function(typename Forward_Iterator::value_type) >::type
min_value_of(Forward_Iterator first, Forward_Iterator last,
             Unary_Function&& f = Unary_Function())
{
    auto it = min_of(first, last, f);
    return it != last
        ? f(*it)
        : typename std::result_of< Unary_Function(typename Forward_Iterator::value_type) >::type();
}

/**
 * Return minimum element in range; range version.
 * @param rg Range
 * @param f Use f to obtain key from value.
 */
template < class Forward_Range,
           class Unary_Function = detail::Identity >
auto
min_value_of(Forward_Range&& rg,
             Unary_Function&& f = Unary_Function())
    -> typename std::result_of< Unary_Function(typename decltype(rg.begin())::value_type) >::type
{
    return min_value_of(rg.begin(), rg.end(), f);
}


/**
 * Return iterator to maximum element in range; iterator version.
 * @param first Range begin
 * @param last Range end
 * @param f Use f to obtain key from value.
 */
template < class Forward_Iterator,
           class Unary_Function = detail::Identity >
Forward_Iterator
max_of(Forward_Iterator first, Forward_Iterator last,
       Unary_Function&& f = Unary_Function())
{
    if (first == last) return last;
    auto max_it = first++;
    for (auto it = first; it != last; ++it)
    {
        if (f(*max_it) < f(*it))
        {
            max_it = it;
        }
    }
    return max_it;
}

/**
 * Return iterator to maximum element in range; range version.
 * @param rg Range
 * @param f Use f to obtain key from value.
 */
template < class Forward_Range,
           class Unary_Function = detail::Identity >
auto
max_of(Forward_Range&& rg,
       Unary_Function&& f = Unary_Function())
    -> decltype(rg.end())
{
    static_assert(not std::is_rvalue_reference< Forward_Range >::value,
                  "rvalue reference to range not allowed: returned iterator might not exist");
    return max_of(rg.begin(), rg.end(), f);
}

/**
 * Return maximum element in range; iterator version.
 * @param first Range begin
 * @param last Range end
 * @param f Use f to obtain key from value.
 */
template < class Forward_Iterator,
           class Unary_Function = detail::Identity >
typename std::result_of< Unary_Function(typename Forward_Iterator::value_type) >::type
max_value_of(Forward_Iterator first, Forward_Iterator last,
             Unary_Function&& f = Unary_Function())
{
    auto it = max_of(first, last, f);
    return it != last
        ? f(*it)
        : typename std::result_of< Unary_Function(typename Forward_Iterator::value_type) >::type();
}

/**
 * Return maximum element in range; range version.
 * @param rg Range
 * @param f Use f to obtain key from value.
 */
template < class Forward_Range,
           class Unary_Function = detail::Identity >
auto
max_value_of(Forward_Range&& rg,
             Unary_Function&& f = Unary_Function())
    -> typename std::result_of< Unary_Function(typename decltype(rg.begin())::value_type) >::type
{
    return max_value_of(rg.begin(), rg.end(), f);
}


/**
 * Return iterator to minimum and maximum elements in range; iterator version.
 * @param first Range begin
 * @param last Range end
 * @param f Use f to obtain key from value.
 */
template < class Forward_Iterator,
           class Unary_Function = detail::Identity >
std::pair< Forward_Iterator, Forward_Iterator >
minmax_of(Forward_Iterator first, Forward_Iterator last,
          Unary_Function&& f = Unary_Function())
{
    if (first == last) return std::make_pair(last, last);
    auto min_it = first++;
    auto max_it = min_it;
    for (auto it = first; it != last; ++it)
    {
        if (f(*it) < f(*min_it))
        {
            min_it = it;
        }
        if (f(*max_it) < f(*it))
        {
            max_it = it;
        }
    }
    return std::make_pair(min_it, max_it);
}

/**
 * Return iterator to minimum and maximum elements in range; range version.
 * @param rg Range
 * @param f Use f to obtain key from value.
 */
template < class Forward_Range,
           class Unary_Function = detail::Identity >
auto
minmax_of(Forward_Range&& rg,
          Unary_Function&& f = Unary_Function())
    -> std::pair< decltype(rg.end()), decltype(rg.end()) >
{
    static_assert(not std::is_rvalue_reference< Forward_Range >::value,
                  "rvalue reference to range not allowed: returned iterator might not exist");
    return minmax_of(rg.begin(), rg.end(), f);
}

/**
 * Return minimum and maximum elements in range; iterator version.
 * @param first Range begin
 * @param last Range end
 * @param f Use f to obtain key from value.
 */
template < class Forward_Iterator,
           class Unary_Function = detail::Identity >
std::pair< typename std::result_of< Unary_Function(typename Forward_Iterator::value_type) >::type,
           typename std::result_of< Unary_Function(typename Forward_Iterator::value_type) >::type >
minmax_value_of(Forward_Iterator first, Forward_Iterator last,
                Unary_Function&& f = Unary_Function())
{
    auto p = minmax_of(first, last, f);
    return p.first != last
        ? std::make_pair(f(*p.first), f(*p.second))
        : std::make_pair(typename std::result_of< Unary_Function(typename Forward_Iterator::value_type) >::type(),
                         typename std::result_of< Unary_Function(typename Forward_Iterator::value_type) >::type());
}

/**
 * Return minimum and maximum elements in range; range version.
 * @param rg Range
 * @param f Use f to obtain key from value.
 */
template < class Forward_Range,
           class Unary_Function = detail::Identity >
auto
minmax_value_of(Forward_Range&& rg,
                Unary_Function&& f = Unary_Function())
    -> std::pair< typename std::result_of< Unary_Function(typename decltype(rg.begin())::value_type) >::type,
                  typename std::result_of< Unary_Function(typename decltype(rg.begin())::value_type) >::type >
{
    return minmax_value_of(rg.begin(), rg.end(), f);
}

/**
 * Compute mean and stdv of a sequence of samples; iterator version.
 * @tparam Float_Type Floating point type to use for calculations.
 * @param first Range begin
 * @param last Range end
 * @param f Use f to obtain key from value.
 */
template < typename Float_Type = double, class Input_Iterator, class Unary_Function = detail::Identity >
std::pair< Float_Type, Float_Type >
mean_stdv_of(Input_Iterator it_begin, Input_Iterator it_end,
             Unary_Function&& f = Unary_Function())
{
    Float_Type s = 0.0;
    Float_Type s2 = 0.0;
    long unsigned n = 0;
    for (Input_Iterator it = it_begin; it != it_end; ++it)
    {
        s += f(*it);
        s2 += f(*it) * f(*it);
        ++n;
    }
    Float_Type mean = n > 0? s / n : (Float_Type)0;
    Float_Type stdv = n > 1? std::sqrt((s2 - s * mean * 2.0 + mean * mean * (Float_Type)n)/(n - 1)) : (Float_Type)0;
    return std::make_pair(mean, stdv);
}

/**
 * Compute mean and stdv of a sequence of samples; range version.
 * @tparam Float_Type Floating point type to use for calculations.
 * @param rg Range
 * @param f Use f to obtain key from value.
 */
template < typename Float_Type = double, class Input_Range, class Unary_Function = detail::Identity >
std::pair< Float_Type, Float_Type >
mean_stdv_of(const Input_Range& rg, Unary_Function&& f = Unary_Function())
{
    return mean_stdv_of< Float_Type >(rg.begin(), rg.end(), f);
}

/**
 * Check if all values in range are equal; iterator version.
 * @param first Range begin
 * @param last Range end
 * @param f Use f to obtain key from value.
 */
template < class Forward_Iterator,
           class Unary_Function = detail::Identity >
bool
equal_of(Forward_Iterator first, Forward_Iterator last,
         Unary_Function&& f = Unary_Function())
{
    if (first == last) return true;
    for (auto it = std::next(first); it != last; ++it)
    {
        if (f(*it) != f(*first))
        {
            return false;
        }
    }
    return true;
}

/**
 * Check if all values in range are equal; range version.
 * @param rg Range
 * @param f Use f to obtain key from value.
 */
template < class Forward_Range,
           class Unary_Function = detail::Identity >
bool
equal_of(Forward_Range&& rg,
         Unary_Function&& f = Unary_Function())
{
    return equal_of(rg.begin(), rg.end(), f);
}

/**
 * Check functor returns true for all values in range; iterator version.
 * @param first Range begin
 * @param last Range end
 * @param f Use f to obtain key from value.
 */
using std::all_of;

/**
 * Check functor returns true for all values in range; range version.
 * @param rg Range
 * @param f Use f to obtain key from value.
 */
template < class Forward_Range,
           class Unary_Function = detail::Identity >
bool
all_of(Forward_Range&& rg,
       Unary_Function&& f = Unary_Function())
{
    return all_of(rg.begin(), rg.end(), f);
}

/**
 * Check functor returns true for at least one value in range; iterator version.
 * @param first Range begin
 * @param last Range end
 * @param f Use f to obtain key from value.
 */
using std::any_of;

/**
 * Check functor returns true for at least one value in range; range version.
 * @param rg Range
 * @param f Use f to obtain key from value.
 */
template < class Forward_Range,
           class Unary_Function = detail::Identity >
bool
any_of(Forward_Range&& rg,
       Unary_Function&& f = Unary_Function())
{
    return any_of(rg.begin(), rg.end(), f);
}

/**
 * Accumulate; iterator version.
 */
using std::accumulate;

/**
 * Accumulate; range version.
 */
template < class Forward_Range, typename T >
T accumulate(Forward_Range&& rg, T init)
{
    return accumulate(rg.begin(), rg.end(), init);
}

template < class Forward_Range, typename T, class Binary_Operation >
T accumulate(Forward_Range&& rg, T init, Binary_Operation&& op)
{
    return accumulate(rg.begin(), rg.end(), init, std::forward< Binary_Operation >(op));
}

#endif
