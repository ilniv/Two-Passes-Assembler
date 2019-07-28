#ifndef __PARSER_H_
#define __PARSER_H_

/* ---------------------------------------------------------------
Holds the definition of the parser library
structures and functions.
---------------------------------------------------------------- */

#include "Object.h"
#include "Labels.h"
#include "IO.h"

Boolean firstScan(char *filename, FILE *fp, AssemblerManager *manager, LabelsTable *labels);
Boolean secondScan(char *filename, AssemblerManager *manager);

#endif /* __PARSER_H_ */