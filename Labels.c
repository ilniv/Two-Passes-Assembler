#ifndef __LABELS_C_ 
#define __LABELS_C_ 

/* ---------------------------------------------------------------
Holds the definition of the labels library
structures and functions.
---------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "Labels.h"

/* ------------------------------------------------------------------
Free the allocated memory taken by the label objects
in a labels table (linked list). The function does
not free the memory of the table structre itself.
Params:		 table - A pointer to a labels table structrue.
--------------------------------------------------------------------- */
void freeLabelsTable(LabelsTable *table)
{
	Label *curr, *temp;

	/* Freeing each record in the linken list.*/
	curr = table->first_label;
	while (curr)
	{
		temp = curr;
		curr = curr->next_label;
		free(temp);
	}
}

/* ---------------------------------------------------------------
Searches for a label in the sorted labels table.
If no label is found then create a new record for
the label and add it to the sorted labels linked
list (in the right place).
Params:		 table - the labels table.
name - a name of the label to serach for.
Returns:	 A pointer to the wanted label record in the labels
tabel (the found one or the new one).
--------------------------------------------------------------- */
Label *findLabel(LabelsTable *table, char *name)
{
	Label *prev = NULL, *curr = table->first_label;
	int last_cmp = -1;

	/* Search for a record with the same name saving the result of
	previous compare operation in notFound. */
	while (curr != NULL &&
		((last_cmp = strcmp(curr->name, name)) < 0))
	{
		prev = curr;
		curr = curr->next_label;
	}

	/* If a record was not found (the value of the last compare wasn't 0)
	we create a new record for it. */
	if (last_cmp)
	{
		curr = (Label*)malloc(sizeof(Label));
		if (prev)
		{
			curr->next_label = prev->next_label;
			prev->next_label = curr;
		}
		else
		{
			prev = table->first_label;
			table->first_label = curr;
			curr->next_label = prev;
		}

		/* Initialize the new record. */
		strcpy(curr->name, name);
		curr->address = LABEL_INVALID_ADDRESS;
		curr->type = LABEL_TYPE_UNKNOWN;
		curr->is_entry = FALSE;
	}

	return curr;
}

#endif /* __LABELS_C_ */