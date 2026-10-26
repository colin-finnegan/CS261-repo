/*
 * CS 261 Example
 *
 * Demonstrates the bitwise operations in C.
 */
 
#include <stdio.h>

int main()
{

    unsigned int  a =  0x0f0ab3c2;
    unsigned int  b =  0xff27efb1;
printf("\n=======  Bitwise Operations  ====\n\n");    
    printf(" ~ %.8x            = %.8x\n",  a ,  ~a     );
    printf("~~ %.8x            = %.8x\n",  a , ~~a     );
    puts("") ; 
	printf("   %.8x | %.8x = %.8x\n",  a , b , a | b );
    puts("") ; 
    printf("   %.8x & %.8x = %.8x\n",  a , b , a & b );
    puts("") ; 
    printf("   %.8x ^ %.8x = %.8x\n",  a , b , a ^ b );
    puts("") ; 
printf("\n=======  Logic  Operations  ====\n\n") ;
    printf(" !   %.8x               = %.8x\n",  a , !a     );
    printf(" !!  %.8x               = %.8x\n",  a , !!a     );
    puts("") ; 
    printf("     %.8x  &&  %.8x = %.8x\n",  a , b ,  a  &&  b );
    puts("") ; 
    printf("     %.8x  ||  %.8x = %.8x\n",  a , b ,  a  ||  b );
    puts("") ; 
    printf("     %.8x  XOR %.8x = %.8x\n",  a , b , !a  != !b );

    puts("");
    return 0;
}
