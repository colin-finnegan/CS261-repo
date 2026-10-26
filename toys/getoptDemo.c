/*
 * CS 261 Example
 *
 * Demonstrates how getopt() works to parse command-line options.
 */

//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

void usage ()
{
    printf("\n\n");
    printf("Usage: ./command [options] <inputFileName>\n");
    printf(" Valid options are (in any order/combination):\n");
    printf("  -a \n");
    printf("  -b \n");
    printf("  -n <decimal integer> \n");
    printf("  -f <outputFileName> \n");
}

bool parseCL( int argc, char **argv, 
              bool *optionA, bool *optionN, int *valN, 
              bool *optionB, bool *optionF, char **fVal , char **inFile )
{
    if( !argv )
        return false;
    // should also check the other pointers are not NULL

    printf("\nCommand Line Arguments (%d) BEFORE Parsing:\n" , argc );
    for ( int i=0 ; i<argc ; i++)
        printf(" argv[%2d] = \"%s\" \n" , i , argv[i] ) ;    
    printf("\n\n") ;
  

    const int numBase = 10 ; 
    long number ;  
    *optionA = *optionN = *optionF = *optionB = false ;
    *inFile  =  NULL ;

    opterr = 0 ;           /* Prevent getopt() from printing error messages */
    char *optionStr = "an:f:b" ; /* A leading '+' prevents reordering of argv */
    char opt , *end;

    while ( 1 )  
    {
        printf("Processing argv[%2d] = \"%-20s\"  ....  " , optind , argv[optind] ) ;
        opt = getopt( argc, argv, optionStr ) ;
        if ( opt == -1 )
        {
            printf(" opt = %d " , opt ) ;
            printf("getopt() is DONE\n\n" ) ;
            break ;
        }

        printf(" opt = '%c' " , opt ) ;
        switch (opt) 
        {
            case 'a':   
                *optionA = true; 
                break ;

            case 'b':   
                *optionB = true; 
                break ;
                        
            case 'n':   
                end = optarg;
                number = strtol( optarg, &end, numBase );
                if ( end == optarg )   {  
                    // 'optarg' string does not represent an integer
                    usage() ;
                    return false ;
                }
                *optionN = true;
                *valN    = (int) number ;
                printf( "optarg = \"%s\" converted to %ld" , optarg , number ) ;
                break;

            case 'f':   
                *optionF = true;
                *fVal = optarg ;
                printf( "optarg = \"%s\" " , optarg ) ;
                break;

            default:    
                printf(" INVALID option = '%c' " , optopt ) ;
                usage() ;   // invalid options found
                return false ; 
        }
        printf( "\n" ) ;
    }


    printf("\nCommand Line Arguments (%d) AFTER Parsing:\n" , argc );
    for ( int i=0 ; i<argc ; i++)
        printf(" argv[%2d] = \"%s\" \n" , i , argv[i] ) ;    
    printf("\n\n") ;

    printf( "getopt() Stopped at argv[%2d] \"%s\" .... " , optind , argv[optind] ) ;
    *inFile  =  argv[optind] ;

    if( optind != argc  -1 )
        printf(" There is  more than one non-option argument");

    printf("\n\n");
    return true;
}

int main (int argc, char **argv)
{
    bool opA, opN, opB , opF;
    int nVal ;
    char *inFileName , *fVal ;

    fVal = "a.out" ;  // default output file name 
    
    // Call the C.L. parsing function
    if ( ! parseCL( argc , argv , &opA , &opN , &nVal , &opB 
                    , &opF , &fVal , &inFileName ) ) 
    {
        printf("The parseCL function encountered some problem(s)\n");
        exit( EXIT_FAILURE );
    }

    printf("The following options were present on the C.L. :\n") ;
    // Now, respond to each of the options you received
    if( opA ) printf("\t '-a'\n") ;
    
    if( opB ) printf("\t '-b'\n") ;
    
    if( opN ) printf("\t '-n' with option argument = %d\n" , nVal ) ;
    
    if( opF ) printf("\t '-f' with option argument \"%s\" \n" , fVal ) ;
 
    printf("\n\n") ;
    printf("input  File name: \"%s\" \n", inFileName );
    printf("output File name: \"%s\" \n\n", fVal );
    
   
    return EXIT_SUCCESS;
}

