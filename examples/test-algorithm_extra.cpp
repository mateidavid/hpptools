#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <vector>
#include <list>
#include <functional>
#include <boost/preprocessor.hpp>

#include "algorithm_extra.hpp"

typedef std::vector< int > int_vector;
typedef std::vector< int >::iterator int_vector_iterator;
typedef std::list< int > int_list;
typedef std::list< int >::iterator int_list_iterator;

int int_identity_function(const int& v) { return v; }
struct int_identity_functor
{
    int operator () (const int& v) { return v; }
};

// containers
int_vector cont_lval = { 45, 12, 6, 19, 133, 42 };
const int_vector cont_const_lval = cont_lval;
int_vector& cont_lval_ref = cont_lval;
const int_vector& cont_const_lval_ref = cont_lval;

// element getter
auto getter_fcn_ptr = &int_identity_function;
auto& getter_fcn_ref = int_identity_function;
auto getter_lbd = [] (const int& v) { return v; };
auto getter_ftor = int_identity_functor();
std::function< int(const int&) > getter_stdfcn = int_identity_function;

#define CONTAINER(I) CONTAINER_##I
#define CONTAINER_0 cont_lval
#define CONTAINER_1 cont_const_lval
#define CONTAINER_2 cont_lval_ref
#define CONTAINER_3 cont_const_lval_ref
#define CONTAINER_4 int_vector(cont_lval)
#define CONTAINER_5 std::move(cont_lval)
#define CONTAINER_NUM_LVAL_ONLY 4
#define CONTAINER_NUM_RVAL 6

#define GETTER(I) GETTER_##I
#define GETTER_0 getter_fcn_ptr
#define GETTER_1 getter_fcn_ref
#define GETTER_2 getter_lbd
#define GETTER_3 getter_ftor
#define GETTER_4 getter_stdfcn
#define GETTER_NUM 5

// begin-end, return iterator
#define MAKE_GETTER_SECTION_BE_IT(_1, I, AUX) \
    SECTION("getter: " BOOST_PP_STRINGIZE(GETTER(I)))   \
    { \
    CHECK( FCN(AUX.begin(), AUX.end(), GETTER(I)) == RES(AUX) ); \
    }

#define MAKE_CONTAINER_SECTION_BE_IT(_1, N, _2) \
    SECTION("container: " BOOST_PP_STRINGIZE(CONTAINER(N)))  \
    { \
    BOOST_PP_REPEAT(GETTER_NUM, MAKE_GETTER_SECTION_BE_IT, CONTAINER(N)); \
    }

// range, return iterator
#define MAKE_GETTER_SECTION_RG_IT(_1, I, AUX) \
    SECTION("getter: " BOOST_PP_STRINGIZE(GETTER(I)))   \
    { \
    CHECK( FCN(AUX, GETTER(I)) == RES(AUX) ); \
    }

#define MAKE_CONTAINER_SECTION_RG_IT(_1, N, _2) \
    SECTION("container: " BOOST_PP_STRINGIZE(CONTAINER(N)))  \
    { \
    BOOST_PP_REPEAT(GETTER_NUM, MAKE_GETTER_SECTION_RG_IT, CONTAINER(N)); \
    }

// begin-end, return value
#define MAKE_GETTER_SECTION_BE_VAL(_1, I, AUX) \
    SECTION("getter: " BOOST_PP_STRINGIZE(GETTER(I)))   \
    { \
    CHECK( FCN(AUX.begin(), AUX.end(), GETTER(I)) == RES(AUX) ); \
    }

#define MAKE_CONTAINER_SECTION_BE_VAL(_1, N, _2) \
    SECTION("container: " BOOST_PP_STRINGIZE(CONTAINER(N)))  \
    { \
    BOOST_PP_REPEAT(GETTER_NUM, MAKE_GETTER_SECTION_BE_VAL, CONTAINER(N)); \
    }

// range, return value
#define MAKE_GETTER_SECTION_RG_VAL(_1, I, AUX) \
    SECTION("getter: " BOOST_PP_STRINGIZE(GETTER(I)))   \
    { \
    CHECK( FCN(AUX, GETTER(I)) == RES(AUX) ); \
    }

#define MAKE_CONTAINER_SECTION_RG_VAL(_1, N, _2) \
    SECTION("container: " BOOST_PP_STRINGIZE(CONTAINER(N)))  \
    { \
    BOOST_PP_REPEAT(GETTER_NUM, MAKE_GETTER_SECTION_RG_VAL, CONTAINER(N)); \
    }

TEST_CASE("min_of")
{
    SECTION("return: iterator")
    {
#define FCN min_of
#define RES(C) (C.begin() + 2)
        SECTION("form: begin-end")
        {
            BOOST_PP_REPEAT(CONTAINER_NUM_LVAL_ONLY, MAKE_CONTAINER_SECTION_BE_IT, _);
        }
        SECTION("form: range")
        {
            BOOST_PP_REPEAT(CONTAINER_NUM_LVAL_ONLY, MAKE_CONTAINER_SECTION_RG_IT, _);
        }
#undef RES
#undef FCN
    }
    SECTION("return: value")
    {
#define FCN min_value_of
#define RES(_) (6)
        SECTION("form: begin-end")
        {
            BOOST_PP_REPEAT(CONTAINER_NUM_LVAL_ONLY, MAKE_CONTAINER_SECTION_BE_VAL, _);
        }
        SECTION("form: range")
        {
            BOOST_PP_REPEAT(CONTAINER_NUM_RVAL, MAKE_CONTAINER_SECTION_RG_VAL, _);
        }
#undef RES
#undef FCN
    }
}

TEST_CASE("max_of")
{
    SECTION("return: iterator")
    {
#define FCN max_of
#define RES(C) (C.begin() + 4)
        SECTION("form: begin-end")
        {
            BOOST_PP_REPEAT(CONTAINER_NUM_LVAL_ONLY, MAKE_CONTAINER_SECTION_BE_IT, _);
        }
        SECTION("form: range")
        {
            BOOST_PP_REPEAT(CONTAINER_NUM_LVAL_ONLY, MAKE_CONTAINER_SECTION_RG_IT, _);
        }
#undef RES
#undef FCN
    }
    SECTION("return: value")
    {
#define FCN max_value_of
#define RES(_) (133)
        SECTION("form: begin-end")
        {
            BOOST_PP_REPEAT(CONTAINER_NUM_LVAL_ONLY, MAKE_CONTAINER_SECTION_BE_VAL, _);
        }
        SECTION("form: range")
        {
            BOOST_PP_REPEAT(CONTAINER_NUM_RVAL, MAKE_CONTAINER_SECTION_RG_VAL, _);
        }
#undef RES
#undef FCN
    }
}

TEST_CASE("minmax_of")
{
    SECTION("return: iterator")
    {
#define FCN minmax_of
#define RES(C) (std::make_pair(C.begin() + 2, C.begin() + 4))
        SECTION("form: begin-end")
        {
            BOOST_PP_REPEAT(CONTAINER_NUM_LVAL_ONLY, MAKE_CONTAINER_SECTION_BE_IT, _);
        }
        SECTION("form: range")
        {
            BOOST_PP_REPEAT(CONTAINER_NUM_LVAL_ONLY, MAKE_CONTAINER_SECTION_RG_IT, _);
        }
#undef RES
#undef FCN
    }
    SECTION("return: value")
    {
#define FCN minmax_value_of
#define RES(_) (std::make_pair(6, 133))
        SECTION("form: begin-end")
        {
            BOOST_PP_REPEAT(CONTAINER_NUM_LVAL_ONLY, MAKE_CONTAINER_SECTION_BE_VAL, _);
        }
        SECTION("form: range")
        {
            BOOST_PP_REPEAT(CONTAINER_NUM_RVAL, MAKE_CONTAINER_SECTION_RG_VAL, _);
        }
#undef RES
#undef FCN
    }
}

// Tweak definitions to check pairs of values
#undef MAKE_GETTER_SECTION_BE_VAL
#define MAKE_GETTER_SECTION_BE_VAL(_1, I, AUX) \
    SECTION("getter: " BOOST_PP_STRINGIZE(GETTER(I)))   \
    { \
    CHECK( FCN(AUX.begin(), AUX.end(), GETTER(I)).first == RES(AUX).first ); \
    CHECK( FCN(AUX.begin(), AUX.end(), GETTER(I)).second == RES(AUX).second ); \
    }
#undef MAKE_GETTER_SECTION_RG_VAL
#define MAKE_GETTER_SECTION_RG_VAL(_1, I, AUX) \
    SECTION("getter: " BOOST_PP_STRINGIZE(GETTER(I)))   \
    { \
    CHECK( FCN(AUX, GETTER(I)).first == RES(AUX).first ); \
    CHECK( FCN(AUX, GETTER(I)).second == RES(AUX).second ); \
    }

TEST_CASE("mean_stdv_of")
{
#define FCN mean_stdv_of
// results from:
//   http://www.miniwebtool.com/mean-calculator/
//   http://www.miniwebtool.com/sample-standard-deviation-calculator/
#define RES(_) (std::make_pair(Approx(42.83333), Approx(46.92725)))
    SECTION("form: begin-end")
    {
        BOOST_PP_REPEAT(CONTAINER_NUM_LVAL_ONLY, MAKE_CONTAINER_SECTION_BE_VAL, _);
    }
    SECTION("form: range")
    {
        BOOST_PP_REPEAT(CONTAINER_NUM_RVAL, MAKE_CONTAINER_SECTION_RG_VAL, _);
    }
#undef RES
#undef FCN
}

TEST_CASE("equal_of")
{
    SECTION("empty container")
    {
        int_vector v;
        CHECK( equal_of(v.begin(), v.end(), getter_fcn_ptr) == true );
        CHECK( equal_of(v.begin(), v.end()) == true );
        CHECK( equal_of(v, getter_fcn_ptr) == true );
        CHECK( equal_of(v) == true );
    }
    SECTION("singleton container")
    {
        int_vector v = { 42 };
        CHECK( equal_of(v.begin(), v.end(), getter_fcn_ptr) == true );
        CHECK( equal_of(v.begin(), v.end()) == true );
        CHECK( equal_of(v, getter_fcn_ptr) == true );
        CHECK( equal_of(v) == true );
    }
    SECTION("container with 2 equal elements")
    {
        int_vector v = { 23, 23 };
        CHECK( equal_of(v.begin(), v.end(), getter_fcn_ptr) == true );
        CHECK( equal_of(v.begin(), v.end()) == true );
        CHECK( equal_of(v, getter_fcn_ptr) == true );
        CHECK( equal_of(v) == true );
    }
    SECTION("container with 2 different elements")
    {
        int_vector v = { 15, 16 };
        CHECK( equal_of(v.begin(), v.end(), getter_fcn_ptr) == false );
        CHECK( equal_of(v.begin(), v.end()) == false );
        CHECK( equal_of(v, getter_fcn_ptr) == false );
        CHECK( equal_of(v) == false );
    }
}

TEST_CASE("for_each_advance")
{
    int_list l = { 1, 2, 3, 4, 5 };
    SECTION("non-mutating function")
    {
        int s = 0;
        auto fn = [&] (const int& v) { s += v; };
        for_each_advance(int_vector(), fn);
        CHECK( s == 0 );
        for_each_advance(int_vector({ 4 }), fn);
        CHECK( s == 4 );
        s = 0;
        for_each_advance(l.begin(), l.end(), fn);
        CHECK( s == 15 );
        s = 0;
        for_each_advance(l, fn);
        CHECK( s == 15 );
    }
    SECTION("mutating function")
    {
        int x = 3;
        // if v == x, remove element in l
        auto lbd = [&] (const int& v) {
            if (v != x) return;
            auto it = l.begin();
            while (*it != x and it != l.end()) ++it;
            if (it == l.end()) return;
            l.erase(it);
        };
        for_each_advance(l.begin(), l.end(), lbd);
        CHECK( l == int_list({ 1, 2, 4, 5 }) );
        x = 5;
        for_each_advance(l, lbd);
        CHECK( l == int_list({ 1, 2, 4 }) );
    }
}

TEST_CASE("for_each_it")
{
    int_list l = { 1, 2, 3, 4, 5 };
    SECTION("non-mutating function")
    {
        int s = 0;
        auto fn = [&] (int_list_iterator it) { s += *it; };
        for_each_it(int_list(), fn);
        CHECK( s == 0 );
        for_each_it(int_list({ 4 }), fn);
        CHECK( s == 4 );
        s = 0;
        for_each_it(l.begin(), l.end(), fn);
        CHECK( s == 15 );
        s = 0;
        for_each_it(l, fn);
        CHECK( s == 15 );
    }
    SECTION("mutating function")
    {
        int x = 3;
        // if v == x, remove preceding element in l
        auto lbd = [&] (int_list_iterator it) {
            if (*it != x) return;
            if (it == l.begin()) return;
            --it;
            l.erase(it);
        };
        for_each_it(l.begin(), l.end(), lbd);
        CHECK( l == int_list({ 1, 3, 4, 5 }) );
        x = 5;
        for_each_it(l, lbd);
        CHECK( l == int_list({ 1, 3, 5 }) );
    }
}

TEST_CASE("for_each_it_advance")
{
    int_list l = { 1, 2, 3, 4, 5 };
    SECTION("non-mutating function")
    {
        int s = 0;
        auto fn = [&] (int_list_iterator it) { s += *it; };
        for_each_it_advance(int_list(), fn);
        CHECK( s == 0 );
        for_each_it_advance(int_list({ 4 }), fn);
        CHECK( s == 4 );
        s = 0;
        for_each_it_advance(l.begin(), l.end(), fn);
        CHECK( s == 15 );
        s = 0;
        for_each_it_advance(l, fn);
        CHECK( s == 15 );
    }
    SECTION("mutating function")
    {
        int x = 3;
        // if v == x, remove element in l
        auto lbd = [&] (int_list_iterator it) {
            if (*it != x) return;
            l.erase(it);
        };
        for_each_it_advance(l.begin(), l.end(), lbd);
        CHECK( l == int_list({ 1, 2, 4, 5 }) );
        x = 5;
        for_each_it_advance(l, lbd);
        CHECK( l == int_list({ 1, 2, 4 }) );
    }
}
