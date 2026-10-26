/*
 * CS 261: p0-getopt
 *
 * Name: COLIN FINNEGAN  < or receive no credit for this project >
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

void usage_p0( char *cmd )
{
    printf("Usage: %s <option(s)> fileName\n" , cmd);
    printf(" Options are:\n");
    printf("  -h      display this help screen\n");
    printf("  -s b    start reading at location 'b'. Default b=0\n");
    printf("  -n m    read 'm' bytes. Default m=1\n");
}

/*-------------------------------------------------------------------------*/

bool parse_command_line_p0( int argc, char **argv, bool *sOpt, unsigned *sVal ,
               bool *nOpt, unsigned *nVal, char **file )
{
	//checks if the pointers are NULL.
	if( !argv || !sOpt || !sVal || !nOpt || !nVal || !file )
	{
        return false;
    }

    const int numBase = 10 ; 
    long number ;  
    *sOpt = *nOpt = false ;
	*sVal = 0 ;
	*nVal = 1 ;
    *file  =  NULL ;

    opterr = 0 ;
    char *optionStr = "+s:n:h" ;
    char opt , *end;

    while ( 1 )  
    {
		opt = getopt( argc , argv , optionStr ) ;
		if ( opt == -1 )
		{
			break;
		}
		
        switch (opt) 
        {		
			case 's':   
                end = optarg;
                number = strtol( optarg, &end, numBase );
				if ( end == optarg ) 
				{
					usage_p0( argv[0] );
					return false;
				}
				*sOpt = true ;
				*sVal = (int) number;
                break ;

            case 'n':   
                end = optarg;
                number = strtol( optarg, &end, numBase );
				if ( end == optarg ) 
				{
					usage_p0( argv[0] );
					return false;
				}
				*nOpt = true ;
				*nVal = (int) number;
                break ;
				
			case 'h':   
                usage_p0( argv[0] ) ;
                return false ;
				
            default:
                usage_p0( argv[0] ) ;
                return false ; 
        }
    }
	
	if (optind != argc - 1 )
	{
		usage_p0(argv[0]);
		return false;
	}
	
	*file = argv[optind];
    return true ;
}

/*-------------------------------------------------------------------------*/

bool read_bytes (FILE *file, unsigned  start, unsigned  count, void* buffer)
{

	if ( fseek ( file , start , SEEK_SET ) != 0 )
	{
		return false ;
	}
	
	if ( fread ( buffer , 1 , count , file ) != count )
	{
		return false ;
	}
	
    return true ;
}
