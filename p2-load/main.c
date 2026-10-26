/*
 *  CS 261 PA2: Mini-ELF loader Main driver
 *
 * Name: Colin Finnegan
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "p1-check.h"
#include "p2-load.h"
#include "y86.h"

int main (int argc, char **argv)
{
    
    // Start from your main() of P1, thenmodify and add code as appropriate
    bool header = false; // Flag to indicate if -H is provided
	bool segments = false;
	bool membrief = false;
	bool memfull = false;
    char *fileName = NULL;    // File name from command line
    FILE *file = NULL;        // File pointer
    elf_hdr_t hdr;            // Mini-ELF header structure
	elf_phdr_t *phdr = NULL;

    // Parse command-line arguments
    if ( !parse_command_line_p2 ( argc , argv , &header , &segments , &membrief , &memfull , &fileName ) )
	{
        exit ( EXIT_FAILURE );
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
        printf( "Failed to Open File\n" );
        exit ( EXIT_FAILURE );
    }

    // Read and validate the Mini-ELF header
    if ( !read_header ( file , &hdr ) )
	{
        printf ("Failed to Read ELF Header\n" );
        fclose ( file );
        exit ( EXIT_FAILURE );
    }
	

    // If -H is provided, dump the header
    if ( header )
	{
        dump_header ( hdr );
		printf ( "\n" );
    }
	
	uint16_t num_phdrs = hdr.e_num_phdr;
	phdr = malloc ( num_phdrs * sizeof ( elf_phdr_t ) );
	if ( !phdr )
	{
		printf ( "Failed to Allocate Dynamic Memory\n" );
		fclose ( file );
		exit ( EXIT_FAILURE );
	}
	
	for ( uint16_t i = 0; i < num_phdrs; i++ ) 
	{
        uint16_t offset = hdr.e_phdr_start + i * sizeof ( elf_phdr_t );
        if ( !read_phdr ( file , offset , &phdr[i] ) ) 
		{
            printf ( "Failed to Read Program Header\n" );
            free ( phdr );
            fclose ( file );
            exit ( EXIT_FAILURE );
        }
    }
	
	if ( segments )
	{
		dump_phdrs ( num_phdrs , phdr );
	}
	
	memory_t memory = malloc ( MEMSIZE * sizeof ( uint8_t ) );
    if ( !memory ) 
	{
        printf ( "Failed to Allocate Dynamic memory\n" );
        free ( phdr );
        fclose ( file );
        return EXIT_FAILURE;
    }
	
	 for ( uint16_t i = 0; i < num_phdrs; i++ ) 
	 {
        if ( !load_segment ( file , memory , phdr[i] ) ) 
		{
            printf ( "Failed to Load Segment\n" );
            free ( phdr );
            free ( memory );
            fclose ( file );
            return EXIT_FAILURE;
        }
    }

    // Output memory contents (brief)
    if ( membrief ) 
	{
        for ( uint16_t i = 0; i < num_phdrs; i++ ) 
		{
            if ( phdr[i].p_filesz == 0) continue;
            uint16_t start = phdr[i].p_vaddr & 0xFFF0;
            uint16_t end = phdr[i].p_vaddr + phdr[i].p_filesz;
            dump_memory ( memory , start , end );
        }
    }

	
	if ( memfull )
	{
		dump_memory ( memory , 0x0000 , MEMSIZE );
	}
    // Close the file
	free ( phdr );
	free ( memory );
    fclose ( file );
    return EXIT_SUCCESS;
}

