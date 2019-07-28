#ifndef __OBJECT_H_ 
#define __OBJECT_H_

/* ---------------------------------------------------------------
Holds the definition of the structures and macros
that describe and manage a parsed representation of
an assembly program.
---------------------------------------------------------------- */

#include "Labels.h"
#include "Instructions.h"

#define CODE_MAX_RECORDS 500
#define DATA_MAX_RECORDS 500

/* MACROS */

/* A macro to initailze a new object variable.*/
#define init_ass_manager(asm) \
		{ \
			(asm)->IC = 0; \
			(asm)->DC = 0; \
			(asm)->has_entries = FALSE; \
			(asm)->has_externals = FALSE; \
		}

/* A macro to initailze a new opcode variable.*/
#define init_opcode(op_code) \
		{ \
			(op_code)->value = 0; \
		}

/* TYPES */

typedef enum
{
	OR_TYPE_OPCODE = 0,
	OR_TYPE_VALUE, 
	OR_TYPE_REGISTER_VALUE,
	OR_TYPE_LABEL_ABSOLUTE_ADDRESS,
	OR_TYPE_STRUCT_DATA
} RecordType;


/* A union for opcode to enable both full opcode initialization
and each part initialization.*/
typedef union
{
	struct
	{		
		unsigned int dest_addr_type_value : 2;
		unsigned int src_addr_type_value : 2;
		unsigned int inst_code : 4;
		unsigned int group : 2;
	} fields;
	unsigned short int value;
} OpCode;

typedef struct
{
	RecordType type;
	union
	{
		OpCode op_code;
		short int value;
		Label *label_ptr; /* the label in the instruction for example (not the label at the start of theline!!) */
	} data;
	unsigned inst_addr;
	unsigned int line;
} Record;

typedef struct
{
	unsigned int IC;
	unsigned int DC;
	Boolean has_entries;
	Boolean has_externals;
	Record code[CODE_MAX_RECORDS];
	short int data[DATA_MAX_RECORDS];
} AssemblerManager;

#endif /* __OBJECT_H_ */