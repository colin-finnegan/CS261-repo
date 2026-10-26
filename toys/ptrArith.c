/*
 * CS 261 Example
 *
 * Pointer Arithmetic. 
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    long age;
    double gpa ;
    char  name[32] ;	
} student_t ;

int main()
{
	int  *pA ;
	char *pC ;
	long *pD ;
    student_t *pS ;
	short int *pX , x[5] = { 10, 20, 30, 40, 50 } ;

    pA = 1000 ;  
    printf("pA = %lu, pA+1 = %lu , pA+2 = %lu\n" , pA , pA+1 , pA+2 );
    
    pC = 2000 ;  
    printf("pC = %lu, pC+1 = %lu , pC-2 = %lu\n" , pC , pC+1 , pC-2 );
    
    pD = 3000 ;  
    printf("pD = %lu, pD+1 = %lu , pD+2 = %lu\n" , pD , pD+1 , pD+2 );

    pS = 4000 ;  
    printf("\nThe size of student_t structure is %d bytes\n" , 
           sizeof(student_t) );
    printf("pS = %lu, pS+1 = %lu , pS+2 = %lu\n\n" , pS ,  pS+1 , pS+2 );

    pX = x ;  // or &x[0] ...same effect
    printf("x[0] = %hd , x[3] = %hd\n\n" , *pX , *(pX+3) );
    
    return 0;
}
