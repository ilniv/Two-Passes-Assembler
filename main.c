
#ifndef __MAIN_C_
#define __MAIN_C_

#include <stdio.h>
#include <string.h>
#include "Labels.h"
#include "Object.h"
#include "IO.h"

#include "Parser.h"

/* Max file name length.*/
#define MAX_FILENAME		50
#define MAX_EXT				3

/* File types suffix.*/
#define ASM_FILE_EXT		".as"
#define OBJ_FILE_EXT		".ob"
#define ENTERIES_FILE_EXT	".ent"
#define EXTERNALS_FILE_EXT	".ext"

/* Predecleration of the execute function.*/
void execute(char *fileName);

/*---------------------------------------------------------------
The main function.
Gets the file names to compile from the program's
arguments list and sends each of them to the
execute function.
---------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	int i;
	for (i = 1; i < argc; i++) {
		execute(argv[i]);
	}

	printf("DONE. Press any key to continue");
	getchar();

	return 0;
}

/* -----------------------------------------------------------------
Opens an assembly file, compiles it and writes the
result to an object file. Create an externals
and/or enteries files if neccecery.
-------------------------------------------------------------------- */
void execute(char *file_name)
{
	AssemblerManager manager;
	LabelsTable labels_table;

	char full_file_name[MAX_FILENAME + MAX_EXT + 1];
	FILE *fp = NULL;
	Boolean success;

	init_labels_table(&labels_table);
	init_ass_manager(&manager);

	/* Creates a string holding the name of the asm file.*/
	get_file_name(full_file_name, file_name, ASM_FILE_EXT);

	/* Read the file that needs to be compiled.*/
	if ((fp = open_file(full_file_name, "r")))
	{
		/* First pass on the content of the file.*/
		success = firstScan(full_file_name, fp, &manager, &labels_table);
		fclose(fp);

		if (success)
		{
			/* Second pass is on the object structured filled from the first pass.*/
			if (secondScan(full_file_name, &manager))
			{
				/* If both passes succeeded, we create the object file.*/
				get_file_name(full_file_name, file_name, OBJ_FILE_EXT);

				/* Open file with 'write' permissions.*/
				if ((fp = open_file(full_file_name, "w")))
				{
					write_object(fp, &manager);
					fclose(fp);
				}

				/* Create the entries file if needed.*/
				get_file_name(full_file_name, file_name, ENTERIES_FILE_EXT);

				if (manager.has_entries && (fp = open_file(full_file_name, "w")))
				{
					write_entries(fp, manager.IC, &labels_table);
					fclose(fp);
				}

				/* create the externals file if needed*/
				get_file_name(full_file_name, file_name, EXTERNALS_FILE_EXT);

				if (manager.has_externals && (fp = open_file(full_file_name, "w")))
				{
					write_externals(fp, &manager);
					fclose(fp);
				}
			}
		}
	}
}

#endif  /* _MAIN_C_ */
