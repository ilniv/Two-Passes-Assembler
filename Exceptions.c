#ifndef __EXCEPTIONS_C_
#define __EXCEPTIONS_C_

/* ---------------------------------------------------------------
Holds the definition of the error library structures
and functions.
---------------------------------------------------------------- */

#include "Exceptions.h"
#include <stdio.h>

/* The fprintf tamplate used to print the error to the stderr stream.*/
#define ERROR_TEMPLATE "error %04d: %s at %s line %d\n"

/* The unknown error uses the 0 error code only as a
lookup index into the messages table. 0 "error code" is reserved
for no error at all.*/
#define UNKNOWN_ERROR  0 

/* A private table that maps the error code (the index in the table) to
the an actual error message. */
static char *messages[NUM_OF_ERROR_CODES] = {
	"Unknown Error",
	"Invalid label name",
	"Label name cannot be a keyword",
	"Label name too long",
	"Undefined label",
	"Label already defiend",
	"A label marked as entry cannot be external",
	"Invalid .data decleration argument",
	"Empty .data decleration",
	"Invalid .string decleration argument",
	"Empty .string decleration",
	"Empty .entery decleration",
	"Empty .external decleration",
	"Missing instruction or decleration",
	"Unknown instruction or decleration",
	"Missing instruction argument",
	"Invalid instruction argument",
	"Invalid instruction argument addressing type",
	"Too many args for given instruction",
	"Invalid .struct decleration"
};

/* ---------------------------------------------------------------
Prints a formatted error for a given error code
to the standart error stream.
--------------------------------------------------------------- */
void print_error(ErrorCode ERROR_code, char *filename, int line)
{
	/* Check that the error code is valid.*/
	if ((ERROR_code < 0) || (ERROR_code >= NUM_OF_ERROR_CODES))
		ERROR_code = UNKNOWN_ERROR;

	/* Print the error to the standart error stream.*/
	fprintf(stderr, ERROR_TEMPLATE, ERROR_code, messages[ERROR_code], filename, line);
}


#endif /*  __EXCEPTIONS_C_  */