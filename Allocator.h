// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2015
// Glenn P. Downing
// ------------------------------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert>   // assert
#include <cstddef>   // ptrdiff_t, size_t
#include <new>       // bad_alloc, new
#include <stdexcept> // invalid_argument
#include <stdlib.h>     /* abs */

using namespace std;

// ---------
// Allocator
// ---------

template <typename T, std::size_t N>
class Allocator {
    public:
        // --------
        // typedefs
        // --------

        typedef T                 value_type;

        typedef std::size_t       size_type;
        typedef std::ptrdiff_t    difference_type;

        typedef       value_type*       pointer;
        typedef const value_type* const_pointer;

        typedef       value_type&       reference;
        typedef const value_type& const_reference;

    public:
        // -----------
        // operator ==
        // -----------

        friend bool operator == (const Allocator&, const Allocator&) {
            return true;}                                              // this is correct

        // -----------
        // operator !=
        // -----------

        friend bool operator != (const Allocator& lhs, const Allocator& rhs) {
            return !(lhs == rhs);}

    private:
        // ----
        // data
        // ----

        char a[N];

        // -----
        // valid
        // -----

        /**
         * O(1) in space
         * O(n) in time
         * <your documentation>
         */
        bool valid () const {
    	    int sentinel = 0;
    	    while (sentinel < N){
        	    int begin = abs(a[sentinel]);
        		/* If values go Out of Bounds*/
        		if (sentinel + begin + 8 > N){
                    cout << "Failed in out of bounds case" << endl;
        			return false; }
        		
        		int end = abs(a[sentinel+begin+4]);
        		if (begin != end) { 
                    cout << "Failed in checking if sentinals are equal" << endl;
                    cout << "begin: " << begin << "  and end: " << end << endl;
        			return false; }
        		
        		sentinel += (begin + 8);
    	    }
            return true;}

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         * https://code.google.com/p/googletest/wiki/AdvancedGuide#Private_Class_Members
         */
        FRIEND_TEST(TestAllocator2, index);
        int& operator [] (int i) {
            return *reinterpret_cast<int*>(&a[i]);}

    public:
        // ------------
        // constructors
        // ------------

        /**
         * O(1) in space
         * O(1) in time
         * throw a bad_alloc exception, if N is less than sizeof(T) + (2 * sizeof(int))
         * set up array will sentinals
	 */
        Allocator () {
            if (N < sizeof(T) + 8){throw std::bad_alloc();}
            int v = (sizeof(a))-4-4; //two sentinals
            (*this)[0] = v;
            (*this)[N-4] = v;
            assert(valid());}

        // Default copy, destructor, and copy assignment
        // Allocator  (const Allocator&);
        // ~Allocator ();
        // Allocator& operator = (const Allocator&);

        // --------
        // allocate test
        // --------

        /**
         * O(1) in space
         * O(n) in time
         * after allocation there must be enough space left for a valid block
         * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
         * choose the first block that fits
         * throw a bad_alloc exception, if n is invalid
         */
        pointer allocate (size_type n) {
    	    int sentinel = 0;
            int chunk = n*(sizeof(T));
    	    int begin = a[sentinel];
            /* If able to allocate space */
    	    if ((begin > 0)){
                if (begin >= (chunk+8+sizeof(T))){ //under the size of freespace with room to allocate one more

                    int newBoundary = begin-chunk-8;

            		a[sentinel] = -chunk;
            		a[sentinel+chunk+4] = -chunk;

            		a[sentinel+chunk+8] = newBoundary;
            		a[sentinel+begin+4] = newBoundary;
                    return reinterpret_cast<pointer>(a+sentinel+4);}
                else if(begin < (chunk+8+sizeof(T))){ // able to allocate, small amount left over
                    a[sentinel+begin+4] = -begin;
                    return reinterpret_cast<pointer>(a+sentinel+4);
                }
                // else if(begin == (chunk+8)){ //can
                //     a[sentinel] = -begin;
                //     a[sentinel+begin+4] = -begin;
                //     return reinterpret_cast<pointer>(a+sentinel+4);   
                // }
    	    }
            assert(valid());
            return nullptr;}             // replace!

        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         */
        void construct (pointer p, const_reference v) {
            new (p) T(v);                               // this is correct and exempt
            assert(valid());}                           // from the prohibition of new

        // ----------
        // deallocate
        // ----------

        /**
         * O(1) in space
         * O(1) in time
         * after deallocation adjacent free blocks must be coalesced
         * throw an invalid_argument exception, if p is invalid
         * <your documentation>
         */
        void deallocate (pointer p, size_type) {
            // <your code>
            assert(valid());}

        // -------
        // destroy
        // -------

        /**
         * O(1) in space
         * O(1) in time
         */
        void destroy (pointer p) {
            p->~T();               // this is correct
            assert(valid());}

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        const int& operator [] (int i) const {
            return *reinterpret_cast<const int*>(&a[i]);}};

#endif // Allocator_h
