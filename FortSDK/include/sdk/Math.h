#pragma once
#define CONSTEXPR constexpr

namespace SDK
{
	class FMath {
      public:
            template <class T>
            static CONSTEXPR FORCEINLINE T Min( const T A, const T B ) {
                    return ( A <= B ) ? A : B;
            }

            template <class T>
            static constexpr FORCEINLINE T
            DivideAndRoundUp( T Dividend, T Divisor ) {
                    return ( Dividend + Divisor - 1 ) / Divisor;
            }

            static FORCEINLINE UINT32 FloorLog2( UINT32 Value ) {
                    // Use BSR to return the log2 of the integer
                    unsigned long Log2;
                    if ( _BitScanReverse( &Log2, Value ) != 0 ) {
                            return Log2;
                    }

                    return 0;
            }
    };
}