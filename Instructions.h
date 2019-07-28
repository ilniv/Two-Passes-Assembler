#ifndef __INSTRUCTIONS_H_
#define __INSTRUCTIONS_H_

/* ---------------------------------------------------------------
Represents the definitions of instructions syntax allowed.
---------------------------------------------------------------- */

/* ';' is used to comment a line in the assembly.*/
#define COMMENT_CHAR	';'

/* '#' is used for immediate addressing.*/
#define IMMEDIATE_CHAR	'#'

/* Represents the keyword types.
The enum values are given as a bit mask.*/
typedef enum
{
	KW_TYPE_INSTRUCTION = 1,
	KW_TYPE_REGISTER = 2,
	KW_TYPE_DECLARATION = 4,
	KW_TYPE_ALL = KW_TYPE_INSTRUCTION | KW_TYPE_REGISTER | KW_TYPE_DECLARATION
} KeywordType;

/* Represents the declaration types.*/
typedef enum
{
	DEC_TYPE_DATA = 0,
	DEC_TYPE_ENTRY = 1,
	DEC_TYPE_EXTERN = 2,
	DEC_TYPE_STRING = 3,
	DEC_TYPE_STRUCT = 4
} DeclarationType;

/* Represents the instruction arguments addressing types.
The enum values are given as a bit mask.*/
typedef enum
{
	ADDR_TYPE_NONE = 0,
	ADDR_TYPE_IMMEDIATE = 1,
	ADDR_TYPE_DIRECT = 2,
	ADDR_TYPE_STRUCT = 4,
	ADDR_TYPE_REGISTER = 8,
	ADDR_TYPE_ALL = ADDR_TYPE_IMMEDIATE | ADDR_TYPE_DIRECT | ADDR_TYPE_REGISTER | ADDR_TYPE_STRUCT
} AddressType;

/* -------------------------------------------------------------
The structure describes a keyword in the given
assembly language.
--------------------------------------------------------------- */
typedef struct
{
	char *name;
	KeywordType type;
	unsigned char id;
} Keyword;

/* ---------------------------------------------------------------
The structure describes the addressing types of
the instruction's arguments.
--------------------------------------------------------------- */
typedef struct
{
	AddressType src_addr_type_mask;
	AddressType destAddrTypeMask;
} InstructionAddressRecord;

/* ---------------------------------------------------------------
Search for keyword in the given keyword tables.
----------------------------------------------------------------- */
Keyword *get_keyword(char *search_text, KeywordType types);

/* ---------------------------------------------------------------
Search for an instruction addressing record
for a given instruction id.
--------------------------------------------------------------- */
InstructionAddressRecord *get_instruction_addressing(unsigned char inst_id);

#endif /* __INSTRUCTIONS_H_ */