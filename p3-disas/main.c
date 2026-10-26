/*
 * CS 261 PA3: Mini-ELF disassembler Main driver
 *
 * Name: Colin Finnegan
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "p1-check.h"
#include "p2-load.h"
#include "p3-disas.h"

int main (int argc, char **argv)
{

    // Build starting from your PA-02 main.c and up
    bool header = false;      // Flag for -H option
    bool segments = false;    // Flag for -S option
    bool membrief = false;    // Flag for -m option
    bool memfull = false;     // Flag for -M option
    bool disas_code = false;  // Flag for -d option (new for P3)
    bool disas_data = false;  // Flag for -D option (new for P3)
    char *fileName = NULL;    // File name from command line
    FILE *file = NULL;        // File pointer
    elf_hdr_t hdr;            // Mini-ELF header structure
    elf_phdr_t *phdr = NULL;
    memory_t memory = NULL;

    // Parse command-line arguments (now using P3 version)
    if ( !parse_command_line_p3 ( argc , argv , &header , &segments , &membrief , 
                             &memfull , &disas_code , &disas_data , &fileName ) ) 
	{
        exit ( EXIT_FAILURE );
    }

    // If no filename provided (just -h), exit successfully
    if ( fileName == NULL ) 
	{
        exit ( EXIT_SUCCESS );
    }

    // Open the Mini-ELF file
    file = fopen ( fileName , "r" );
    if ( !file ) 
	{
        printf ( "Failed to Open File\n" );
        exit ( EXIT_FAILURE );
    }

    // Read and validate the Mini-ELF header
    if ( !read_header ( file , &hdr ) ) 
	{
        printf ( "Failed to Read ELF Header\n" );
        fclose ( file );
        exit ( EXIT_FAILURE );
    }

    // If -H is provided, dump the header
    if ( header ) 
	{
        dump_header ( hdr );
        printf ( "\n" );
    }

    // Allocate and read program headers
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

    // If -S is provided, dump program headers
    if ( segments ) 
	{
        dump_phdrs ( num_phdrs , phdr );
    }

    // Allocate and initialize memory
    memory = malloc ( MEMSIZE * sizeof ( uint8_t ) );
    if ( !memory ) 
	{
        printf ( "Failed to Allocate Dynamic Memory\n" );
        free ( phdr );
        fclose ( file );
        return EXIT_FAILURE;
    }

    // Load all segments into memory
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
            if ( phdr[i].p_filesz == 0 ) continue;
            uint16_t start = phdr[i].p_vaddr & 0xFFF0;
            uint16_t end = phdr[i].p_vaddr + phdr[i].p_filesz;
            dump_memory ( memory , start , end );
        }
    }

    // Output full memory contents
    if ( memfull ) 
	{
        dump_memory ( memory , 0x0000 , MEMSIZE );
    }

    // Disassemble code segments if -d option is set
    if ( disas_code ) 
	{
        printf ( "Disassembly of executable contents:\n" );
		for ( uint16_t i = 0; i < num_phdrs; i++ ) 
		{
            if ( phdr[i].p_type == CODE ) 
			{
                disassemble_code ( memory , &phdr[i] , &hdr );
				printf ( "\n" );
            }
        }
    }

    // Disassemble data segments if -D option is set
    if ( disas_data ) 
	{
        printf ( "Disassembly of data contents:\n" );
		for ( uint16_t i = 0; i < num_phdrs; i++ ) 
		{
            if ( phdr[i].p_type == DATA ) 
			{
                if ( phdr[i].p_flag == 6 ) 
				{
                    disassemble_data ( memory , &phdr[i] );
                } 
				else if ( phdr[i].p_flag == 4 ) 
				{
                    disassemble_rodata ( memory, &phdr[i] );
                }
				printf ( "\n" );
            }
        }
    }

    // Clean up and exit
    free ( phdr );
    free ( memory );
    fclose ( file );
    return EXIT_SUCCESS;
}

