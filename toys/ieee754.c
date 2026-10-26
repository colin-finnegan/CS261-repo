/* The IEEE-754 Floating-Point Representation */
/* By: Mohamed Aboutabl */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>


void showBytes( void *p , size_t n)
{
    unsigned i;
    for ( i=0 ; i<n ; i++)
    {
        printf(" %02X" , * ( (uint8_t*) p) );
        p++;
    }
}

void showBytesReverse( void *p , size_t n)
{
    unsigned i;
    p += n-1 ;
    for ( i=0 ; i<n ; i++)
    {
        printf(" %02X" , * ( (uint8_t*) p) );
        p--;
    }
}

int main(int argc , char **argv)
{
    float    flt , *fltPtr ;
	unsigned ux ;

    if ( argc < 2 )
    {
        puts("must provide a floating-point value as a command-line argument" );
        return 0;
    }

    flt = strtof( argv[1] , NULL ); 
    
    printf ("The value '%s' is stored as Single Precision: %20.10e\n" , argv[1] , flt ); 
    printf ("\tEncoded as   HEX : ") ; showBytesReverse( &flt , sizeof(float) );
    puts("");
    printf ("\tStored in Memory : ") ; showBytes( &flt , sizeof(float) );
    puts("");
    
	
	printf("\nEnter a 32-bit hexdecimal number --> " );
	scanf(" %x" , &ux);
	fltPtr = (float *) & ux ;

	printf("\nThe hexadecimal value:"); 
    showBytesReverse( fltPtr , sizeof(float) ) ; 
    printf("\n  translates to IEEE-754 Single precision: %20.10e\n" , *fltPtr );
   
}
