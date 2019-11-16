#include "data_structures.h"
#include "interpreter.h"

command makeCommand(char code[], char arg1[], char arg2[], char arg3[]) {
	command c;
	strcpy(c.code, code);
	strcpy(c.arg1, arg1);
	strcpy(c.arg2, arg2);
	strcpy(c.arg3, arg3);
	return c;
}

void executeRegistry(command c) {
	int arg1 = atoi(c.arg1);
	int arg2 = atoi(c.arg2);

	if		(stringEquals(c.code, "AR")) registry[arg1] += registry[arg2];
	else if (stringEquals(c.code, "SR")) registry[arg1] -= registry[arg2];
	else if (stringEquals(c.code, "MR")) registry[arg1] *= registry[arg2];
	else if (stringEquals(c.code, "DR")) registry[arg1] /= registry[arg2];
	else if (stringEquals(c.code, "CR")) state = sgn(registry[arg1] - registry[arg2]);
	else if (stringEquals(c.code, "LR")) registry[arg1] = registry[arg2];

	if (!(stringEquals(c.code, "CR") || stringEquals(c.code, "LR"))) state = sgn(registry[arg1]);
}

void executeJump(command c) {
	int id = find(labels, c.arg1);

	if		(stringEquals(c.code, "JN") && state >= 0) return;
	else if (stringEquals(c.code, "JZ") && state != 0) return;
	else if (stringEquals(c.code, "JP") && state <= 0) return;

	current_line = labels.data[id].value - 1;
}

void executeCommand(command c) {
	char code[MAX_LENGTH], arg1[MAX_LENGTH], arg2[MAX_LENGTH], arg3[MAX_LENGTH];
	strcpy(code, c.code);
	strcpy(arg1, c.arg1);
	strcpy(arg2, c.arg2);
	strcpy(arg3, c.arg3);

	//OBLICZANIE ADRESU

	int id1 = atoi(arg1);
	int id2 = find(variables, arg2);

	if (id2 == -1)    id2 = atoi(arg2);
	else              id2 = variables.data[id2].value * 4;
	if (strlen(arg3)) id2 += registry[atoi(arg3)];

	prev_arg2 = id2;

	id2 /= 4;

	//ARYTMETYKA

	if		  (stringEquals(code, "A")) {
		registry[id1] += memory.data[id2];
		state = sgn(registry[id1]);
	} else if (stringEquals(code, "S")) {
		registry[id1] -= memory.data[id2];
		state = sgn(registry[id1]);
	} else if (stringEquals(code, "M")) {
		registry[id1] *= memory.data[id2];
		state = sgn(registry[id1]);
	} else if (stringEquals(code, "D")) {
		registry[id1] /= memory.data[id2];
		state = sgn(registry[id1]);
	} else if (stringEquals(code, "C")) {
		state = sgn(registry[id1] - memory.data[id2]);
	}

	//ŁADOWANIE

	else if (stringEquals(code, "L"))  registry[id1] = memory.data[id2];
	else if (stringEquals(code, "ST")) memory.data[id2] = registry[id1], prev_memory_change = id2;
	else if (stringEquals(code, "LA")) registry[id1] = id2 * 4;
}

void initMemory() {
	V_INIT(raw_code, char);
	V_INIT(code_lines, char[MAX_LENGTH]);
	V_INIT(memory, int);
	V_INIT(memory_history, int);
	V_INIT(variables, node);
	V_INIT(variable_names, char[MAX_LENGTH]);
	V_INIT(labels, node);
	V_INIT(label_names, char[MAX_LENGTH]);
	V_INIT(program, command);
}

void freeMemory() {
	V_FREE(raw_code);
	V_FREE(code_lines);
	V_FREE(memory);
	V_FREE(memory_history);
	V_FREE(variables);
	V_FREE(variable_names);
	V_FREE(labels);
	V_FREE(label_names);
	V_FREE(program);
}