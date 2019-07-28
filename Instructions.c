#ifndef __INSTRUCTIONS_C_
#define __INSTRUCTIONS_C_

/* ---------------------------------------------------------------
Represents the definitions of instructions syntax allowed.
---------------------------------------------------------------- */

#include "Instructions.h"
#include <string.h>

/* Declearation keywords. Have to be ordered lexically */
static Keyword declarations[] = {
	{ ".data", KW_TYPE_DECLARATION, DEC_TYPE_DATA },
	{ ".entry", KW_TYPE_DECLARATION, DEC_TYPE_ENTRY },
	{ ".extern", KW_TYPE_DECLARATION, DEC_TYPE_EXTERN },
	{ ".string", KW_TYPE_DECLARATION, DEC_TYPE_STRING },
	{ ".struct", KW_TYPE_DECLARATION, DEC_TYPE_STRUCT },
	{ NULL, 0, 0 }
};

/* Instructions keywords ordered by name */
static Keyword instructions[] = {
	{ "add", KW_TYPE_INSTRUCTION, 2 },
	{ "bne", KW_TYPE_INSTRUCTION, 10 },
	{ "clr", KW_TYPE_INSTRUCTION, 5 },
	{ "cmp", KW_TYPE_INSTRUCTION, 1 },
	{ "dec", KW_TYPE_INSTRUCTION, 8 },
	{ "inc", KW_TYPE_INSTRUCTION, 7 },
	{ "jmp", KW_TYPE_INSTRUCTION, 9 },
	{ "jsr", KW_TYPE_INSTRUCTION, 13 },
	{ "lea", KW_TYPE_INSTRUCTION, 6 },
	{ "mov", KW_TYPE_INSTRUCTION, 0 },
	{ "not", KW_TYPE_INSTRUCTION, 4 },
	{ "prn", KW_TYPE_INSTRUCTION, 12 },
	{ "red", KW_TYPE_INSTRUCTION, 11 },
	{ "rts", KW_TYPE_INSTRUCTION, 14 },
	{ "stop", KW_TYPE_INSTRUCTION, 15 },
	{ "sub", KW_TYPE_INSTRUCTION, 3 },
	{ NULL, 0, 0 }
};

/* Registers keywords ordered by name */
static Keyword registers[] = {
	{ "r0", KW_TYPE_REGISTER, 0 },
	{ "r1", KW_TYPE_REGISTER, 1 },
	{ "r2", KW_TYPE_REGISTER, 2 },
	{ "r3", KW_TYPE_REGISTER, 3 },
	{ "r4", KW_TYPE_REGISTER, 4 },
	{ "r5", KW_TYPE_REGISTER, 5 },
	{ "r6", KW_TYPE_REGISTER, 6 },
	{ "r7", KW_TYPE_REGISTER, 7 },
	{ NULL, 0, 0 }
};

/* Table holding allowed addressing types for each instruction code
(which is the index in the table).*/
static InstructionAddressRecord InstAddrTable[] = {
	{ ADDR_TYPE_ALL,	ADDR_TYPE_DIRECT | ADDR_TYPE_STRUCT | ADDR_TYPE_REGISTER },						/* mov */
	{ ADDR_TYPE_ALL,	ADDR_TYPE_ALL },																/* cmp */
	{ ADDR_TYPE_ALL,	ADDR_TYPE_DIRECT | ADDR_TYPE_STRUCT | ADDR_TYPE_REGISTER },						/* add*/
	{ ADDR_TYPE_ALL,	ADDR_TYPE_DIRECT | ADDR_TYPE_STRUCT | ADDR_TYPE_REGISTER },						/* sub */
	{ ADDR_TYPE_NONE,	ADDR_TYPE_DIRECT | ADDR_TYPE_STRUCT | ADDR_TYPE_REGISTER },						/* not */
	{ ADDR_TYPE_NONE,	ADDR_TYPE_DIRECT | ADDR_TYPE_STRUCT | ADDR_TYPE_REGISTER },						/* clr */
	{ ADDR_TYPE_DIRECT | ADDR_TYPE_STRUCT, ADDR_TYPE_DIRECT | ADDR_TYPE_STRUCT | ADDR_TYPE_REGISTER },	/* lea */
	{ ADDR_TYPE_NONE,	ADDR_TYPE_DIRECT | ADDR_TYPE_STRUCT | ADDR_TYPE_REGISTER },						/* inc */
	{ ADDR_TYPE_NONE,	ADDR_TYPE_DIRECT | ADDR_TYPE_STRUCT | ADDR_TYPE_REGISTER },						/* dec */
	{ ADDR_TYPE_NONE,	ADDR_TYPE_DIRECT | ADDR_TYPE_STRUCT | ADDR_TYPE_REGISTER },						/* jmp */
	{ ADDR_TYPE_NONE,	ADDR_TYPE_DIRECT | ADDR_TYPE_STRUCT | ADDR_TYPE_REGISTER },						/* bne */
	{ ADDR_TYPE_NONE,	ADDR_TYPE_DIRECT | ADDR_TYPE_STRUCT | ADDR_TYPE_REGISTER },						/* red */
	{ ADDR_TYPE_NONE,	ADDR_TYPE_ALL },																/* prn */
	{ ADDR_TYPE_NONE,	ADDR_TYPE_DIRECT | ADDR_TYPE_STRUCT | ADDR_TYPE_REGISTER },						/* jsr */
	{ ADDR_TYPE_NONE,	ADDR_TYPE_NONE },																/* rts */
	{ ADDR_TYPE_NONE,	ADDR_TYPE_NONE },																/* stop */
};

/* ---------------------------------------------------------------
Searches for a keyword in a given table.
--------------------------------------------------------------- */
static Keyword *search_keywords_table(Keyword *table, char *name)
{
	int last_cmp = -1;

	/* Searches the table by name until we find the keyword or we
	find a keyword which is laxialy bigger then the given name
	or we got to the end of the table.*/
	while (table->name && ((last_cmp = strcmp(table->name, name)) < 0))
	{
		table++;
	}

	/* If we are in the end of the table or the last comparrison
	result wasnt 0, that means that the keyword was not found
	and we set the return value to NULL.*/
	if (table->name == NULL || last_cmp)
	{
		table = NULL;
	}

	return table;
}

/* ---------------------------------------------------------------
Search for keyword in the given keyword tables.
----------------------------------------------------------------- */
Keyword *get_keyword(char *search_text, KeywordType types)
{
	Keyword *kw = NULL;

	/* Search the instruction table if needed.*/
	if (types & KW_TYPE_INSTRUCTION)
		kw = search_keywords_table(instructions, search_text);

	/* Search the declaration table if needed.*/
	if (kw == NULL && (types & KW_TYPE_DECLARATION))
		kw = search_keywords_table(declarations, search_text);

	/* Search the registers table if needed.*/
	if (kw == NULL && (types & KW_TYPE_REGISTER))
		kw = search_keywords_table(registers, search_text);

	return kw;
}

/* ---------------------------------------------------------------
Search for an instruction addressing record
--------------------------------------------------------------- */
InstructionAddressRecord *get_instruction_addressing(unsigned char inst_id)
{
	return &(InstAddrTable[inst_id]);
}

#endif /* __INSTRUCTIONS_C_ */