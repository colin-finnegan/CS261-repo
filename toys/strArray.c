/*
 * CS 261 Example
 *
 * Illustrates arrays of C strings.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAXNAMELEN 20
int main ()
{
	//==========================================================================
	// First, illustration of using string arrays and pointers interchangeably
	//==========================================================================

    char *names[] = { "Person Zero" , "Person One" , "Person Two" } ;
    char **ptrs;
    
    ptrs = names ;
    for( int i=0 ; i<3 ; i++)
        printf("names[%d] is '%s' \t and ptrs[%d] is '%s'\n" 
           , i , names[i] , i , ptrs[i] );

	//==========================================================================
	// Second, illustration of dynamic string arrays and pointers
	//==========================================================================

    unsigned numStudents ;
    printf("How many students do we have today?  " );
    scanf("%u" , &numStudents );

	// Clear the \n leftover after previous scanf to avoid problems with fgets() later
    while ( getchar() != '\n' ) ;  

    char ** stdList;
    stdList = ( char ** )  malloc( numStudents * sizeof( char *) ) ;

    if( stdList == NULL )
        {   printf ("Out of Memory on the heap\n");  exit(1); }

    for( int i=0 ; i < numStudents ; i++ )
    {
        stdList[i] = ( char * ) malloc( MAXNAMELEN * sizeof(char) ) ;

        if( stdList[i] == NULL )
            {   printf ("Out of Memory on the heap\n");  exit(1); }

        printf("Enter name of student #%d (max %d chars) --> " , i , MAXNAMELEN -1 );
        fgets( stdList[i] , MAXNAMELEN , stdin ) ;

    }

    printf("\nHere is the name list you entered:\n");
    for( int i=0 ; i < numStudents ; i++ )
	{
        unsigned long len = strlen( stdList[i] ) ; 

		printf("student #%d (%2lu characters) --> '%s'\n" 
				, i , len , stdList[i] ) ;
	}

    // Now, free up the heap memory
    for( int i=0 ; i < numStudents ; i++ )
        free( stdList[i] ) ;

    free( stdList ) ;
}

