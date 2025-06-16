#pragma once
#include <cmath>
#include <cassert>

template <typename A, typename B> struct TAreTypesEqual;

template <typename, typename> struct TAreTypesEqual {
        enum { Value = false };
};

template <typename A> struct TAreTypesEqual<A, A> {
        enum { Value = true };
};

#define ARE_TYPES_EQUAL( A, B ) TAreTypesEqual<A, B>::Value


inline void CheckHandler( const char *expr, const char *file, int line,
                          const std::string &msg = "" ) {
        std::cerr << "CHECK FAILED: (" << expr << ")" << " at " << file << ":"
                  << line;
        if ( !msg.empty() )
                std::cerr << " - " << msg;
        std::cerr << std::endl;

        std::abort();
}

inline void CheckHandler( const char *expr, const char *file, int line,
                          const std::wstring &msg = L"" ) {
        std::wcerr << L"CHECK FAILED: (" << expr << L")" << L" at " << file << L":"
                  << line;
        if ( !msg.empty() )
                std::wcerr << " - " << msg;
        std::cerr << std::endl;

        std::abort();
}

inline void CheckHandlerWithNoStr( const char *expr, const char *file, int line ) {
        std::wcerr << L"CHECK FAILED: (" << expr << L")" << L" at " << file
                   << L":" << line;
        std::cerr << std::endl;

        std::abort();
}

inline void CheckSlowHandler( const char *expr, const char *file, int line) {
        std::wcerr << L"CHECK FAILED: (" << expr << L")" << L" at " << file
                   << L":" << line;
        std::cerr << std::endl;

        std::abort();
}

#define DO_CHECK_SLOW 0

#define check( expr )                                                          \
        ( ( expr ) ? static_cast<void>( 0 )                                    \
                   : CheckHandlerWithNoStr( #expr, __FILE__, __LINE__ ) )

#define checkf( expr, message, ... )                                           \
        ( ( expr ) ? static_cast<void>( 0 )                                    \
                   : CheckHandler( #expr, __FILE__, __LINE__,                   \
                                  std::format( message, __VA_ARGS__ ) ) )

#if DO_CHECK_SLOW
#define checkSlow( expr )                                                      \
        ( ( expr ) ? static_cast<void>( 0 )                                    \
                   : CheckSlowHandler( #expr, __FILE__, __LINE__ ) )
#else
#define checkSlow( expr ) ( (void)0 )
#endif

inline bool EnsureFailed( const char *expr, const char *file, int line,
                          const std::string &msg, bool &triggered ) {
        if ( !triggered ) {
                triggered = true;

                std::cerr << "ENSURE FAILED: (" << expr << ")"
                          << " at " << file << ":" << line << " - " << msg
                          << std::endl;
        }
        return false;
}

#define ensureMsgf( expr, message )                                            \
        ( []() -> bool {                                                       \
                static bool triggered = false;                                 \
                return ( expr ) ? true                                         \
                                : EnsureFailed( #expr, __FILE__, __LINE__,     \
                                                ( message ), triggered );      \
        } )()

#define RESTRICT __restrict
#define UE_ASSUME( x ) __assume(x)

enum { INDEX_NONE = -1 };
enum { UNICODE_BOM = 0xfeff };

template <typename T> struct TRemoveReference {
        typedef T Type;
};
template <typename T> struct TRemoveReference<T &> {
        typedef T Type;
};
template <typename T> struct TRemoveReference<T &&> {
        typedef T Type;
};

template <typename T>
FORCEINLINE T &&Forward( typename TRemoveReference<T>::Type &Obj ) {
        return static_cast<T &&>( Obj );
}

template <typename T>
FORCEINLINE T &&Forward( typename TRemoveReference<T>::Type &&Obj ) {
        static_assert( !std::is_lvalue_reference<T>::value,
                       "Do not forward an rvalue as an lvalue." );
        return static_cast<T &&>( Obj );
}

/**
 * MoveTemp will cast a reference to an rvalue reference.
 * This is UE's equivalent of std::move.  It doesn't static assert like
 * MoveTemp, because it is useful in templates or macros where it's not obvious
 * what the argument is, but you want to take advantage of move semantics where
 * you can but not stop compilation.
 */
template <typename T>
FORCEINLINE typename TRemoveReference<T>::Type &&MoveTempIfPossible( T &&Obj ) {
        typedef typename TRemoveReference<T>::Type CastType;
        return (CastType &&)Obj;
}

template <typename FuncType, typename... ArgTypes>
FORCEINLINE auto Invoke( FuncType &&Func, ArgTypes &&...Args )
    -> decltype( Forward<FuncType>( Func )( Forward<ArgTypes>( Args )... ) ) {
        return Forward<FuncType>( Func )( Forward<ArgTypes>( Args )... );
}

FORCEINLINE size_t DefaultQuantizeSize( size_t Count,
                                              uint32_t Alignment ) {
        return Count;
}

FORCEINLINE int32_t DefaultCalculateSlackGrow( int32_t NumElements,
                                               int32_t NumAllocatedElements,
                                               size_t BytesPerElement,
                                               bool bAllowQuantize,
                                               uint32_t Alignment = 8 ) {
        const size_t FirstGrow = 4;
        const size_t ConstantGrow = 16;
        int32_t Retval;

        size_t Grow = FirstGrow;

        if ( NumAllocatedElements || size_t( NumElements ) > Grow ) {
                Grow = size_t( NumElements ) + 3 * size_t( NumElements ) / 8 +
                       ConstantGrow;
        }
        if ( bAllowQuantize ) {
                Retval = int32_t(
                    DefaultQuantizeSize( Grow * BytesPerElement, Alignment ) /
                    BytesPerElement );
        } else {
                Retval = int32_t( Grow );
        }
        if ( NumElements > Retval ) {
                Retval = INT32_MAX;
        }

        printf( "RetVal: %d\n", Retval );

        return Retval;
}

namespace SDK 
{
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint64_t uint64;
typedef uint8_t uint8;
typedef int64_t int64;

template <typename ContainerType, typename ElementType, typename SizeType>
class TIndexedContainerIterator {
      public:
        TIndexedContainerIterator( ContainerType &InContainer,
                                   SizeType StartIndex = 0 )
            : Container( InContainer ), Index( StartIndex ) {}

        /** Advances iterator to the next element in the container. */
        TIndexedContainerIterator &operator++() {
                ++Index;
                return *this;
        }
        TIndexedContainerIterator operator++( int ) {
                TIndexedContainerIterator Tmp( *this );
                ++Index;
                return Tmp;
        }

        /** Moves iterator to the previous element in the container. */
        TIndexedContainerIterator &operator--() {
                --Index;
                return *this;
        }
        TIndexedContainerIterator operator--( int ) {
                TIndexedContainerIterator Tmp( *this );
                --Index;
                return Tmp;
        }

        /** iterator arithmetic support */
        TIndexedContainerIterator &operator+=( SizeType Offset ) {
                Index += Offset;
                return *this;
        }

        TIndexedContainerIterator operator+( SizeType Offset ) const {
                TIndexedContainerIterator Tmp( *this );
                return Tmp += Offset;
        }

        TIndexedContainerIterator &operator-=( SizeType Offset ) {
                return *this += -Offset;
        }

        TIndexedContainerIterator operator-( SizeType Offset ) const {
                TIndexedContainerIterator Tmp( *this );
                return Tmp -= Offset;
        }

        ElementType &operator*() const { return Container[Index]; }

        ElementType *operator->() const { return &Container[Index]; }

        /** conversion to "bool" returning true if the iterator has not reached
         * the last element. */
        FORCEINLINE explicit operator bool() const {
                return Container.IsValidIndex( Index );
        }

        /** Returns an index to the current element. */
        SizeType GetIndex() const { return Index; }

        /** Resets the iterator to the first element. */
        void Reset() { Index = 0; }

        /** Sets the iterator to one past the last element. */
        void SetToEnd() { Index = Container.Num(); }

        /** Removes current element in array. This invalidates the current
         * iterator value and it must be incremented */
        void RemoveCurrent() {
                Container.RemoveAt( Index );
                Index--;
        }

        FORCEINLINE friend bool
        operator==( const TIndexedContainerIterator &Lhs,
                    const TIndexedContainerIterator &Rhs ) {
                return &Lhs.Container == &Rhs.Container &&
                       Lhs.Index == Rhs.Index;
        }
        FORCEINLINE friend bool
        operator!=( const TIndexedContainerIterator &Lhs,
                    const TIndexedContainerIterator &Rhs ) {
                return &Lhs.Container != &Rhs.Container ||
                       Lhs.Index != Rhs.Index;
        }

      private:
        ContainerType &Container;
        SizeType Index;
};

/**
 * Pointer-like iterator type for ranged-for loops which checks that the
 * container hasn't been resized during iteration.
 */
template <typename ElementType, typename SizeType>
struct TCheckedPointerIterator {
        // This iterator type only supports the minimal functionality needed to
        // support C++ ranged-for syntax.  For example, it does not provide
        // post-increment ++ nor ==.
        //
        // We do add an operator-- to help FString implementation

        explicit TCheckedPointerIterator( const SizeType &InNum,
                                          ElementType *InPtr )
            : Ptr( InPtr ), CurrentNum( InNum ), InitialNum( InNum ) {}

        FORCEINLINE ElementType &operator*() const { return *Ptr; }

        FORCEINLINE TCheckedPointerIterator &operator++() {
                ++Ptr;
                return *this;
        }

        FORCEINLINE TCheckedPointerIterator &operator--() {
                --Ptr;
                return *this;
        }

      private:
        ElementType *Ptr;
        const SizeType &CurrentNum;
        SizeType InitialNum;

        FORCEINLINE friend bool
        operator!=( const TCheckedPointerIterator &Lhs,
                    const TCheckedPointerIterator &Rhs ) {
                // We only need to do the check in this operator, because no
                // other operator will be called until after this one returns.
                //
                // Also, we should only need to check one side of this
                // comparison - if the other iterator isn't even from the same
                // array then the compiler has generated bad code.
                return Lhs.Ptr != Rhs.Ptr;
        }
};

template <typename InElementType> class TArray {
        template <typename OtherInElementType> friend class TArray;
        friend class FString;

      public:
        typedef InElementType ElementType;
        typedef int32_t SizeType;

        /**
         * Constructor, initializes element number counters.
         */
        FORCEINLINE TArray() : Data(nullptr), ArrayNum( 0 ), ArrayMax( 0 ) {}


      public:

        /**
         * Helper function for returning a typed pointer to the first array
         * entry.
         *
         * @returns Pointer to first array entry or nullptr if ArrayMax == 0.
         */
        FORCEINLINE ElementType *GetData() { return (ElementType *)Data; }

        FORCEINLINE const ElementType *GetData() const {
                return (const ElementType *)Data;
        }

        /**
         * Helper function returning the size of the inner type.
         *
         * @returns Size in bytes of array type.
         */
        FORCEINLINE uint32 GetTypeSize() const { return sizeof( ElementType ); }

        /**
         * Returns the amount of slack in this array in elements.
         *
         * @see Num, Shrink
         */
        FORCEINLINE int32 GetSlack() const { return ArrayMax - ArrayNum; }

        /**
         * Checks array invariants: if array size is greater than zero and less
         * than maximum.
         */
        FORCEINLINE void CheckInvariants() const {
                checkSlow( ( ArrayNum >= 0 ) &
                           ( ArrayMax >= ArrayNum ) ); // & for one branch
        }

        /**
         * Checks if index is in array range.
         *
         * @param Index Index to check.
         */
        FORCEINLINE void RangeCheck( int32_t Index ) const {
                CheckInvariants();
        }

        /**
         * Tests if index is valid, i.e. greater than or equal to zero, and less
         * than the number of elements in the array.
         *
         * @param Index Index to test.
         * @returns True if index is valid. False otherwise.
         */
        FORCEINLINE bool IsValidIndex( int32 Index ) const {
                return Index >= 0 && Index < ArrayNum;
        }

        /**
         * Returns true if the array is empty and contains no elements.
         *
         * @returns True if the array is empty.
         * @see Num
         */
        bool IsEmpty() const { return ArrayNum == 0; }

        /**
         * Returns number of elements in array.
         *
         * @returns Number of elements in array.
         * @see GetSlack
         */
        FORCEINLINE int32 Num() const { return ArrayNum; }

        /**
         * Returns maximum number of elements in array.
         *
         * @returns Maximum number of elements in array.
         * @see GetSlack
         */
        FORCEINLINE int32 Max() const { return ArrayMax; }

        /**
         * Array bracket operator. Returns reference to element at give index.
         *
         * @returns Reference to indexed element.
         */
        FORCEINLINE ElementType &operator[]( int32 Index ) {
                RangeCheck( Index );
                return GetData()[Index];
        }

        /**
         * Array bracket operator. Returns reference to element at give index.
         *
         * Const version of the above.
         *
         * @returns Reference to indexed element.
         */
        FORCEINLINE const ElementType &operator[]( int32 Index ) const {
                RangeCheck( Index );
                return GetData()[Index];
        }

        /**
         * Finds element within the array.
         *
         * @param Item Item to look for.
         * @param Index Will contain the found index.
         * @returns True if found. False otherwise.
         * @see FindLast, FindLastByPredicate
         */
        FORCEINLINE bool Find( const ElementType &Item,
                               SizeType &Index ) const {
                Index = this->Find( Item );
                return Index != INDEX_NONE;
        }

        /**
         * Finds element within the array.
         *
         * @param Item Item to look for.
         * @returns Index of the found element. INDEX_NONE otherwise.
         * @see FindLast, FindLastByPredicate
         */
        int32 Find( const ElementType &Item ) const {
                const ElementType *__restrict Start = GetData();
                for ( const ElementType *__restrict
                          Data = Start,
                          *__restrict DataEnd = Data + ArrayNum;
                      Data != DataEnd; ++Data ) {
                        if ( *Data == Item ) {
                                return static_cast<int32>( Data - Start );
                        }
                }
                return INDEX_NONE;
        }

        /**
         * Finds element within the array starting from the end.
         *
         * @param Item Item to look for.
         * @param Index Output parameter. Found index.
         * @returns True if found. False otherwise.
         * @see Find, FindLastByPredicate
         */
        FORCEINLINE bool FindLast( const ElementType &Item,
                                   SizeType &Index ) const {
                Index = this->FindLast( Item );
                return Index != INDEX_NONE;
        }

        /**
         * Finds element within the array starting from the end.
         *
         * @param Item Item to look for.
         * @returns Index of the found element. INDEX_NONE otherwise.
         */
        SizeType FindLast( const ElementType &Item ) const {
                for ( const ElementType *RESTRICT
                          Start = GetData(),
                          *RESTRICT Data = Start + ArrayNum;
                      Data != Start; ) {
                        --Data;
                        if ( *Data == Item ) {
                                return static_cast<SizeType>( Data - Start );
                        }
                }
                return INDEX_NONE;
        }

        /**
         * Searches an initial subrange of the array for the last occurrence of
         * an element which matches the specified predicate.
         *
         * @param Pred Predicate taking array element and returns true if
         * element matches search criteria, false otherwise.
         * @param Count The number of elements from the front of the array
         * through which to search.
         * @returns Index of the found element. INDEX_NONE otherwise.
         */
        template <typename Predicate>
        SizeType FindLastByPredicate( Predicate Pred, SizeType Count ) const {
                check( Count >= 0 && Count <= this->Num() );
                for ( const ElementType *RESTRICT
                          Start = GetData(),
                          *RESTRICT Data = Start + Count;
                      Data != Start; ) {
                        --Data;
                        if ( ::Invoke( Pred, *Data ) ) {
                                return static_cast<SizeType>( Data - Start );
                        }
                }
                return INDEX_NONE;
        }

        /**
         * Searches the array for the last occurrence of an element which
         * matches the specified predicate.
         *
         * @param Pred Predicate taking array element and returns true if
         * element matches search criteria, false otherwise.
         * @returns Index of the found element. INDEX_NONE otherwise.
         */
        template <typename Predicate>
        FORCEINLINE SizeType FindLastByPredicate( Predicate Pred ) const {
                return FindLastByPredicate( Pred, ArrayNum );
        }

        /**
         * Finds an item by key (assuming the ElementType overloads operator==
         * for the comparison).
         *
         * @param Key The key to search by.
         * @returns Index to the first matching element, or INDEX_NONE if none
         * is found.
         */
        template <typename KeyType>
        SizeType IndexOfByKey( const KeyType &Key ) const {
                const ElementType *RESTRICT Start = GetData();
                for ( const ElementType *RESTRICT
                          Data = Start,
                          *RESTRICT DataEnd = Start + ArrayNum;
                      Data != DataEnd; ++Data ) {
                        if ( *Data == Key ) {
                                return static_cast<SizeType>( Data - Start );
                        }
                }
                return INDEX_NONE;
        }

        /**
         * Finds an item by predicate.
         *
         * @param Pred The predicate to match.
         * @returns Index to the first matching element, or INDEX_NONE if none
         * is found.
         */
        template <typename Predicate>
        SizeType IndexOfByPredicate( Predicate Pred ) const {
                const ElementType *RESTRICT Start = GetData();
                for ( const ElementType *RESTRICT
                          Data = Start,
                          *RESTRICT DataEnd = Start + ArrayNum;
                      Data != DataEnd; ++Data ) {
                        if ( ::Invoke( Pred, *Data ) ) {
                                return static_cast<SizeType>( Data - Start );
                        }
                }
                return INDEX_NONE;
        }

        /**
         * Finds an item by key (assuming the ElementType overloads operator==
         * for the comparison).
         *
         * @param Key The key to search by.
         * @returns Pointer to the first matching element, or nullptr if none is
         * found.
         * @see Find
         */
        template <typename KeyType>
        FORCEINLINE const ElementType *FindByKey( const KeyType &Key ) const {
                return const_cast<TArray *>( this )->FindByKey( Key );
        }

        /**
         * Finds an item by key (assuming the ElementType overloads operator==
         * for the comparison). Time Complexity: O(n), starts iteration from the
         * beginning so better performance if Key is in the front
         *
         * @param Key The key to search by.
         * @returns Pointer to the first matching element, or nullptr if none is
         * found.
         * @see Find
         */
        template <typename KeyType>
        ElementType *FindByKey( const KeyType &Key ) {
                for ( ElementType *RESTRICT Data = GetData(),
                                            *RESTRICT DataEnd = Data + ArrayNum;
                      Data != DataEnd; ++Data ) {
                        if ( *Data == Key ) {
                                return Data;
                        }
                }

                return nullptr;
        }

        /**
         * Finds an element which matches a predicate functor.
         *
         * @param Pred The functor to apply to each element.
         * @returns Pointer to the first element for which the predicate returns
         * true, or nullptr if none is found.
         * @see FilterByPredicate, ContainsByPredicate
         */
        template <typename Predicate>
        FORCEINLINE const ElementType *FindByPredicate( Predicate Pred ) const {
                return const_cast<TArray *>( this )->FindByPredicate( Pred );
        }

        /**
         * Finds an element which matches a predicate functor.
         *
         * @param Pred The functor to apply to each element. true, or nullptr if
         * none is found.
         * @see FilterByPredicate, ContainsByPredicate
         */
        template <typename Predicate>
        ElementType *FindByPredicate( Predicate Pred ) {
                for ( int32_t i = 0; i < ArrayNum; ++i ) {
                        if ( Pred( Data[i] ) ) {
                                return &Data[i];
                        }
                }
                return nullptr;
        }

        /**
         * Filters the elements in the array based on a predicate functor.
         *
         * @param Pred The functor to apply to each element.
         * @returns TArray with the same type as this object which contains
         *          the subset of elements for which the functor returns true.
         * @see FindByPredicate, ContainsByPredicate
         */
        template <typename Predicate>
        TArray<ElementType> FilterByPredicate( Predicate Pred ) const {
                TArray<ElementType> FilterResults;
                for ( const ElementType *RESTRICT
                          Data = GetData(),
                          *RESTRICT DataEnd = Data + ArrayNum;
                      Data != DataEnd; ++Data ) {
                        if ( ::Invoke( Pred, *Data ) ) {
                                FilterResults.Add( *Data );
                        }
                }
                return FilterResults;
        }

        /**
         * Checks if this array contains the element.
         *
         * @returns	True if found. False otherwise.
         * @see ContainsByPredicate, FilterByPredicate, FindByPredicate
         */
        template <typename ComparisonType>
        bool Contains( const ComparisonType &Item ) const {
                for ( const ElementType *RESTRICT
                          Data = GetData(),
                          *RESTRICT DataEnd = Data + ArrayNum;
                      Data != DataEnd; ++Data ) {
                        if ( *Data == Item ) {
                                return true;
                        }
                }
                return false;
        }

        /**
         * Checks if this array contains element for which the predicate is
         * true.
         *
         * @param Predicate to use
         * @returns	True if found. False otherwise.
         * @see Contains, Find
         */
        template <typename Predicate>
        FORCEINLINE bool ContainsByPredicate( Predicate Pred ) const {
                return FindByPredicate( Pred ) != nullptr;
        }

        /**
         * Equality operator.
         *
         * @param OtherArray Array to compare.
         * @returns True if this array is the same as OtherArray. False
         * otherwise.
         */
        bool operator==( const TArray &OtherArray ) const {
                SizeType Count = Num();

                return Count == OtherArray.Num(); /*&&
                       CompareItems( GetData(), OtherArray.GetData(), Count );*/
        }

        /**
         * Inequality operator.
         *
         * @param OtherArray Array to compare.
         * @returns True if this array is NOT the same as OtherArray. False
         * otherwise.
         */
        FORCEINLINE bool operator!=( const TArray &OtherArray ) const {
                return !( *this == OtherArray );
        }

        	/**
         * Adds a given number of uninitialized elements into the array.
         *
         * Caution, AddUninitialized() will create elements without calling
         * the constructor and this is not appropriate for element types that
         * require a constructor to function properly.
         *
         * @param Count Number of elements to add.
         * @returns Number of elements in array before addition.
         */
        FORCEINLINE SizeType AddUninitialized( SizeType Count = 1 ) {
                CheckInvariants();
                checkSlow( Count >= 0 );

                if ( ArrayMax < ArrayNum + Count ) {
                        ResizeGrow( Count );
                }
                return ArrayNum;
        }

        /**
         * Checks that the specified address is not part of an element within
         * the container. Used for implementations to check that reference
         * arguments aren't going to be invalidated by possible reallocation.
         *
         * @param Addr The address to check.
         * @see Add, Remove
         */
        FORCEINLINE void CheckAddress( const ElementType *Addr ) const {
                check( Addr );
        }

        FORCEINLINE void ResizeGrow(int32 OldNum)
        {
                Data = (ElementType *)FMemory::Realloc(
                    Data,
                    ( ArrayMax = OldNum + ArrayNum ) * sizeof( ElementType ),
                    alignof( ElementType ) );

        }

        void RemoveAtImpl( SizeType Index, SizeType Count,
                           bool bAllowShrinking ) {
                if ( Count ) {
                        CheckInvariants();
                        checkSlow( ( Count >= 0 ) & ( Index >= 0 ) &
                                   ( Index + Count <= ArrayNum ) );

                        // DestructItems( GetData() + Index, Count );

                        // Skip memmove in the common case that there is nothing
                        // to move.
                        SizeType NumToMove = ArrayNum - Index - Count;
                        if ( NumToMove ) {
                                memmove( (uint8 *)Data +
                                             ( Index ) * sizeof( ElementType ),
                                         (uint8 *)Data +
                                             ( Index + Count ) *
                                                 sizeof( ElementType ),
                                         NumToMove * sizeof( ElementType ) );
                        }
                        ArrayNum -= Count;

                        if ( bAllowShrinking ) {
                                // ResizeShrink();
                        }
                }
        }

        /**
         * Removes an element (or elements) at given location optionally
         * shrinking the array.
         *
         * @param Index Location in array of the element to remove.
         * @param Count (Optional) Number of elements to remove. Default is 1.
         * @param bAllowShrinking (Optional) Tells if this call can shrink array
         * if suitable after remove. Default is true.
         */
        FORCEINLINE void RemoveAt( SizeType Index ) {
                RemoveAtImpl( Index, 1, true );
        }

        /**
         * Removes an element (or elements) at given location optionally
         * shrinking the array.
         *
         * @param Index Location in array of the element to remove.
         * @param Count (Optional) Number of elements to remove. Default is 1.
         * @param bAllowShrinking (Optional) Tells if this call can shrink array
         * if suitable after remove. Default is true.
         */
        template <typename CountType>
        FORCEINLINE void RemoveAt( SizeType Index, CountType Count,
                                   bool bAllowShrinking = true ) {
                static_assert( !TAreTypesEqual<CountType, bool>::Value,
                               "TArray::RemoveAt: unexpected bool passed as "
                               "the Count argument" );
                RemoveAtImpl( Index, (SizeType)Count, bAllowShrinking );
        }

        FORCEINLINE SizeType Emplace(ElementType& Item) {
                const SizeType Index = AddUninitialized( 1 );
                GetData()[Index] = Item;
                ArrayNum++;
                return Index;
        }

        FORCEINLINE SizeType Emplace(const ElementType& Item) {
                const SizeType Index = AddUninitialized( 1 );
                GetData()[Index] = Item;
                ArrayNum++;
                return Index;
        }

        /**
         * Constructs a new item at the end of the array, possibly reallocating
         * the whole array to fit.
         *
         * @param Args	The arguments to forward to the constructor of the new
         * item.
         * @return A reference to the newly-inserted element.
         */
        template <typename... ArgsType>
        FORCEINLINE ElementType &Emplace_GetRef( ArgsType &&...Args ) {
                const SizeType Index = AddUninitialized( 1 );
                ElementType *Ptr = GetData() + Index;
                new ( Ptr ) ElementType( Forward<ArgsType>( Args )... );
                return *Ptr;
        }

        FORCEINLINE SizeType Add( ElementType &Item ) { 
                CheckAddress( &Item );
                return Emplace( Item );
        }

        /**
         * Adds a new item to the end of the array, possibly reallocating the
         * whole array to fit.
         *
         * @param Item The item to add
         * @return Index to the new item
         * @see AddDefaulted, AddUnique, AddZeroed, Append, Insert
         */
        FORCEINLINE SizeType Add( const ElementType &Item ) {
                CheckAddress( &Item );
                return Emplace( Item );
        }

        /**
         * Adds a new item to the end of the array, possibly reallocating the
         * whole array to fit.
         *
         * Move semantics version.
         *
         * @param Item The item to add
         * @return A reference to the newly-inserted element.
         * @see AddDefaulted_GetRef, AddUnique_GetRef, AddZeroed_GetRef,
         * Insert_GetRef
         */
        FORCEINLINE ElementType &Add_GetRef( ElementType &&Item ) {
                CheckAddress( &Item );
                return Emplace_GetRef( MoveTempIfPossible( Item ) );
        }

        /**
         * Adds a new item to the end of the array, possibly reallocating the
         * whole array to fit.
         *
         * @param Item The item to add
         * @return A reference to the newly-inserted element.
         * @see AddDefaulted_GetRef, AddUnique_GetRef, AddZeroed_GetRef,
         * Insert_GetRef
         */
        FORCEINLINE ElementType &Add_GetRef( const ElementType &Item ) {
                CheckAddress( &Item );
                return Emplace_GetRef( Item );
        }

        /**
         * Adds new items to the end of the array, possibly reallocating the
         * whole array to fit. The new items will be zeroed.
         *
         * Caution, AddZeroed() will create elements without calling the
         * constructor and this is not appropriate for element types that
         * require a constructor to function properly.
         *
         * @param  Count  The number of new items to add.
         * @return Index to the first of the new items.
         * @see Add, AddDefaulted, AddUnique, Append, Insert
         */
        SizeType AddZeroed( SizeType Count = 1 ) {
                const SizeType Index = AddUninitialized( Count );
                ZeroMemory( (uint8 *)Data +
                                      Index * sizeof( ElementType ),
                                  Count * sizeof( ElementType ) );
                return Index;
        }

        /**
         * Adds a new item to the end of the array, possibly reallocating the
         * whole array to fit. The new item will be zeroed.
         *
         * Caution, AddZeroed_GetRef() will create elements without calling the
         * constructor and this is not appropriate for element types that
         * require a constructor to function properly.
         *
         * @return A reference to the newly-inserted element.
         * @see Add_GetRef, AddDefaulted_GetRef, AddUnique_GetRef, Insert_GetRef
         */
        ElementType &AddZeroed_GetRef() {
                const SizeType Index = AddUninitialized( 1 );
                ElementType *Ptr = GetData() + Index;
                ZeroMemory( Ptr, sizeof( ElementType ) );
                return *Ptr;
        }

     protected:
       InElementType *Data;
       int32 ArrayNum;
       int32 ArrayMax;

     public:



       	typedef TIndexedContainerIterator<TArray, ElementType, SizeType>
           TIterator;
       typedef TIndexedContainerIterator<const TArray, const ElementType,
                                         SizeType>
           TConstIterator;

       /**
        * Creates an iterator for the contents of this array
        *
        * @returns The iterator.
        */
       TIterator CreateIterator() { return TIterator( *this ); }

       /**
        * Creates a const iterator for the contents of this array
        *
        * @returns The const iterator.
        */
       TConstIterator CreateConstIterator() const {
               return TConstIterator( *this );
       }

       	typedef TCheckedPointerIterator<ElementType, SizeType>
           RangedForIteratorType;
       typedef TCheckedPointerIterator<const ElementType, SizeType>
           RangedForConstIteratorType;

       FORCEINLINE RangedForIteratorType begin() {
               return RangedForIteratorType( ArrayNum, GetData() );
       }
       FORCEINLINE RangedForConstIteratorType begin() const {
               return RangedForConstIteratorType( ArrayNum, GetData() );
       }
       FORCEINLINE RangedForIteratorType end() {
               return RangedForIteratorType( ArrayNum, GetData() + Num() );
       }
       FORCEINLINE RangedForConstIteratorType end() const {
               return RangedForConstIteratorType( ArrayNum, GetData() + Num() );
       }
};

template<typename T>
struct TTupleBaseElement
{
        TTupleBaseElement( TTupleBaseElement && ) = default;
        TTupleBaseElement( const TTupleBaseElement & ) = default;
        TTupleBaseElement &operator=( TTupleBaseElement && ) = default;
        TTupleBaseElement &operator=( const TTupleBaseElement & ) = default;

        TTupleBaseElement() : Value() {}

        T Value;
};

template<typename ...Types>
struct TTupleBase : TTupleBaseElement<Types>
{
        TTupleBase() = default;
        TTupleBase( TTupleBase &&Other ) = default;
        TTupleBase( const TTupleBase &Other ) = default;
        TTupleBase &operator=( TTupleBase &&Other ) = default;
        TTupleBase &operator=( const TTupleBase &Other ) = default;
};

template<typename... Types> struct TTuple : TTupleBase < Types> {
      public:
        TTuple() = default;
        TTuple( TTuple && ) = default;
        TTuple( const TTuple & ) = default;
        TTuple &operator=( TTuple && ) = default;
        TTuple &operator=( const TTuple & ) = default;
};

template<typename KeyType, typename ValueType>
using TPair = TTuple<KeyType, ValueType>;

/** Allocated elements are overlapped with free element info in the element
 * list. */
template <typename ElementType> union TSparseArrayElementOrFreeListLink {
        /** If the element is allocated, its value is stored here. */
        ElementType ElementData;

        struct {
                /** If the element isn't allocated, this is a link to the
                 * previous element in the array's free list. */
                int32 PrevFreeIndex;

                /** If the element isn't allocated, this is a link to the next
                 * element in the array's free list. */
                int32 NextFreeIndex;
        };
};

template <int32 Size, uint32 Alignment> struct TAlignedBytes {
        alignas( Alignment ) uint8 Pad[Size];
};

template <uint32 NumInlineElements> class TInlineAllocator {
      public:
        template <typename ElementType> class ForElementType {
              private:
                static constexpr int32 ElementSize = sizeof( ElementType );
                static constexpr int32 ElementAlign = alignof( ElementType );

                static constexpr int32 InlineDataSizeBytes =
                    NumInlineElements * ElementSize;

              private:
                TAlignedBytes<ElementSize, ElementAlign>
                    InlineData[NumInlineElements];
                ElementType *SecondaryData;

              public:
                ForElementType()
                    : InlineData{ 0x0 }, SecondaryData( nullptr ) {}

                ForElementType( ForElementType && ) = default;
                ForElementType( const ForElementType & ) = default;

              public:
                ForElementType &operator=( ForElementType && ) = default;
                ForElementType &operator=( const ForElementType & ) = default;

              public:
                inline const ElementType *GetAllocation() const {
                        return SecondaryData
                                   ? SecondaryData
                                   : reinterpret_cast<const ElementType *>(
                                         &InlineData );
                }

                inline uint32 GetInitialCapacity() const {
                        return NumInlineElements;
                }
        };
};

#define NumBitsPerDWORD ( (int32)32 )
#define NumBitsPerDWORDLogTwo ( (int32)5 )

struct FBitSet {
        /** Clears the next set bit in the mask and returns its index. */
        static FORCEINLINE uint32 GetAndClearNextBit( uint32 &Mask ) {
                const uint32 LowestBitMask = ( Mask ) & ( -(int32)Mask );
                const uint32 BitIndex = FMath::FloorLog2( LowestBitMask );
                Mask ^= LowestBitMask;
                return BitIndex;
        }

        // Clang generates 7 instructions for int32 DivideAndRoundUp but only 2
        // for uint32
        static constexpr uint32 BitsPerWord = NumBitsPerDWORD;

        FORCEINLINE static uint32 CalculateNumWords( int32 NumBits ) {
                checkSlow( NumBits >= 0 );
                return FMath::DivideAndRoundUp( static_cast<uint32>( NumBits ),
                                                BitsPerWord );
        }
};

class TBitArray
{
        typedef uint32 WordType;
        static constexpr WordType FullWordMask = (WordType)-1;
      public:
        TBitArray()
            : NumBits( 0 ), MaxBits( AllocatorInstance.GetInitialCapacity() *
                                     NumBitsPerDWORD ) {
                // ClearPartialSlackBits is already satisfied since final word
                // does not exist when NumBits == 0
        }

        /**
         * Minimal initialization constructor.
         * @param Value - The value to initial the bits to.
         * @param InNumBits - The initial number of bits in the array.
         */
        FORCEINLINE explicit TBitArray( bool bValue, int32 InNumBits )
            : MaxBits( AllocatorInstance.GetInitialCapacity() *
                       NumBitsPerDWORD ) {
        }

     public:
        void CheckInvariants() const {

                checkf( NumBits <= MaxBits,
                        TEXT( "TBitArray::NumBits (%d) should never be greater "
                              "than MaxBits (%d)" ),
                        NumBits, MaxBits );
                checkf(
                    NumBits >= 0 && MaxBits >= 0,
                    TEXT(
                        "NumBits (%d) and MaxBits (%d) should always be >= 0" ),
                    NumBits, MaxBits );

                // Verify the ClearPartialSlackBits invariant
                const int32 UsedBits = ( NumBits % NumBitsPerDWORD );
                if ( UsedBits != 0 ) {
                        const int32 LastWordIndex = NumBits / NumBitsPerDWORD;
                        const uint32 SlackMask = FullWordMask << UsedBits;

                        const uint32 LastWord = *( GetData() + LastWordIndex );
                        checkf( ( LastWord & SlackMask ) == 0,
                                TEXT( "TBitArray slack bits are non-zero, this "
                                      "will result in undefined behavior." ) );
                }
        }

        FORCEINLINE const uint32 *GetData() const {
                return (uint32 *)AllocatorInstance.GetAllocation();
        }

        FORCEINLINE uint32 *GetData() {
                return (uint32 *)AllocatorInstance.GetAllocation();
        }



     private:
       TInlineAllocator<4>::ForElementType<int32> AllocatorInstance;
       int32 NumBits;
       int32 MaxBits;

       	/**
        * Clears the slack bits within the final partially relevant Word
        */
       void ClearPartialSlackBits() {
               // TBitArray has a contract about bits outside of the active
               // range - the bits in the final word past NumBits are guaranteed
               // to be 0 This prevents easy-to-make determinism errors from
               // users of TBitArray that do not carefully mask the final word.
               // It also allows us optimize some operations which would
               // otherwise require us to mask the last word.
               const int32 UsedBits = NumBits % NumBitsPerDWORD;
               if ( UsedBits != 0 ) {
                       const int32 LastWordIndex = NumBits / NumBitsPerDWORD;
                       const uint32 SlackMask =
                           FullWordMask >> ( NumBitsPerDWORD - UsedBits );

                       uint32 *LastWord = ( GetData() + LastWordIndex );
                       *LastWord = *LastWord & SlackMask;
               }
       }

       	FORCEINLINE uint32 GetNumWords() const {
               return FBitSet::CalculateNumWords( NumBits );
       }

       FORCEINLINE uint32 GetMaxWords() const {
               return FBitSet::CalculateNumWords( MaxBits );
       }

       FORCEINLINE static void SetWords( uint32 *Words, int32 NumWords,
                                         bool bValue ) {
               if ( NumWords > 8 ) {
                       memset( Words, bValue ? 0xff : 0,
                                        NumWords * sizeof( uint32 ) );
               } else {
                       uint32 Word = bValue ? ~0u : 0u;
                       for ( int32 Idx = 0; Idx < NumWords; ++Idx ) {
                               Words[Idx] = Word;
                       }
               }
       }

       /**
        * Removes all bits from the array retaining any space already allocated.
        */
       void Reset() { NumBits = 0; }

};

template<typename InElementType>
class TSparseArray
{
      private:
        typedef TSparseArrayElementOrFreeListLink<
            TAlignedBytes<sizeof( InElementType ), alignof( InElementType )>>
            FElementOrFreeListLink;

      public:
        /** Accessor for the element or free list data. */
        FElementOrFreeListLink &GetData( int32 Index ) {
                return ( (FElementOrFreeListLink *)Data.GetData() )[Index];
        }

        /** Accessor for the element or free list data. */
        const FElementOrFreeListLink &GetData( int32 Index ) const {
                return ( (FElementOrFreeListLink *)Data.GetData() )[Index];
        }

      private:

        typedef TArray<FElementOrFreeListLink>
            DataType;
        DataType Data;
        typedef TBitArray
            AllocationBitArrayType;
        TBitArray AllocationFlags;
        int32 FirstFreeIndex;
        int32 NumFreeIndices;


};

template<typename InElementType>
class TSetElement
{
      private:
        template <typename SetDataType> friend class TSet;
      private:
        InElementType Value;
        int32 HashNextId;
        int32 HashIndex;
};



template<typename InElementType>
class TSet
{
      public:
        typedef InElementType ElementType;

      private:
        typedef TSetElement<InElementType> SetElementType;

      public:
        /** Initialization constructor. */
        FORCEINLINE TSet() : HashSize( 0 ) {}

      private:

        typedef TSparseArray<SetElementType>
              ElementArrayType;
        using HashType = TInlineAllocator<1>::ForElementType<int32>;

        ElementArrayType Elements;

        mutable HashType Hash;
        mutable int32 HashSize;

        public:
};

template<typename KeyType, typename ValueType>
class TMapBase
{
       template <typename OtherKeyType, typename OtherValueType>
       friend class TMapBase;

       public:
       typedef TPair<KeyType, ValueType> ElementType;

       protected:
       TMapBase() = default;
       TMapBase( TMapBase && ) = default;
       TMapBase( const TMapBase & ) = default;
       TMapBase &operator=( TMapBase && ) = default;
       TMapBase &operator=( const TMapBase & ) = default;

       protected:
       typedef TSet<ElementType> ElementSetType;

       /** A set of the key-value pairs in the map. */
       ElementSetType Pairs;

};

template<typename InKeyType, typename InValueType>
class TMap : TMapBase<InKeyType, InValueType>
{
      public:
        typedef InKeyType KeyType;
        typedef InValueType ValueType;

        TMap() = default;
        TMap( TMap && ) = default;
        TMap( const TMap & ) = default;
        TMap &operator=( TMap && ) = default;
        TMap &operator=( const TMap & ) = default;

        public:

};

#define UE_PTRDIFF_TO_INT32( argument ) static_cast<int32>( argument )
#define UE_PTRDIFF_TO_UINT32( argument ) static_cast<uint32>( argument )

#define _tcslen wcslen
#define _tcsstr wcsstr
static FORCEINLINE int32 Strlen( const TCHAR *String ) {
        return _tcslen( String );
}

typedef TCHAR WIDECHAR;


FORCEINLINE const typename TCHAR* Strstr( const TCHAR*String, const TCHAR*Find ) {
        return _tcsstr( String, Find );
}

class FString
{
      private:
        /** Array holding the character data */
        typedef TArray<TCHAR> DataType;
        DataType Data;

      public:
        using ElementType = TCHAR;

		FString() = default;
        FString( FString && ) = default;
        FString( const FString & ) = default;
        FString &operator=( FString && ) = default;
        FString &operator=( const FString & ) = default;




        /** Get the length of the string, excluding terminating character */
        FORCEINLINE int32 Len() const {
                return Data.Num() ? Data.Num() - 1 : 0;
        }
        
        /**
         * Tests if index is valid, i.e. greater than or equal to zero, and less
         * than the number of characters in this string (excluding the null
         * terminator).
         *
         * @param Index Index to test.
         *
         * @returns True if index is valid. False otherwise.
         */
        FORCEINLINE bool IsValidIndex( int32 Index ) const {
                return Index >= 0 && Index < Len();
        }

        /**
         * Get pointer to the string
         *
         * @Return Pointer to Array of TCHAR if Num, otherwise the empty string
         */
        FORCEINLINE const TCHAR *operator*() const {
                return Data.Num() ? Data.GetData() : TEXT( "" );
        }

     public:
        FString( const wchar_t *Str ) {
               const uint32 Length = static_cast<uint32>( wcslen( Str ) );
               Data.AddUninitialized( Length + 1 );
               memcpy( Data.GetData(), Str,
                      ( Length + 1 ) * sizeof( wchar_t ) );
        }
        
        FString( const std::string &Str )
            : FString( std::wstring( Str.begin(), Str.end() )
                           .c_str() )
        {
        }
        
        FString( const std::wstring &Str )
            : FString( Str.c_str() )
        {
        }

     public:
        std::string ToString() {
               wchar_t *ProcessedData = Data.GetData();
               std::wstring WideDataString( ProcessedData );

               return std::string( WideDataString.begin(),
                                   WideDataString.end() );
        }

        std::string ToString() const {
                const wchar_t *ProcessedData = Data.GetData();
                std::wstring WideDataString( ProcessedData );

                return std::string( WideDataString.begin(),
                                    WideDataString.end() );
        }

       std::wstring ToWideString() {
               wchar_t *ProcessedData = Data.GetData();
               std::wstring WideDataString( ProcessedData );

               return WideDataString;
       }

       std::wstring ToWideString() const
       {
               const wchar_t *ProcessedData = Data.GetData();
               std::wstring WideDataString( ProcessedData );

               return WideDataString;
       }

       bool IsEmpty() const { 
               bool bEmpty = ToWideString().empty();
               return bEmpty;
       }

       bool operator==( FString &Other ) {
               return this->Data.Data == Other.Data.Data;
       }

       bool operator==( const FString &Other ) {
               return this->Data.Data == Other.Data.Data;
       }

       bool operator==( const FString &Other ) const {
               return this->Data.Data == Other.Data.Data;
       }

       bool operator==( FString Other ) {
               return this->Data.Data == Other.Data.Data;
       }
       
       void ParseIntoArray( TArray<FString> &OutArray, const FString &Delimiter,
                            bool bCullEmpty = true ) const {
               size_t Start = 0;
               size_t End;
               const std::wstring &Source = ToWideString();
               const std::wstring &Delim = Delimiter.ToWideString();

               while ( ( End = Source.find( Delim, Start ) ) !=
                       std::wstring::npos ) {
                       std::wstring substractedstr =
                           Source.substr( Start, End - Start );
                       FString Token =
                           FString( substractedstr );
                       if ( !bCullEmpty || !Token.IsEmpty() )
                               OutArray.Add( Token );
                       Start = End + Delim.length();
               }
               std::wstring substr = Source.substr( Start );
               FString LastToken = FString( substr );
               if ( !bCullEmpty || !LastToken.IsEmpty() )
                       OutArray.Add( LastToken );
       }
};


}
