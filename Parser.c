#ifndef __PARSER_C_
#define __PARSER_C_

/* ---------------------------------------------------------------
Holds the definition of the parser library
structures and functions.
---------------------------------------------------------------- */

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "Parser.h"
#include "Exceptions.h"
#include "Instructions.h"

/*---------------------------------------------------------------
 A macro that checks if the character is space or tab
---------------------------------------------------------------*/
#define isblank(c) ((c)==' ' || (c)=='\t')

/*---------------------------------------------------------------
Remove white space for the string value.
---------------------------------------------------------------*/
static char* trim(char *input)
{
	char *start = NULL;
	size_t len;

	while (isspace(*input))
		input++;

	start = input;
	if (*start)
	{
		len = strlen(start);
		input = start + len - 1;
		while (isspace(*input))
			input--;

		*(++input) = '\0';
	}

	return start;
}

/*---------------------------------------------------------------
Split the line to 3 sections which contain label, token and the arguments
in case exist.
---------------------------------------------------------------*/
static void initLine(char *line, char** label, char** token, char** args)
{
	char *temp;
	
	/* Check for label */
	if ((temp = strchr(line, ':')))
	{
		*(temp++) = '\0';
		*label = trim(line);
		line = temp;
	}
	else
		*label = "";

	/* removing white spaces*/
	while (isspace(*line))
		line++;


	if ((temp = strpbrk(line, " \t\n")))
	{
		*(temp++) = '\0';
		*token = trim(line);
		*args = trim(temp);
	}
	else
	{
		*token = line;
		*args = "";
	}
}

/*---------------------------------------------------------------
Indicates if the label name is valid
---------------------------------------------------------------*/
static ErrorCode validateLabelName(char *label)
{
	ErrorCode ERROR_code = VALID;

	int i = 0;

	if (isalpha(label[i++]))
	{
		while (isalnum(label[i]))
			i++;

		if (label[i] != '\0')
			ERROR_code = ERROR_INVALID_LABEL_NAME;
		else if (get_keyword(label, KW_TYPE_ALL))
			ERROR_code = ERROR_KEYWORD_AS_LABEL_NAME;
		else if (i > MAX_LABEL_SIZE)
			ERROR_code = ERROR_LABEL_NAME_TOO_LONG;
	}
	else
		ERROR_code = ERROR_INVALID_LABEL_NAME;

	return ERROR_code;
}

/*---------------------------------------------------------------
Parses the arguments of the data declaration.
---------------------------------------------------------------*/
static ErrorCode parseDataArguments(char *args, AssemblerManager *manager)
{
	char *temp, *arg;
	unsigned int count = 0;
	ErrorCode errResult = VALID;

	/* Parse each argument that ends with a ','.*/
	while (errResult == VALID &&
		(temp = strchr(args, ',')) != NULL)
	{
		*(temp++) = '\0';
		arg = trim(args);
		args = temp;

		manager->data[manager->DC + count] = (short int)strtol(arg, &temp, 10);
		if (*temp || (arg == temp))
			errResult = ERROR_INVALID_DATA_DEC_ARG;
		else
			count++;
	}

	if (errResult == VALID)
	{
		/* Parse the last argument in the list.*/
		arg = trim(args);
		manager->data[manager->DC + count] = (short int)strtol(arg, &temp, 10);

		if (*temp)
			errResult = ERROR_INVALID_DATA_DEC_ARG;
		else if (arg == temp)
		{
			if (count)
				errResult = ERROR_INVALID_DATA_DEC_ARG;
			else
				errResult = ERROR_EMPTY_DATA_DEC;
		}
		else
			count++;

		if (errResult == VALID)
			manager->DC += count;
	}

	return errResult;
}

/*---------------------------------------------------------------
Parses the arguments of the struct declaration.
---------------------------------------------------------------*/
static ErrorCode parse_struct_declaration_arguments(char *args, AssemblerManager *manager)
{
	unsigned int dcChange = 0;
	size_t len, i;
	char* temp;

	args = trim(args);
	manager->data[manager->DC] = (short int)strtol(args, &temp, 10);
	if (*temp == '\0' || args == temp)
		return ERROR_INVALID_STRUCT_DEC;
	dcChange++;
	args = temp;
	args = trim(args);

	if (*args != ',')
		return ERROR_INVALID_STRUCT_DEC;
	args++;
	args = trim(args);
	len = strlen(args);
	if ((args[0] != '\"') || (args[len - 1] != '\"'))
		return ERROR_INVALID_STRUCT_DEC;
	args++;
	for (i = 0; i < len - 2; i++)
	{
		if (args[i] == '\"')
			return ERROR_INVALID_STRUCT_DEC;
		manager->data[manager->DC + dcChange] = args[i];
		dcChange++;
	}

	manager->data[manager->DC + dcChange] = '\0';
	dcChange++;
	manager->DC += dcChange;
	
	return VALID;
}

/*---------------------------------------------------------------
Parses the arguments of a string declaration.
---------------------------------------------------------------*/
static ErrorCode parse_string_declaration_arguments(char *args, AssemblerManager *manager)
{
	unsigned int i = 0;
	ErrorCode ERROR_code = VALID;
	size_t len;

	if (*args)
	{
		len = strlen(args);

		if ((args[0] == '\"') && (args[len - 1] == '\"'))
		{
			args++;
			for (i = 0; !ERROR_code && (i < len - 2); i++)
			{
				if (args[i] != '\"')
					manager->data[manager->DC + i] = args[i];
				else
					ERROR_code = ERROR_INVALID_STRING_DEC_ARG;
			}

			if (!ERROR_code)
			{
				manager->data[manager->DC + i] = '\0';
				manager->DC += i + 1;
			}
		}
		else
			ERROR_code = ERROR_INVALID_STRING_DEC_ARG;
	}
	else
		ERROR_code = ERROR_EMPTY_STRING_DEC;

	return ERROR_code;
}

/*---------------------------------------------------------------
Parses the arguments of an external declaration.
---------------------------------------------------------------*/
static ErrorCode parse_external_declaration_arguments(
	char *args, 
	AssemblerManager *manager,
	LabelsTable *labels)
{
	ErrorCode ERROR_code = VALID;
	Label *label;

	if (!*args)
		ERROR_code = ERROR_EMPTY_EXTERNAL_DEC;
	else if ((ERROR_code = validateLabelName(args)) == VALID)
	{
		label = findLabel(labels, args);
		label->type = LABEL_TYPE_EXTERNAL;
		label->address = 0;
		manager->has_externals = TRUE;
	}

	return ERROR_code;
}

/*---------------------------------------------------------------
Parses the arguments of an entry declaration.
---------------------------------------------------------------*/
static ErrorCode parse_entry_declaration_arguments(
	char *args, 
	AssemblerManager *manager,
	LabelsTable *labels)
{
	ErrorCode ERROR_code = VALID;
	Label *label;

	if ((*args) &&
		(ERROR_code = validateLabelName(args)) == VALID)
	{
		label = findLabel(labels, args);
		label->is_entry = TRUE;
		manager->has_entries = TRUE;
	}
	else
		ERROR_code = ERROR_EMPTY_ENTRY_DEC;

	return(ERROR_code);
}

/*---------------------------------------------------------------
Parses a declaration.
---------------------------------------------------------------*/
static ErrorCode parse_decleration(
	Keyword *kw, 
	char *args, 
	AssemblerManager *manager,
	LabelsTable *labels)
{
	ErrorCode ERROR_code;

	switch (kw->id)
	{
	case DEC_TYPE_DATA:
		ERROR_code = parseDataArguments(args, manager);
		break;
	case DEC_TYPE_ENTRY:
		ERROR_code = parse_entry_declaration_arguments(args, manager, labels);
		break;
	case DEC_TYPE_EXTERN:
		ERROR_code = parse_external_declaration_arguments(args, manager, labels);
		break;
	case DEC_TYPE_STRING:
		ERROR_code = parse_string_declaration_arguments(args, manager);
		break;
	case DEC_TYPE_STRUCT:
		ERROR_code = parse_struct_declaration_arguments(args, manager);
		break;
	}
	return(ERROR_code);
}

/*---------------------------------------------------------------
Deep copy string from src to dest.
---------------------------------------------------------------*/
static void copyString(char* src, char* dest)
{
	int i = 0;

	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}

	dest[i] = '\0';
}

/*---------------------------------------------------------------
Splits instructions arguments
---------------------------------------------------------------*/
static void splitInstructionArgs(char *args, char** first_arg, char** second_arg)
{
	*first_arg = args;
	if ((args = strchr(args, ',')))
	{
		*(args++) = '\0';
		*second_arg = trim(args);
	}

	else
		*second_arg = "";

	*first_arg = trim(*first_arg);
}

/*---------------------------------------------------------------
Analyzes instructions arguments
---------------------------------------------------------------*/
static ErrorCode analyzeInstructionArgs(
	char *arg, 
	AddressType* addr_type, 
	char** label_name,
	short int* value)
{
	Keyword *kw = NULL;
	ErrorCode error_code = VALID;
	char* field_start;

	/* argument is struct addressing */
	if ((field_start = strchr(arg, '.')) != NULL)
	{
		arg = trim(arg);
		*field_start = '\0';
		field_start++;
		copyString(arg, *label_name);
		error_code = validateLabelName(*label_name);
		if (error_code != VALID)
			return error_code;

		field_start = trim(field_start);
		if (strlen(field_start) != 1 || (*field_start != '1' && *field_start != '2'))
			return ERROR_INVALID_INST_ARG;

		*addr_type = ADDR_TYPE_STRUCT;
		*value = *field_start - '0';
		return VALID;
		
	}

	/* Argument is in immediate addressing.*/
	if (*arg == IMMEDIATE_CHAR)
	{
		/* Ignore the '#'*/
		arg++;
		*addr_type = ADDR_TYPE_IMMEDIATE;
		*value = (short)strtol(arg, &arg, 10);

		/* If there are any chars after the number the argument
		is invalid.*/
		if (*arg)
		{
			error_code = ERROR_INVALID_INST_ARG;
		}
	}
	/* Argument is in register addressing.*/
	else if ((kw = get_keyword(arg, KW_TYPE_REGISTER)))
	{
		*value = kw->id;
		*addr_type = ADDR_TYPE_REGISTER;
	}
	/* Argument is in direct addressing.*/
	else if (!validateLabelName(arg))
	{
		*addr_type = ADDR_TYPE_DIRECT;
		*label_name = arg;
	}
	/* Argument is invalid */
	else
	{
		error_code = ERROR_INVALID_INST_ARG;
	}

	return error_code;
}

static ErrorCode parseInstructionArgs(
	char *arg, 
	AddressType inst_addr_mask, 
	AddressType* addr_type, 
	int *reg_code,
	Record *value_rec,
	Record *extra_struct_record,
	Boolean* is_reg_code_used,
	LabelsTable *labels)
{
	char label_name[MAX_LABEL_SIZE + 1];
	char* label_name_pointer = label_name;
	short int value;
	ErrorCode ERROR_code = VALID;

	if (arg == NULL)
	{
		ERROR_code = ERROR_MISSING_INST_ARG;
	}
	else
	{
		ERROR_code = analyzeInstructionArgs(arg, addr_type, &label_name_pointer, &value);

		if (ERROR_code == VALID)
		{
			if (!(inst_addr_mask & (*addr_type)))
			{
				ERROR_code = ERROR_INVALID_INST_ARG_ADRR_TYPE;
			}
			else
			{
				*is_reg_code_used = FALSE;
				*reg_code = 0;

				switch (*addr_type)
				{
				case ADDR_TYPE_IMMEDIATE:
				{
					value_rec->type = OR_TYPE_VALUE;
					value_rec->data.value = value;
					break;
				}
				case ADDR_TYPE_DIRECT:
				{
					value_rec->type = OR_TYPE_LABEL_ABSOLUTE_ADDRESS;
					value_rec->data.label_ptr = findLabel(labels, label_name_pointer);

					break;
				}
				case ADDR_TYPE_REGISTER:
				{
					*is_reg_code_used = TRUE;
					*reg_code = value;
					break;
				}
				case ADDR_TYPE_STRUCT:
				{
					value_rec->type = OR_TYPE_LABEL_ABSOLUTE_ADDRESS;
					value_rec->data.label_ptr = findLabel(labels, label_name_pointer);

					extra_struct_record->type = OR_TYPE_STRUCT_DATA;
					extra_struct_record->data.value = value;
					break;
				}
				case ADDR_TYPE_NONE:
				case ADDR_TYPE_ALL:
					break;
				}
			}
		}
	}

	return ERROR_code;
}

static int calculateAddressType(AddressType addr_type)
{
	static int lookup[][2] = {
		{ ADDR_TYPE_IMMEDIATE, 0 },
		{ ADDR_TYPE_DIRECT, 1 },
		{ ADDR_TYPE_STRUCT, 2 },
		{ ADDR_TYPE_REGISTER, 3 }
	};

	int i;
	/* Search the lookup table for the proper addressing type.*/
	for (i = 0; lookup[i][0] != addr_type; i++);

	/*return the addressing type value.*/
	return(lookup[i][1]);
}

static ErrorCode parseInstruction(
	Keyword * kw, 
	char * args_str, 
	AssemblerManager * manager, 
	LabelsTable * labels, 
	unsigned char line_num)
{
	ErrorCode errResult = VALID;
	InstructionAddressRecord *addressing = get_instruction_addressing(kw->id);

	char temp_String[MAX_LABEL_SIZE + 1];
	char another_Temp[MAX_LABEL_SIZE + 1];

	Record * regRecord = NULL; /* Used to store the register record value, if used */
	Record extraStructRecord;
	Record * rec = &(manager->code[manager->IC]);

	OpCode* op_code = &(rec->data.op_code);
	char *args[3] = { "", "", "" };
	unsigned int counter = 0;
	int src_reg_code = 0, dst_reg_code = 0;
	AddressType addr_type;
	Boolean is_reg_code_used = FALSE;
	unsigned int inst_addr = manager->IC;

	/* Set the op_code record.*/
	rec->type = OR_TYPE_OPCODE;
	rec->line = line_num;
	rec->inst_addr = inst_addr;
	op_code->fields.group = 0;

	copyString(args_str, temp_String);

	/* Split the args string.*/
	splitInstructionArgs(temp_String, &args[0], &args[1]);

	/* Initialize the opcode and set the instruction code.*/
	init_opcode(op_code);
	op_code->fields.inst_code = kw->id;

	/* Advance to the next object record.*/
	rec++;
	manager->IC++;

	/* Parsing the source argument.*/
	if (addressing->src_addr_type_mask != ADDR_TYPE_NONE)
	{
		rec->inst_addr = inst_addr;
		rec->line = line_num;
		op_code->fields.group++;

		copyString(args[counter], another_Temp);
		if ((errResult = parseInstructionArgs(another_Temp, addressing->src_addr_type_mask, &addr_type,
			&src_reg_code, rec, &extraStructRecord, &is_reg_code_used, labels)) == VALID)
		{
			op_code->fields.src_addr_type_value = calculateAddressType(addr_type);

			counter++;

			/* Check if the record was used.*/
			if (is_reg_code_used)
			{
				regRecord = rec;
			}

			/* Advance to the next object record.*/
			rec++;
			manager->IC++;

			if (addr_type == ADDR_TYPE_STRUCT)
			{
				extraStructRecord.line = line_num;
				*rec = extraStructRecord;
				rec++;
				manager->IC++;
			}
		}
	}

	/* Fill data for the first extra record if required*/
	if (regRecord)
	{
		if (op_code->fields.src_addr_type_value == 3) {
			regRecord->type = OR_TYPE_REGISTER_VALUE;
			regRecord->data.value = (src_reg_code << PRINT_WORD_LENGTH);
		}
	}

	/* Parsing the dest argument.*/
	if (errResult == VALID && (addressing->destAddrTypeMask != ADDR_TYPE_NONE))
	{
		rec->inst_addr = inst_addr;
		rec->line = line_num;
		op_code->fields.group++;

		if ((errResult = parseInstructionArgs(args[counter], addressing->destAddrTypeMask, &addr_type,
			&dst_reg_code, rec, &extraStructRecord, &is_reg_code_used, labels)) == VALID)
		{
			op_code->fields.dest_addr_type_value = calculateAddressType(addr_type);

			counter++;

			/* Checks if the record was used. */
			if (is_reg_code_used)
			{
				if (regRecord == NULL || op_code->fields.src_addr_type_value == 2)
				{
					regRecord = rec;
					manager->IC++;
				}
			}
			else
			{
				manager->IC++;
			}

			if (addr_type == ADDR_TYPE_STRUCT)
			{
				extraStructRecord.line = line_num;
				*rec = extraStructRecord;
				rec++;
				manager->IC++;
			}
		}
	}

	if (errResult == VALID && (*(args[counter])))
		errResult = ERROR_INST_TOO_MANY_ARGS;

	if (regRecord)
	{
		if (op_code->fields.dest_addr_type_value == 3) {
			if (op_code->fields.src_addr_type_value != 3) {
				rec++;
			}
			regRecord->type = OR_TYPE_REGISTER_VALUE;
			regRecord->data.value = ((src_reg_code << PRINT_WORD_LENGTH) | dst_reg_code);
		}

	}

	return errResult;
}

static ErrorCode insertLabel(char *label_name, unsigned short address, LabelType type, LabelsTable *labels)
{
	Label *label;
	ErrorCode errResult = VALID;

	/* Checks that it's a valid label. */
	if (!(errResult = validateLabelName(label_name)))
	{
		label = findLabel(labels, label_name);

		/* Checks that the label wasn't already defined.*/
		if (label->type == LABEL_TYPE_UNKNOWN)
		{
			label->address = address;
			label->type = type;
		}
		else
			errResult = ERROR_DUPLICATE_LABEL_DEFINITION;
	}

	return errResult;
}

static ErrorCode parseLine(
	char * line, 
	unsigned int line_num, 
	AssemblerManager * manager, 
	LabelsTable *labels)
{
	ErrorCode errResult = VALID;
	char *label_name, *token, *args;
	Keyword *kw;
	unsigned short int addr_IC, addr_DC;

	line = trim(line);

	if (*line && (*line != COMMENT_CHAR))
	{
		/* get the 3 sections of the code line*/
		initLine(line, &label_name, &token, &args);

		/* check if token exists */
		if (!(*token))
			errResult = ERROR_MISSING_MAIN_TOKEN;
		else
		{
			addr_IC = manager->IC;
			addr_DC = manager->DC;

			/* check if the token is valid.*/
			if ((kw = get_keyword(token, KW_TYPE_INSTRUCTION)))
			{
				/* Parse the instruction.*/
				errResult = parseInstruction(kw, args, manager, labels, line_num);

				/* Insert a label if neccecery.*/
				if (errResult == VALID && (*label_name))
					errResult = insertLabel(label_name, addr_IC, LABEL_TYPE_CODE, labels);
			}
			/* Declaration.*/
			else if ((kw = get_keyword(token, KW_TYPE_DECLARATION)))
			{
				/* Parse the decleration.*/
				errResult = parse_decleration(kw, args, manager, labels);

				/* Insert a label if neccecery.*/
				if (errResult == VALID && (*label_name))
					errResult = insertLabel(label_name, addr_DC, LABEL_TYPE_DATA, labels);
			}
			/* Main token is not a known keyword.*/
			else
				errResult = ERROR_UNKNOWN_MAIN_TOKEN;
		}
	}

	return(errResult);
}

Boolean firstScan(
	char *filename, 
	FILE *fp, 
	AssemblerManager *manager, 
	LabelsTable *labels)
{
	unsigned int line_num = 1;
	char lineBuffer[SOURCE_LINE_MAX_LENGTH];
	ErrorCode errResult;
	Boolean no_errors = TRUE;

	/* Read each line of the source code and try to parse it.*/
	while (read_line(fp, lineBuffer))
	{
		errResult = parseLine(lineBuffer, line_num, manager, labels);
		if (errResult != VALID)
		{
			/* Found an error - print it.*/
			print_error(errResult, filename, line_num);
			no_errors = FALSE;
		}

		line_num++;
	}

	return no_errors;
}

Boolean secondScan(
	char *filename, 
	AssemblerManager *manager)
{
	unsigned int i;
	Boolean no_errors = TRUE;
	Record *record;

	/* Scan each line in the object struct that was filled in the first pass
	validating that each label has a valid address.*/
	for (i = 0; i < manager->IC; i++)
	{
		/* Point to the proper record in the object code array.*/
		record = manager->code + i;

		/* Check that the record is a label record and that the address is invalid.*/
		if ((record->type == OR_TYPE_LABEL_ABSOLUTE_ADDRESS) &&
			(record->data.label_ptr->address == LABEL_INVALID_ADDRESS))
		{
			no_errors = FALSE;
			print_error(ERROR_UNDEFINED_LABEL, filename, record->line);
		}
	}
	return no_errors;
}

#endif /* __PARSER_C_ */
