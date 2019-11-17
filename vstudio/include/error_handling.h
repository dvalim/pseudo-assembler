#ifndef _ERROR_HANDLING_H
#define _ERROR_HANDLING_H

#include "data_structures.h"
#include "interpreter.h"

#include <errno.h>
#include <limits.h>

#define CONV_ERROR -2147483647

enum error_code {
    FILE_NOT_FOUND,
    LINE_TOO_LONG,
    INVALID_COMMAND,
    INVALID_SYNTAX,
    LABEL_MISSING,
    VARIABLE_MISSING,
    INVALID_ADDRESS
};

char breakpoint[MAX_LENGTH * 10], error_message[MAX_LENGTH];

int stringToInt(char[]);
int checkRegister(char[]);
void reconstructLine(command, char[]);

void error(enum error_code, char[]);

void checkParsing(char[][MAX_LENGTH], int, char[]);
void checkJumpExecution(command);
void checkCommandExecution(command);

#endif