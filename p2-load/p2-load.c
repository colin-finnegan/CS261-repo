/*
 * CS 261 PA2: Mini-ELF loader
 *
 * Name: Colin Finnegan
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "y86.h"
#include "p2-load.h"

void usage_p2 ()
{
    printf("Usage: y86 <option(s)> mini-elf-file\n");
    printf(" Options are:\n");
    printf("  -h      Display usage\n");
    printf("  -H      Show the Mini-ELF header\n");
	printf("  -a      Show all with brief memory\n");
    printf("  -f      Show all with full memory\n");
	printf("  -s      Show the program headers\n");
    printf("  -m      Show the memory contents (brief)\n");
	printf("  -M      Show the memory contents (full)\n");
    printf("Options must not be repeated neither explicitly nor implicitly.\n");
}

bool parse_command_line_p2 ( int argc , char **argv ,
        bool *header , bool *segments , bool *membrief , bool *memfull ,
        char **file )
{
    if ( !argv || !header || !segments || !membrief || !memfull || !file )
	{
		return false;
	}
	
	*header = false;
	*segments = false;
	*membrief = false;
	*memfull = false;
	*file = NULL;
	
	opterr = 0;
	const char *optionStr = "HhafsmM" ;
	int opt;
	
	bool h_seen = false;
	bool H_seen = false;
	bool a_seen = false;
	bool f_seen = false;
	bool s_seen = false;
	bool m_seen = false;
	bool M_seen = false;
	
	
	while ( 1 )
	{
		opt = getopt( argc , argv , optionStr ) ;
		if ( opt == -1 )
		{
			break;
		}
		
		switch ( opt )
		{
			case 'h':
				if ( h_seen || H_seen || a_seen || f_seen || s_seen || m_seen || M_seen )
				{
					usage_p2();
					*header = false;
					*segments = false;
					*membrief = false;
					*memfull = false;
					*file = NULL;
					return false;
				}
				h_seen = true;
				usage_p2() ;
				return true;
				
			case 'H':
				if ( h_seen || H_seen || a_seen || f_seen )
				{
					usage_p2();
					*header = false;
					*segments = false;
					*membrief = false;
					*memfull = false;
					*file = NULL;
					return false;
				}
				H_seen = true;
				*header = true;
				break;
			
			//Shortcut for -H -s -m
			case 'a':
				if ( h_seen || H_seen || a_seen || f_seen || s_seen || m_seen || M_seen )
				{
					usage_p2();
					*header = false;
					*segments = false;
					*membrief = false;
					*memfull = false;
					*file = NULL;
					return false;
				}
				a_seen = true;
				*header = true;
				*segments = true;
				*membrief = true;
				break;
			
			//Shortcut for -H -s -M
			case 'f':
				if ( h_seen || H_seen || a_seen || f_seen || s_seen || m_seen || M_seen )
				{
					usage_p2();
					*header = false;
					*segments = false;
					*membrief = false;
					*memfull = false;
					*file = NULL;
					return false;
				}
				f_seen = true;
				*header = true;
				*segments = true;
				*memfull = true;
				break;
				
			case 's':
				if ( h_seen || a_seen || f_seen || s_seen )
				{
					usage_p2();
					*header = false;
					*segments = false;
					*membrief = false;
					*memfull = false;
					*file = NULL;
					return false;
				}
				s_seen = true;
				*segments = true;
				break;
				
			case 'm':
				if ( h_seen || a_seen || f_seen || m_seen || M_seen )
				{
					usage_p2();
					*header = false;
					*segments = false;
					*membrief = false;
					*memfull = false;
					*file = NULL;
					return false;
				}
				m_seen = true;
				*membrief = true;
				break;
				
			case 'M':
				if ( h_seen || a_seen || f_seen || m_seen || M_seen )
				{
					usage_p2();
					*header = false;
					*segments = false;
					*membrief = false;
					*memfull = false;
					*file = NULL;
					return false;
				}
				M_seen = true;
				*memfull = true;
				break;
				
			case '?':
				usage_p2();
					*header = false;
					*segments = false;
					*membrief = false;
					*memfull = false;
					*file = NULL;
					return false;
				
			default:
				usage_p2();
					*header = false;
					*segments = false;
					*membrief = false;
					*memfull = false;
					*file = NULL;
					return false;
		}
	}
	
	if ( optind != argc - 1 )
	{
		usage_p2();
		return false;
	}
	
	*file = argv[optind];
	return true;
}

bool read_phdr ( FILE *file , uint16_t offset , elf_phdr_t *phdr )
{
    if ( !file || !phdr ) return false;
    
    if ( fseek ( file , offset , SEEK_SET ) != 0 ) 
	{
        return false;
    }
    
    if ( fread ( phdr , sizeof ( elf_phdr_t ) , 1 , file ) != 1 ) 
	{
        return false;
    }
    
    /* Validate program header magic number */
    if ( phdr[0].magic != 0xDEADBEEF ) 
	{
        return false;
    }
    
    return true;
}

void dump_phdrs ( uint16_t numphdrs , elf_phdr_t phdr[] )
{
	printf("Segment   Offset    VirtAddr  FileSize  Type      Flag\n");
    
    for ( uint16_t i = 0; i < numphdrs; i++ ) 
	{
        // Print segment number (00-99)
        printf("  %02d      " , i );
        
        // Print numeric fields
        printf ("0x%04x    " , phdr[i].p_offset );
        printf ("0x%04x    " , phdr[i].p_vaddr );
        printf ("0x%04x    " , phdr[i].p_filesz );
        
        // Print type string based on enum
        switch ( phdr[i].p_type ) 
		{
            case DATA:    printf ("DATA    "); break;
            case CODE:    printf ("CODE    "); break;
            case STACK:  printf ("STACK   "); break;
            case HEAP:   printf ("HEAP    "); break;
            default:     printf ("UNKNOWN "); break;
        }
        
        // Print flags based on p_flag
        printf ("  ");
        printf ( ( phdr[i].p_flag & 0x4 ) ? "R" : " ");
        printf ( ( phdr[i].p_flag & 0x2 ) ? "W" : " ");
        printf ( ( phdr[i].p_flag & 0x1 ) ? "X" : " ");
        
        printf ("\n");
    }
	printf ("\n");
}

bool load_segment ( FILE *file, memory_t memory, elf_phdr_t phdr )
{
     if ( !file || !memory ) return false;
    
    // Validate the segment
     if ( phdr.p_vaddr >= MEMSIZE || 
        phdr.p_filesz > MEMSIZE || 
        phdr.p_vaddr + phdr.p_filesz > MEMSIZE ) 
	{
        return false;
    }
    
    // Skip zero-size segments
    if ( phdr.p_filesz == 0 ) 
	{
        return true;
    }
    
    if ( fseek ( file , phdr.p_offset , SEEK_SET ) != 0) 
	{
        return false;
    }
    
    if ( fread ( memory + phdr.p_vaddr , 1 , phdr.p_filesz , file ) != phdr.p_filesz ) 
	{
        return false;
    }
    
    return true;
}

void dump_memory ( memory_t memory , uint16_t start , uint16_t end )
{
	 // Print the header line
    printf ("Contents of memory from %04x to %04x:\n", start , end);
    
    // Align start to 16-byte boundary
    uint16_t current = start & 0xFFF0;
    
    while ( current < end ) 
	{
        // Print address
        printf ("  %04x ", current);
        
        // Print hex bytes (8 bytes, space, then 8 bytes)
        for ( int i = 0; i < 16; i++ ) 
		{
            if ( current + i < start ) 
			{
                printf ("   ");
            }
            else if ( current + i < end ) 
			{
                printf (" %02x", memory[current + i] );
            }
            else 
			{
                printf ("   ");
            }
            
            // Extra space after 8 bytes
            if ( i == 7 ) printf (" ");
        }
        
        printf ("\n");
        current += 16;
    }
	printf ("\n");
}
