/*
 * CS 261: Driver for p0-getopt
 *
 * Name: COLIN FINNEGAN  < or receive no credit for this project >
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

#include "p0-getopt.h"

#define MAXBUF  2000

int main (int argc, char **argv)
{
	uint8_t     data[MAXBUF] ;
        
    bool opS, opN;
    unsigned sVal, nVal ;
    char *inFileName;
    
	//Calling parse_command_line_p0(), exits if parsing fails.
    if ( ! parse_command_line_p0( argc , argv , &opS , &sVal , &opN,
		&nVal , &inFileName ) ) 
    {
		exit( EXIT_FAILURE );
    }

	//Exits if parsing succeeds, but no filename was returned.
	if ( inFileName == NULL )
	{
		exit( EXIT_SUCCESS );
	}
	
	//Attempts to open the file, exits if opening fails.
	FILE* inFile = fopen(inFileName, "r");
	if ( inFile == NULL )
	{
		printf("Failed to Open file");
		exit( EXIT_FAILURE);
	}
	
	//Exits if reading bytes fails.
	if ( ! read_bytes ( inFile , sVal , nVal , data ) )
	{
		printf( "Failed to Read file" );
		fclose( inFile );
		exit(EXIT_FAILURE);
	}
	
	//Printing the bytes read from the file.
	for (int i = 0; i < nVal; i++)
	{
		if (i % 16 == 0) 
        {
            printf("%08x  ", i + sVal);
        }
	
        printf("%02x", data[i]);

        if (i % 8 == 7)
		{    
			printf(" ");
        }

        if (i % 16 != 15 && i != nVal - 1)
		{    
			printf(" ");
        }

        if (i % 16 == 15 || i == nVal - 1) 
		{
            printf("\n");
        }
    }

	//Closes file after reading.
    fclose(inFile);
   
    return EXIT_SUCCESS;
}
