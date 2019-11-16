#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "data_structures.h"

int current_line;
int state; 
map variables; 
map labels; 
array memory; 
int registry[16];

int prev_memory_change, prev_arg2;

typedef struct {
	char code[MAX_LENGTH], arg1[MAX_LENGTH], arg2[MAX_LENGTH], arg3[MAX_LENGTH];
} command;

command makeCommand(char[], char[], char[], char[]);

typedef VECTOR(command) command_vector;
command_vector program;

void executeRegistry(command);
void executeJump(command);
void executeCommand(command);

string_vector label_names, variable_names;
array memory_history;
int registry_history[16];

void initMemory();
void freeMemory();

#endif