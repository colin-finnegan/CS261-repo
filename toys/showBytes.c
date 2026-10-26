/*
 * CS 261 Example
 *
 * Showing the Bytes in Memory
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


void showBytes( void *q , size_t n)
{
    unsigned i;
    uint8_t *p = (uint8_t *) q ;
    for ( i=0 ; i<n ; i++)
    {
        printf(" %02x" , *p );
        p++;
    }
}

int main()
{
	short x = 0xAB98  , *xPtr = &x ;
	int   y = 0x12345678 ;
	long  z = 0x1A2B3C4D5E6F1122L ;
	
	printf( "x    = 0x%-18hX ,  In memory: " , x )  ; 
	showBytes ( & x , sizeof(x) ) ; 
	puts("") ;

	printf( "xPtr = %-20p ,  In memory: " ,  xPtr )  ; 
	showBytes ( & xPtr , sizeof(xPtr) ) ; 
	puts("") ;

	printf( "y    = 0x%-18X ,  In memory: " , y )  ; 
	showBytes ( & y , sizeof(y) ) ; 
	puts("") ;

	printf( "z    = 0x%-18lX ,  In memory: " , z )  ; 
	showBytes ( & z , sizeof(z) ) ; 
	puts("") ;
	
}