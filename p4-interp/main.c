/*
 * CS 261: P4 Mini-ELF Interpreter Main driver
 *
 * Name: Colin Finnegan
 *    or risk losing points, and delays when you seek my help during office hours 
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "p1-check.h"
#include "p2-load.h"
#include "p3-disas.h"
#include "p4-interp.h"
#include "y86.h"
#include "elf.h"

int main(int argc, char **argv)
{
    bool header = false, segments = false, membrief = false, memfull = false;
    bool disas_code = false, disas_data = false;
    bool exec_normal = false, exec_debug = false;
    char *fileName = NULL;
    FILE *file = NULL;
    elf_hdr_t hdr;
    elf_phdr_t *phdr = NULL;
    memory_t memory = NULL;

    // Parse command line
    if ( !parse_command_line_p4 ( argc , argv , &header , &segments , &membrief ,
                             &memfull , &disas_code , &disas_data , &exec_normal ,
                             &exec_debug, &fileName ) ) 
	{
        exit ( EXIT_FAILURE );
    }

    if ( fileName == NULL ) 
	{
        exit ( EXIT_SUCCESS );
    }

    // File operations
    if ( ! ( file = fopen ( fileName , "r" ) ) ) 
	{
        printf ( "Failed to open File\n" );
        exit ( EXIT_FAILURE );
    }

    if ( !read_header ( file , &hdr ) ) 
	{
        printf ( "Failed to Read ELF Header\n" );
        fclose ( file );
        exit ( EXIT_FAILURE );
    }

    // Header display
    if ( header ) 
	{
        dump_header ( hdr );
        printf ( "\n" );
    }

    // Load program headers
    phdr = malloc ( hdr.e_num_phdr * sizeof ( elf_phdr_t ) );
    if ( !phdr ) 
	{
        printf ( "Failed to Allocate Dynamic Memory\n" );
        fclose ( file );
        exit ( EXIT_FAILURE );
    }

    for ( uint16_t i = 0; i < hdr.e_num_phdr; i++ ) 
	{
        uint16_t offset = hdr.e_phdr_start + i * sizeof ( elf_phdr_t );
        if ( !read_phdr ( file , offset , &phdr [ i ] ) ) 
		{
            printf ( "Failed to Read Program Header\n" );
            free ( phdr );
            fclose ( file );
            exit ( EXIT_FAILURE );
        }
    }

    if ( segments ) 
	{
        dump_phdrs ( hdr.e_num_phdr , phdr );
    }

    // Memory operations - with initialization
    memory = malloc ( MEMSIZE * sizeof ( uint8_t ) ); // Zero-initialized
	memset(memory, 0, MEMSIZE * sizeof(uint8_t));
    if ( !memory ) 
	{
        printf ( "Failed to Allocate Dynamic Memory\n" );
        free ( phdr );
        fclose ( file );
        return EXIT_FAILURE;
    }

    for ( uint16_t i = 0; i < hdr.e_num_phdr; i++ ) 
	{
        if ( !load_segment ( file , memory , phdr [ i ] ) ) 
		{
            printf ( "Failed to Load Segment\n" );
            free ( phdr );
            free ( memory );
            fclose ( file );
            return EXIT_FAILURE;
        }
    }

    // Display options
    if ( membrief ) 
	{
        for ( uint16_t i = 0; i < hdr.e_num_phdr; i++ ) 
		{
            if ( phdr [ i ].p_filesz == 0) continue;
            uint16_t start = phdr [ i ].p_vaddr & 0xFFF0;
            uint16_t end = phdr [ i ].p_vaddr + phdr [ i ].p_filesz;
            dump_memory ( memory , start , end );
        }
    }

    if ( memfull ) 
	{
        dump_memory ( memory , 0x0000 , MEMSIZE );
    }

    if ( disas_code ) 
	{
        printf ( "Disassembly of executable contents:\n" );
        for ( uint16_t i = 0; i < hdr.e_num_phdr; i++ ) 
		{
            if ( phdr[ i ].p_type == CODE ) 
			{
                disassemble_code ( memory , &phdr [ i ] , &hdr );
                printf ( "\n" );
            }
        }
    }

    if ( disas_data ) 
	{
        printf ( "Disassembly of data contents:\n" );
        for ( uint16_t i = 0; i < hdr.e_num_phdr; i++ ) 
		{
            if ( phdr[ i ].p_type == DATA ) 
			{
                if ( phdr[ i ].p_flag == 6 ) 
				{
                    disassemble_data ( memory , &phdr[ i ] );
                } else if ( phdr[ i ].p_flag == 4 ) 
				{
                    disassemble_rodata ( memory , &phdr[ i ] );
                }
                printf ( "\n" );
            }
        }
    }

    // Execution engine
    if ( exec_normal || exec_debug ) 
	{
        y86_t cpu = 
		{
            .rax = 0, .rcx = 0, .rdx = 0, .rbx = 0,
            .rsp = 0, .rbp = 0, .rsi = 0, .rdi = 0,
            .r8 = 0, .r9 = 0, .r10 = 0, .r11 = 0,
            .r12 = 0, .r13 = 0, .r14 = 0,
            .zf = false, .sf = false, .of = false,
            .pc = hdr.e_entry,
            .stat = AOK
        };
        
        long count = 0;
        
        printf ( "Entry execution point at 0x%04lx\n" , cpu.pc );
        printf ( "Initial " );
        dump_cpu ( &cpu );

        // Main execution loop
        while ( cpu.stat == AOK ) 
		{
            // 1. FETCH
            y86_inst_t ins = fetch ( &cpu , memory );
            count++;
            
			
            // 2. Handle invalid instructions
            if ( ins.type == INVALID ) 
			{
                if ( exec_normal ) 
				{
                    printf ( "Corrupt Instruction (opcode 0x%02x) at address 0x%04lx\n" ,
                          memory [ cpu.pc ], cpu.pc );
					printf ( "Post-Fetch " );  // Changed from Post-Exec
					dump_cpu ( &cpu );
                }
                cpu.stat = INS;
                continue;
            }

			if (exec_debug) 
			{
                printf ( "Executing: " );
                disassemble ( ins );
                printf ( "\n" );
            }
            // 3. DECODE/EXECUTE
            bool cond = false;
            y86_register_t valA = 0;
            y86_register_t valE = decode_execute ( &cpu , &cond , &ins , &valA );
           
            // 4. MEMORY/WRITEBACK/PC UPDATE
            memory_wb_pc ( &cpu , memory , cond , &ins , valE , valA );
			
			
            
            // Debug output
            if ( exec_debug ) 
			{
                printf ( "Post-Exec " );
                dump_cpu ( &cpu );
            }
			
            
        }

        // Final output
        if ( exec_normal && cpu.stat != INS ) 
		{
            printf ( "Post-Exec " );
            dump_cpu ( &cpu );
        }
        printf ( "Total execution count: %ld instructions\n\n" , count );
         
        if ( exec_debug ) 
		{
            dump_memory ( memory , 0x0000 , 0x1000 );
        }
    }

    // Cleanup
    free ( phdr );
    free ( memory );
    fclose ( file );
    return EXIT_SUCCESS;
}