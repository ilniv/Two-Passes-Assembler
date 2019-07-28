#ifndef __LABELS_H_
#define __LABELS_H_

/* ---------------------------------------------------------------
Holds the definition of the labels library
structures and functions.
---------------------------------------------------------------- */

/* Maximum length of a label.*/
#define MAX_LABEL_SIZE 30

/* A constant for an invalid label address.*/
#define LABEL_INVALID_ADDRESS -1

#define init_labels_table(table) \
		{ \
		(table)->first_label = NULL; \
		}

/* -------------------------------------------------------------------
Enum for label types: code, data or external.
LABEL_TYPE_UNKNOWN is not a label type - this
is used for label initialization.
---------------------------------------------------------------------- */
typedef enum
{
	LABEL_TYPE_UNKNOWN,
	LABEL_TYPE_CODE,
	LABEL_TYPE_DATA,
	LABEL_TYPE_EXTERNAL
} LabelType;

/* ---------------------------------------------------------------
A structure for Boolean
--------------------------------------------------------------- */
typedef enum
{
	FALSE	= 0,
	TRUE	= 1
} Boolean;


/* ---------------------------------------------------------------
A structure type that describes a label in the given
assembly language. This structure has the format of a
linked list where next_label field holds a pointer to
next node in the list.
--------------------------------------------------------------- */
typedef struct lbl
{
	char name[MAX_LABEL_SIZE + 1];
	int address;
	LabelType type;
	Boolean is_entry;
	struct lbl* next_label;
} Label;

/* -----------------------------------------------------------
A structure type that describes a sorted linked
list of the labels.
-------------------------------------------------------------- */
typedef struct
{
	Label *first_label;
} LabelsTable;

/* ------------------------------------------------------------------
Free the allocated memory taken by the label objects
in a labels table (linked list). The function does
not free the memory of the table structre itself.
--------------------------------------------------------------------- */
void freeLabelsTable(LabelsTable *table);

/* ---------------------------------------------------------------
Searches for a label in the sorted labels table.
If no label is found then create a new record for
the label and add it to the sorted labels linked
list (in the right place).
--------------------------------------------------------------- */
Label *findLabel(LabelsTable *table, char *name);

#endif /* __LABELS_H_ */