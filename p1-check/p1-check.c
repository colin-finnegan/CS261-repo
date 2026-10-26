/*
 * CS 261 PA1: Mini-ELF header verifier
 *
 * Name: Colin Finnegan
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "p1-check.h"

void usage_p1 ()
{
    printf("Usage: y86 <option(s)> mini-elf-file\n");
    printf(" Options are:\n");
    printf("  -h      Display usage\n");
    printf("  -H      Show the Mini-ELF header\n");
    printf("Options must not be repeated neither explicitly nor implicitly.\n");
}

bool parse_command_line_p1 ( int argc , char **argv , bool *header , char **file )
{
    // Implement this function
	
	if ( !argv || !header || !file )
	{
		return false;
	}
	
	*header = false;
	*file = NULL;
	
	opterr = 0;
	const char *optionStr = "Hh" ;
	int opt;
	
	bool h_seen = false;
	bool H_seen = false;
	
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
				if ( h_seen || H_seen )
				{
					usage_p1();
					return false;
				}
				h_seen = true;
				usage_p1() ;
				*header = false;
				*file = NULL;
				return true;
				
			case 'H':
				if ( h_seen || H_seen )
				{
					usage_p1();
					return false;
				}
				H_seen = true;
				*header = true;
				break;
				
			case '?':
				usage_p1();
				return false;
				
			default:
				usage_p1() ;
						return false;
		}
	}
	
	if (optind != argc - 1 )
	{
		usage_p1();
		return false;
	}
	*file = argv[optind];
	return true;
}

bool read_header (FILE *file, elf_hdr_t *hdr)
{
    if ( !file || !hdr )
	{
		return false;
	}
	
	if ( fread ( hdr , sizeof ( elf_hdr_t ) , 1 , file ) != 1 )
	{
		return false;
	}
	
	if ( hdr->magic != 0x00464c45 )
	{
		return false;
	}
	
	if ( hdr->e_version != 0x0001 )
	{
		return false;
	}
	
    return true;
}

void dump_header (elf_hdr_t hdr)
{
    printf("00000000  ");
    for ( int i = 0; i < 16; i++ ) 
	{
        printf("%02x ", *((unsigned char *)&hdr + i));
        if (i == 7) {
            printf(" ");
        }
    }
    printf("\n");

    // Print the Mini-ELF version
    printf("Mini-ELF version %d\n", hdr.e_version);

    // Print the entry point address
    printf("Entry point 0x%03x\n", hdr.e_entry);

    // Print the program headers information
    printf("There are %d program headers, starting at offset %d (0x%02x)\n",
           hdr.e_num_phdr, hdr.e_phdr_start, hdr.e_phdr_start);

    // Print the symbol table information
    if ( hdr.e_symtab != 0 )
	{
        printf("There is a symbol table starting at offset %d (0x%02x)\n",
               hdr.e_symtab, hdr.e_symtab);
    }
	else 
	{
		printf("There is no symbol table present\n");
	}

    // Print the string table information
    if ( hdr.e_strtab != 0)
	{
        printf("There is a string table starting at offset %d (0x%02x)\n",
               hdr.e_strtab, hdr.e_strtab);
    }
	else
	{
        printf("There is no string table present\n");
    }
}

