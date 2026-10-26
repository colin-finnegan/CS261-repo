/*
 * CS 261 Example
 *
 * Demonstrates the difference between arithmetic and logical right shifts, and
 * the default C implementation:
         arithmetic for signed ints, 
         logical for unsigned ints).
 */

#include <stdio.h>

int main()
{
             int   x  = (int)0x0f000000;
             int   y  = (int)0xff000000;
    unsigned int  ux  =      0x0f000000;
    unsigned int  uy  =      0xff000000;

             int  shx =  x >> 8;     // arithmetic
             int  shy =  y >> 8;
    unsigned int shux = ux >> 8;     // logical
    unsigned int shuy = uy >> 8;

    printf(" int: %08x >> 8 = %08x\n",  x,  shx);
    printf(" int: %08x >> 8 = %08x\n",  y,  shy);
    printf("uint: %08x >> 8 = %08x\n", ux, shux);
    printf("uint: %08x >> 8 = %08x\n", uy, shuy);
    printf("8 >> 2 = %d\n" , 8 >> 2 );

    return 0;
}
