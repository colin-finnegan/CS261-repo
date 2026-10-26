/*
 * CS 261: Main driver
 *
 * Name: Colin Finnegan
 */

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "elf.h"
#include "p1-check.h"

int main (int argc, char **argv)
{
    bool show_header = false; // Flag to indicate if -H is provided
    char *fileName = NULL;    // File name from command line
    FILE *file = NULL;        // File pointer
    elf_hdr_t hdr;            // Mini-ELF header structure

    // Parse command-line arguments
    if ( !parse_command_line_p1 ( argc , argv , &show_header , &fileName ) )
	{
        exit ( EXIT_FAILURE ); // Command-line parsing failed
    }

    // If -h is provided, exit successfully
    if ( fileName == NULL )
	{
        exit ( EXIT_SUCCESS );
    }

    // Open the Mini-ELF file
    file = fopen ( fileName , "r" );
    if ( !file )
	{
        printf( "Failed to read file\n" );
        exit ( EXIT_FAILURE );
    }

	fseek ( file , 0 , SEEK_END );
	long file_size = ftell ( file );
	fseek ( file , 0 , SEEK_SET );
	
	if ( file_size < sizeof ( elf_hdr_t ) )
	{
		printf ("Failed to read file\n" );
		fclose ( file );
		exit ( EXIT_FAILURE );
	}
	
    // Read and validate the Mini-ELF header
    if ( !read_header ( file , &hdr ) )
	{
        printf ("Failed to read file\n" );
        fclose ( file );
        exit ( EXIT_FAILURE );
    }

    // If -H is provided, dump the header
    if ( show_header )
	{
        dump_header ( hdr );
    }

    // Close the file
    fclose ( file );
    exit ( EXIT_SUCCESS );
}

