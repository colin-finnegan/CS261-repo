/*
 * CS 261: p0-getopt  
 *            D O    N O T    M O D I F Y    T H I S    F I L E 
 * Name: Mohamed Aboutabl
 */

#include <stdio.h>
#include <stdbool.h>

/*-------------------------------------------------------------------------*/

void usage_p0( char *cmd ) ;

bool parse_command_line_p0( int argc, char **argv, bool *sOpt, unsigned *sVal ,
               bool *nOpt, unsigned *nVal, char **file ) ;

bool read_bytes (FILE *file, unsigned  start, unsigned  count, void* buffer) ;
