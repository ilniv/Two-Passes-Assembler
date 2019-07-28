#ifndef __EXCPTIONS_H_
#define __EXCPTIONS_H_

/* ---------------------------------------------------------------
Holds the definition of the error library structures
and functions.
---------------------------------------------------------------- */

/* ---------------------------------------------------------------
Enum of handled error codes.
The NUM_OF_ERROR_CODES value is not an error code
but an	indication to the number of error codes
defined.
--------------------------------------------------------------- */
typedef enum error_codes_enum
{
	VALID = 0,
	ERROR_INVALID_LABEL_NAME,
	ERROR_KEYWORD_AS_LABEL_NAME,
	ERROR_LABEL_NAME_TOO_LONG,
	ERROR_UNDEFINED_LABEL,
	ERROR_DUPLICATE_LABEL_DEFINITION,
	ERROR_ENTRY_LABEL_IS_EXTERNAL,
	ERROR_INVALID_DATA_DEC_ARG,
	ERROR_EMPTY_DATA_DEC,
	ERROR_INVALID_STRING_DEC_ARG,
	ERROR_EMPTY_STRING_DEC,
	ERROR_EMPTY_ENTRY_DEC,
	ERROR_EMPTY_EXTERNAL_DEC,
	ERROR_MISSING_MAIN_TOKEN,
	ERROR_UNKNOWN_MAIN_TOKEN,
	ERROR_MISSING_INST_ARG,
	ERROR_INVALID_INST_ARG,
	ERROR_INVALID_INST_ARG_ADRR_TYPE,
	ERROR_INST_TOO_MANY_ARGS,
	ERROR_INVALID_STRUCT_DEC,

	NUM_OF_ERROR_CODES
} ErrorCode;

/* ---------------------------------------------------------------
Prints a formatted error for a given error code
to the standart error stream.
--------------------------------------------------------------- */
void print_error(ErrorCode ERROR_code, char *filename, int line);

#endif /*  __EXCPTIONS_H_ */