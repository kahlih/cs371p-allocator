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
#include <stdlib.h>  /* abs */
#include <cmath>     /* signbit */

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
                    // cout << "Failed in out of bounds case" << endl;
        			return false; }
        		
                //absolute value check
        		int end = abs(a[sentinel+begin+4]);
        		if (begin != end) { 
                    // cout << "Failed in checking if sentinels are equal" << endl;
                    // cout << "begin: " << begin << "  and end: " << end << endl;
        			return false; }

                //sign check
                // if(signbit(a[sentinel]) != signbit(a[sentinel+begin+4])) {
                //     cout << "Failed in checking if sentinel signs are equal" <<endl;
                //     cout << "begin: " << begin << "  and end: " << end << endl;
                //     return false;}    

                //check for adjacent free blocks
                //take current sentinel and check behind
                if(sentinel-4 > 0 && sentinel > 0) {
                    if((a[sentinel] > 0) && (a[sentinel-4] > 0)) { //check sentinel behind
                        // cout << "Failed: Two free blocks side by side" << endl;
                        return false;
                    }
                }


        		


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
        FRIEND_TEST(TestCustomAllocate, multipleAlloc);
        FRIEND_TEST(TestCustomAllocate, properAlloc);
        FRIEND_TEST(TestCustomValid, valid_1);
        FRIEND_TEST(TestCustomAllocate, oddTypeAlloc);
        FRIEND_TEST(TestCustomValid, valid_2);
        FRIEND_TEST(TestCustomValid, valid_3);
        FRIEND_TEST(TestCustomValid, valid_4);

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
            while(sentinel < N){

                int begin = a[sentinel];

                /* If able to allocate space */
                if ((begin > 0)){

                    if(begin == (chunk+8)){ //can
                        a[sentinel] = -begin;
                        a[sentinel+begin+4] = -begin;
                        return reinterpret_cast<pointer>(a+sentinel+4);   
                    }
                    else if (begin >= (chunk+8+sizeof(T))){ //under the size of freespace with room to allocate one more

                        int newBoundary = begin-chunk-8;
                        int sentinelLHS = sentinel;
                        int sentinelRHS = sentinel+chunk+4;

                        // cout << "sentinelLHS position: " << sentinelLHS << endl;
                        // cout << "sentinelRHS position: " << sentinelRHS << endl;


                		(*this)[sentinelLHS] = -chunk;
                		(*this)[sentinelRHS] = -chunk;


                        // cout << "sentinelLHS value: " << (*this)[sentinelLHS]  << endl;
                        // cout << "sentinelRHS value: " << (*this)[sentinelRHS] << endl;



                		(*this)[sentinel+chunk+8] = newBoundary;
                		(*this)[sentinel+begin+4] = newBoundary;


                        return reinterpret_cast<pointer>(a+sentinel+4);
                    }
                    else if((begin > (chunk+8)) && begin < (chunk+8+sizeof(T))){ // able to allocate, small amount left over
                        a[sentinel+begin+4] = -begin;
                        return reinterpret_cast<pointer>(a+sentinel+4);
                    }
                    /* If none of the above worked, space is too small to allocate. Must move on to next chunk of space */
                    else{
                        int jump = abs(begin) + 8;
                        sentinel += jump;                        
                    }

        	    }
                /* Negative value => Allocated Space; jump to next free space*/
                else{
                    int jump = abs(begin) + 8;
                    sentinel += (jump);
                }
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
        void deallocate (pointer p, size_type n) {
            // <your code>

            /*
                number of blocks to free is n
                point to right after sentinel is p
            */

                // //debug to show Sentinels prior to being freed
                // cout<<"Sentinel1: "<< *(p-1) << endl << endl;
                // cout<<"Sentinel2: "<< *(p+(1*n)) << endl << endl;
                char* convertedPoint = reinterpret_cast<char*>(p);

            //go to LHS sentinel and make it positive

                char* lhsPointer = convertedPoint-1*sizeof(int);
                int lhsSentinel = *(lhsPointer);
                    lhsSentinel = abs(lhsSentinel);
            //go to RHS sentinel and make it positive
                char* rhsPointer = convertedPoint + lhsSentinel;
                int rhsSentinel = *(rhsPointer);
                    rhsSentinel = abs(rhsSentinel);

                //debug check to show that Sentinels are freed
                // cout<<"flippedSentinel1: "<< lhsSentinel << endl;
                // cout<<"flippedSentinel2: "<< rhsSentinel << endl << endl;


                /*
                    conditional will check if the adjacent sentinel to lhsSentinel is free
                */
                char* lhsAdjSent = lhsPointer - sizeof(int);

                if(*lhsAdjSent > 0) {
                    int chunkLeft = *lhsAdjSent+lhsSentinel;
                    char* leftJumpPointer = lhsAdjSent -(*lhsAdjSent - sizeof(int));
                    *leftJumpPointer = chunkLeft + 8; //to account for the inner sentinels

                    *lhsPointer = 0;  //zeroed inner sentinels
                    *lhsAdjSent = 0;  //zeroed inner Sentinels

                        *rhsPointer = *leftJumpPointer; //converted rightmost sentinel to new value
                         lhsPointer = leftJumpPointer; //new lhsSentinel
                }





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

        // int& operator [] (int i) {
        //     return *reinterpret_cast<int*>(&a[i]);}

        const int& operator [] (int i) const {
            return *reinterpret_cast<const int*>(&a[i]);}};


#endif // Allocator_h
