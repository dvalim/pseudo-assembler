#ifndef _ERROR_HANDLING_H
#define _ERROR_HANDLING_H

#include "data_structures.h"
#include "interpreter.h"

#include <errno.h>
#include <limits.h>

#define CONV__ERROR (-2147483648)

enum error_code {
    FILE_NOT_FOUND,
    LINE_TOO_LONG,
    INVALID_COMMAND,
    INVALID_SYNTAX,
    DIVISION_BY_ZERO,
    LABEL_MISSING,
    VARIABLE_MISSING,
    INVALID_ADDRESS
};

char breakpoint[MAX_INPUT * 10], error_message[MAX_INPUT];

int stringToInt(char[]);
int checkRegister(char[]);
void reconstructLine(command, char[]);

void error(enum error_code, char[]);

void checkParsing(char[][MAX_INPUT], int, char[]);
void checkRegistryExecution(command);
void checkJumpExecution(command);
void checkCommandExecution(command);

#endif