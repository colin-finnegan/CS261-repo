/*
 * CS 261 Example
 *
 * Illustrates the sizes of C integer data types.
 */

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#define PRINT_SIZE(T) printf("%20s %2lu bytes:\t", #T, (long unsigned) sizeof(T))

int main ()
{

    printf("\n");
    PRINT_SIZE(char);
    printf("%20lld -->  %-20lld\n" , (long long) SCHAR_MIN, (long long) SCHAR_MAX) ;
    PRINT_SIZE(unsigned char);
    printf("%20llu -->  %-20llu\n" , 0LLU , (unsigned long long) UCHAR_MAX) ;

    PRINT_SIZE(short);
    printf("%20lld -->  %-20lld\n" , (long long) SHRT_MIN, (long long) SHRT_MAX) ;

    PRINT_SIZE(unsigned short);
    printf("%20llu -->  %-20llu\n" , 0LLU , (unsigned long long) USHRT_MAX) ;

    PRINT_SIZE(int);
    printf("%20lld -->  %-20lld\n" , (long long) INT_MIN, (long long) INT_MAX) ;

    PRINT_SIZE(unsigned int);
    printf("%20llu -->  %-20llu\n" , 0LLU, (unsigned long long) UINT_MAX) ;

    PRINT_SIZE(long);
    printf("%20lld -->  %-20lld\n" , (long long) LONG_MIN, (long long) LONG_MAX) ;

    PRINT_SIZE(unsigned long);
    printf("%20llu -->  %-20llu\n" , 0LLU, (unsigned long long) ULONG_MAX) ;

    PRINT_SIZE(long long);
    printf("%20lld -->  %-20lld\n" , LLONG_MIN, LLONG_MAX) ;

    PRINT_SIZE(unsigned long long);
    printf("%20llu -->  %-20llu\n", 0LLU, ULLONG_MAX) ;

    PRINT_SIZE(int8_t);
    printf("%20d -->  %-20d\n" , INT8_MIN, INT8_MAX) ;

    PRINT_SIZE(uint8_t);
    printf("%20d -->  %-20d\n", 0, UINT8_MAX) ;

    PRINT_SIZE(int16_t);
    printf("%20d -->  %-20d\n" , INT16_MIN, INT16_MAX) ;

    PRINT_SIZE(uint16_t);
    printf("%20d -->  %-20d\n", 0, UINT16_MAX) ;

    PRINT_SIZE(int32_t);
    printf("%20d -->  %-20d\n" , INT32_MIN, INT32_MAX) ;

    PRINT_SIZE(uint32_t);
    printf("%20d -->  %-20u\n", 0, UINT32_MAX) ;

    PRINT_SIZE(int64_t);
    printf("%20ld -->  %-20ld\n" , INT64_MIN, INT64_MAX) ;

    PRINT_SIZE(uint64_t);
    printf("%20d -->  %-20lu\n" , 0, UINT64_MAX) ;

    PRINT_SIZE( __int128);
    printf("\n");
    
    PRINT_SIZE(unsigned __int128);
    printf("\n\n");
    
}
