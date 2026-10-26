/*
 * CS 261: p0-getopt
 *
 * Name: PUT YOUR FULL NAME HERE  < or receive no credit for this project >
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

    // implement this function
    
    return true;
}

/*-------------------------------------------------------------------------*/

bool read_bytes (FILE *file, unsigned  start, unsigned  count, void* buffer)
{

    // implement this function
    
    return true;
}
