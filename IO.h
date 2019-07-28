#ifndef __IO_H_
#define __IO_H_

/* ---------------------------------------------------------------
Holds the definition of the IO library structures and functions.
---------------------------------------------------------------- */

#include <stdio.h>
#include "Object.h"

#define THREE_ONES_IN_ROW (7 << 12);

#define MEMORY_OFFSET 100
#define SOURCE_LINE_MAX_LENGTH 80 /* Maximum length of a line (in chars) in a source file */
#define PRINT_WORD_LENGTH 4
#define ABSOLUTE_SHIFTING 2

#define HEADER_PRINT_FORMAT "%s %s\n"
#define PRINT_FORMAT "%s %s\n"


typedef enum
{
	ARE_FLAG_A, /* Absolute */
	ARE_FLAG_E, /* External */
	ARE_FLAG_R /* Relocatable */
} AerFlagType;

/* -----------------------------------------------------------------
A macro that concats a filename and an extention and writes the result to a buffer.
-------------------------------------------------------------------- */
#define get_file_name(buffer, file_name, ext) \
		{ \
		strcpy(buffer, file_name); \
		strcat(buffer, ext); \
		} 

/* ---------------------------------------------------------------
A macro that reads a line from a file stream into a
given buffer. The macro reads up to
SOURCE_LINE_MAX_LENGTH chars from the stream or
until it gets to a newline char. The macro
inserts a '\0' into the buffer at the end of
the read data.
--------------------------------------------------------------- */
#define read_line(fp, buffer) fgets(buffer, SOURCE_LINE_MAX_LENGTH + 1, fp)

/* ---------------------------------------------------------------
Opens a file in a given mode. In case that the
function could not open the file, an error
message is printed to the standart error stream.
--------------------------------------------------------------- */
FILE *open_file(char *filename, char *mode);

/* ---------------------------------------------------------------
Prints the content of an object structure to a given
file stream. The content is printed in a structured
manner where each line is of the given format:
"<address>	<code>	<flag>"
--------------------------------------------------------------- */
void write_object(FILE *fp, AssemblerManager *obj);

/* ---------------------------------------------------------------
Writes every label which is labeled as an entry
label of a given labels table into an ouput stream.
For each entery label in the table a record of the
following format would be written:
<label name> <label address>
Params:		 fp -		   A file pointer to the output stream.
The stream must be allready openned.
--------------------------------------------------------------- */
void write_entries(FILE *fp, unsigned int code_size, LabelsTable *labels_table);

/* ---------------------------------------------------------------
For each occurrence of an external label in a given
object structure the function writes a record
into an output stream. Each record written into
the output stream is of the following format:
<label name> <address of the occurrence of the label>
--------------------------------------------------------------- */
void write_externals(FILE *fp, AssemblerManager *object);

#endif /* _IO_H_ */