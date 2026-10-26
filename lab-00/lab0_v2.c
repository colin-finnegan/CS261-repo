/*
 * CS 261 Example
 *
 * fopen / printf / fscanf / Intro to Scope of Variables
 *
 * Building command:     gcc lab0.c  -o lab0
 */

#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *dataFp ;
	unsigned char start , count ;
	int i /* = 640  */ ;
	
	dataFp = fopen ("testData.txt" , "r" ) ;
	if ( dataFp == NULL )  // Did it fail ?
	{
		printf("ERROR:  could not open the file\n");
        exit( EXIT_FAILURE );
	}
    
	fscanf( dataFp , "%hhx %hhd" , &start , &count );
	printf( "Will count starting at HEX %X  for %hhd iterations\n" , start , count );

	for ( /* int */  i=0 ; i< count ; i++ )
		printf( "  %02hhX\n" , start+i ) ;

	printf("\nFinal value of 'i' = %d\n" , i ) ;
	
	return 0;
}
