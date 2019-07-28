#ifndef __IO_C_
#define __IO_C_

/* ---------------------------------------------------------------
Holds the definition of the IO library structures and functions.
---------------------------------------------------------------- */

#include <string.h>
#include <math.h>
#include "IO.h"
#include "Instructions.h"

/* ---------------------------------------------------------------
A macro that writes a formatted external or
entry record to an output stream.
--------------------------------------------------------------- */
#define print_external_entry_record(fp, label, address, buffer) \
	fprintf(fp,"%s %s\n", label, int_to_special_base32(address+MEMORY_OFFSET, buffer, FALSE))

/* ---------------------------------------------------------------
Macros that define the special base 32
--------------------------------------------------------------- */
#define MAX_BASE32_INT_LEN 2 /* 10 bits word takes 2 base 32 characters */
#define OUTPUT_BASE 32
#define MAX_BASE32_VALUE 1023
#define BASE32_STR "!@#$%^&*<>abcdefghijklmnopqrstuv"
#define BASE32_ZERO_VALUE '!'


/* ---------------------------------------------------------------
 Reverses a string
--------------------------------------------------------------- */
void reverse_string(char* str)
{
	char tmp;
	char* end = str + strlen(str) - 1;

	while (str < end)
	{
		tmp = *str;
		*str = *end;
		*end = tmp;

		str++;
		end--;
	}
}

/* ---------------------------------------------------------------
 Converts an integer to the equivalent special base 32 representation
--------------------------------------------------------------- */
char* int_to_special_base32(int value, char buf[MAX_BASE32_INT_LEN + 1], Boolean pad_zeroes)
{
	int i = 0;
	int base = OUTPUT_BASE;
	/* make sure we don't overflow the string */
	value = value & MAX_BASE32_VALUE;

	while (value != 0)
	{
		int rem = value % base;
		buf[i++] = BASE32_STR[rem];
		value = value / base;
	}
	if (pad_zeroes)
	{
		for (; i < MAX_BASE32_INT_LEN; i++)
		{
			buf[i] = BASE32_ZERO_VALUE;
		}
	}
	buf[i] = '\0';
	reverse_string(buf);
	return buf;
}

/* ---------------------------------------------------------------
Prints a number in its binary form.
--------------------------------------------------------------- */
void print_binary(FILE * fp, unsigned n)
{
	unsigned i;
	signed x = 0;

	for (i = 1 << 14; i > 0; i = i / 2) {
		x++;

		(n & i) ? fprintf(fp, "1") : fprintf(fp, "0");

		if (x == 3 || x == 5 || x == 9 || x == 11 || x == 13)
			fprintf(fp, "-");
	}
}
/* ---------------------------------------------------------------
Calculates a label decleration absolute address
into the obejct file.
--------------------------------------------------------------- */
static int label_abs_addr(Label *label, int code_size, int modifier)
{
	int address = 0;
	switch (label->type)
	{
	case LABEL_TYPE_CODE:
		address = label->address + modifier;
		break;
	case LABEL_TYPE_DATA:
		address = label->address + code_size + modifier;
		break;
	case LABEL_TYPE_EXTERNAL:
		address = 0;
		break;
	case LABEL_TYPE_UNKNOWN:
		/* Handle Error */
		break;
	}
	return(address);
}

/* ---------------------------------------------------------------
Opens a file in a given mode. In case that the
function could not open the file, an error
message is printed to the standart error stream.
--------------------------------------------------------------- */
FILE *open_file(char *filename, char *mode)
{
	FILE *fp = NULL;
	if (strcmp(mode, "r") && strcmp(mode, "w"))
	{
		fprintf(stderr, "Invalid open mode for file %s\n", filename);
	}
	else if (!(fp = fopen(filename, mode)))
	{
		fprintf(stderr, "Could not open file %s for %s\n", filename, (strcmp(mode, "r") ? "writing" : "reading"));
	}
	return(fp);
}

/* ---------------------------------------------------------------
Prints the content of an object structure to a given file stream. The content is printed in a structured
manner where each line is of the given format:
"<address>	<code>	<flag>"
--------------------------------------------------------------- */
void write_object(FILE *fp, AssemblerManager *manager)
{
	unsigned int i, lineNumber = 0;
	unsigned short binaryValue;
	short int are_flag;
	Record* currentRecord = NULL;
	Label *label = NULL;
	short toPrint;
	char buffer1[MAX_BASE32_INT_LEN + 1];
	char buffer2[MAX_BASE32_INT_LEN + 1];

	Boolean testMode = FALSE;

	fprintf(fp, HEADER_PRINT_FORMAT,
		int_to_special_base32(manager->IC, buffer1, FALSE),
		int_to_special_base32(manager->DC, buffer2, FALSE));

	/* Write the code section.*/
	for (i = 0; i < manager->IC; i++)
	{
		toPrint = 0;
		currentRecord = manager->code + i;

		switch (currentRecord->type)
		{
			case OR_TYPE_OPCODE: /* Absolute flag */
				binaryValue = currentRecord->data.op_code.value;
				are_flag = ARE_FLAG_A; 
				toPrint = ((binaryValue << ABSOLUTE_SHIFTING) | are_flag) | THREE_ONES_IN_ROW;

				break;
			case OR_TYPE_VALUE: /* Relocatable flag */
			case OR_TYPE_REGISTER_VALUE: /* Absolute flag */
				binaryValue = currentRecord->data.value;

				are_flag = ARE_FLAG_A; 
				toPrint = (binaryValue << ABSOLUTE_SHIFTING) | are_flag;
				break;

			case OR_TYPE_STRUCT_DATA:
				toPrint = (currentRecord->data.value << ABSOLUTE_SHIFTING) | ARE_FLAG_A;
				break;
			case OR_TYPE_LABEL_ABSOLUTE_ADDRESS:  /* Relocatable or External flag */
				label = currentRecord->data.label_ptr;
				binaryValue = label_abs_addr(label, manager->IC, MEMORY_OFFSET); 
				are_flag = (label->type == LABEL_TYPE_EXTERNAL ? ARE_FLAG_E : ARE_FLAG_R);
				
				toPrint = (binaryValue << ABSOLUTE_SHIFTING) | are_flag;

				break;
		}

		fprintf(fp, PRINT_FORMAT,
			int_to_special_base32(lineNumber + MEMORY_OFFSET, buffer1, FALSE),
			int_to_special_base32(toPrint, buffer2, TRUE));
		lineNumber++;

		if (testMode == 1) {
			print_binary(fp, toPrint); /* For testMode */
		}
	}
		
	/* Write the data section.*/
	for (i = 0; i < manager->DC; i++) {
		fprintf(fp, PRINT_FORMAT,
			int_to_special_base32(manager->IC + i + MEMORY_OFFSET, buffer1, FALSE),
			int_to_special_base32(manager->data[i], buffer2, TRUE));

		if (testMode == TRUE) {
			print_binary(fp, toPrint);  /* For testMode */
		}
	}
}

/* ---------------------------------------------------------------
Writes every label which is labeled as an entry
label of a given labels table into an ouput stream.
For each entery label in the table a record of the
following format would be written:
<label name> <label address>
--------------------------------------------------------------- */
void write_entries(FILE *fp, unsigned int code_size, LabelsTable *labels_table)
{
	int adrs = 0;
	Label *currLabel = labels_table->first_label;
	char wordBuffer[MAX_BASE32_INT_LEN + 1];

	while (currLabel)
	{
		/* Write the label name and address to the file if
		the label is an entry.*/
		if (currLabel->is_entry) {
			adrs = label_abs_addr(currLabel, code_size, 0);

			print_external_entry_record(fp, (currLabel->name), adrs, wordBuffer);
		}

		/* Go to the next label.*/
		currLabel = currLabel->next_label;
	}
}

/* ---------------------------------------------------------------
For each occurrence of an external label in a given
object structure the function writes a record
into an output stream. Each record written into
the output stream is of the following format:
<label name> <address of the occurrence of the label>
--------------------------------------------------------------- */
void write_externals(FILE *fp, AssemblerManager *manager)
{
	unsigned int i;
	Record *rec;
	char wordBuffer[MAX_BASE32_INT_LEN + 1];

	/* Run on every record in the code section.*/
	for (i = 0; i < manager->IC; i++)
	{
		/* If the current record is an external label then write
		the label to file stream.*/
		rec = manager->code + i;
		if ((rec->type == OR_TYPE_LABEL_ABSOLUTE_ADDRESS) && (rec->data.label_ptr->type == LABEL_TYPE_EXTERNAL)) {
			print_external_entry_record(fp, (rec->data.label_ptr->name), i, wordBuffer);
		}
	}
}

#endif 

/* __IO_C_ */