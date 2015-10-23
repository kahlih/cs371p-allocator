// ------------------------------------
// projects/allocator/TestAllocator1.c++
// Copyright (C) 2015
// Glenn P. Downing
// ------------------------------------

// --------
// includes
// --------

#include <algorithm> // count
#include <memory>    // allocator

#include "gtest/gtest.h"

#include "Allocator.h"

// Custom class to test types with
class Elephant{
    public:
        string name;
        Elephant(string s){
            name = s;
        }
        bool operator==(const Elephant& rhs){
            return this->name == rhs.name;
        }
};


// --------------
// TestAllocator1
// --------------

template <typename A>
struct TestAllocator1 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A             allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::size_type  size_type;
    typedef typename A::pointer    pointer;};

typedef testing::Types<
            std::allocator<int>,
            std::allocator<double>,
            Allocator<int,    100>,
            Allocator<double, 100> >
        my_types_1;

TYPED_TEST_CASE(TestAllocator1, my_types_1);

TYPED_TEST(TestAllocator1, test_1) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 1;
    const value_type     v = 2;
    const pointer        p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator1, test_10) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type  x;
    const size_type       s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}

// --------------
// TestAllocator2
// --------------

TEST(TestAllocator2, const_index) {
    const Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);}

TEST(TestAllocator2, index) {
    Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);}

// --------------
// TestAllocator3
// --------------

template <typename A>
struct TestAllocator3 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A             allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::size_type  size_type;
    typedef typename A::pointer    pointer;};

typedef testing::Types<
            Allocator<int,    100>,
            Allocator<double, 100> >
        my_types_2;

TYPED_TEST_CASE(TestAllocator3, my_types_2);

TYPED_TEST(TestAllocator3, test_1) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 1;
    const value_type     v = 2;
    const pointer        p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}


TYPED_TEST(TestAllocator3, test_10) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 10;
    const value_type     v = 2;
    const pointer        b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}


// --------------
// TestCustomValid
// --------------

/* Verify valid() can catch false/incorrect values and allocations*/
TEST(TestCustomValid, valid_1){
    Allocator<char, 100> x;

    // Hard code values in to test functionality
    x[0] = -4;
    x[8] = -4;
    x[12] = 600;
    x[96] = 600;
    ASSERT_FALSE(x.valid());
}

/* Verify validity after changes */
TEST(TestCustomValid, valid_2){
    Allocator<double, 100> x;

    // Hard code values in to test functionality
    x[0] = -16;
    x[20] = -16;
    x[24] = 8;
    x[36] = 8;
    x[40] = -8;
    x[52] = -8;
    x[56] = 36;
    x[96] = 36;
    ASSERT_TRUE(x.valid());

    x[0] = 32;
    x[36] = 32;
    x[40] = -8;
    x[52] = -8;
    x[56] = 36;
    x[96] = 36;

    ASSERT_TRUE(x.valid());

}

/* Catch two adjacent free chunks and return false */
TEST(TestCustomValid, valid_3){
    Allocator<int, 100> x;

    // Hard code values in to test functionality
    x[0] = 4;
    x[8] = 4;
    x[12] = 80;
    x[96] = 80;
    ASSERT_FALSE(x.valid());
}

/* Catch invalid array where sentinals do not match */
TEST(TestCustomValid, valid_4){
    Allocator<double, 100> x;

    // Hard code values in to test functionality
    x[0] = 92;
    x[96] = 56;
    ASSERT_FALSE(x.valid());
}


// --------------
// TestCustomAllocate
// --------------

/**
*   Test the default constructor if not initialized properly
*/

TEST(TestCustomAllocate, exceptionAlloc) {
    try{
        const Allocator<int, 8> x;
        ASSERT_EQ(1,0);
    }
    catch(std::bad_alloc& exception) {
        SUCCEED();
    }
}

/**
*   Test Multiple Allocations
*/

TEST(TestCustomAllocate, multipleAlloc) {
    Allocator<double, 100> x;
    double* p   = x.allocate(2);
    double* p2  = x.allocate(3);


    ASSERT_EQ(x[0], -16);
    ASSERT_EQ(x[24], -24);
    ASSERT_EQ(x[56], 36);

    double* p_end = p+2;
    double* p2_end = p2+3;

    while (p!=p_end){
        x.construct(p,5);
        ++p;
    }
    while (p2!=p2_end){
        x.construct(p2,5);
        ++p2;
    }

    ASSERT_EQ(x[4],x[28]);
}

/**
*   Test Proper Allocation (skips to appropriate place)
*/

TEST(TestCustomAllocate, properAlloc){
    Allocator<double, 100> x;

    // Hard code values in to test allocation functionality
    x[0] = -16;
    x[20] = -16;
    x[24] = 8;
    x[36] = 8;
    x[40] = -8;
    x[52] = -8;
    x[56] = 36;
    x[96] = 36;

    x.allocate(2);
    ASSERT_EQ(x[56],-16);
    ASSERT_EQ(x[80],12);
}

TEST(TestCustomAllocate, oddTypeAlloc){
    Allocator<Elephant, 100> e;
    Elephant v("Dumbo");
    Elephant* p = e.allocate(1);
    e.construct(p,v);
    ASSERT_EQ(e[0], -(sizeof(Elephant)));
}


// TEST(TestCustomDeallocate, simpleDealloc){
//     Allocator<int,16> x;
//     int* p = x.allocate(2);
//     ASSERT_EQ(x[0],-8);
//     x.deallocate(p,2);
//     ASSERT_EQ(x[0], 12);
// }