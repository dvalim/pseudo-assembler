#include "data_structures.h"
#include "interpreter.h"
#include "console.h"
#include "error_handling.h"

int main() {
	setupConsole();

	printf("%s", fill(100, "\n"));
	printMenu(START);
	readString(file_name);

	//WCZYTANIE PLIKU

	printMenu(LOADING);
	initMemory();

	int c;
	FILE* file;
	file = fopen(file_name, "r");
	if (file) {
		while ((c = getc(file)) != EOF)
			V_ADD(raw_code, char, c);
		fclose(file);
	} else error(FILE_NOT_FOUND, file_name);
	V_ADD(raw_code, char, 0);

	//PODZIAŁ NA LINIE

	char* line = strtok(raw_code.data, "\n\r");
	while (line != NULL) {
		if(strlen(line) > MAX_LENGTH) error(LINE_TOO_LONG, line);
		V_ADD_STRING(code_lines, char[MAX_LENGTH], line);
		line = strtok(NULL, "\n\r");
	}

	//INICJALIZACJA ZMIENNYCH, ZAPISANIE TREŚCI PROGRAMU

	for (int i = 0; i < 16; i++)
		registry[i] = registry_history[i] = UNKNOWN;
	registry[14] = 0;

	for (int i = 0; i < code_lines.size; i++) {
		char line[MAX_LENGTH];
		strcpy(line, code_lines.data[i]);

		//PODZIAŁ LINII NA SŁOWA

		char tokens[6][MAX_LENGTH];
		int token_number = 0;
		char* token = strtok(code_lines.data[i], " ,()*\t");
		if(token == NULL || token[0] == '/') continue;

		while (token != NULL) {
			if(token_number >= 6) error(INVALID_SYNTAX, line);
			strcpy(tokens[token_number++], token);
			token = strtok(NULL, " ,()*\t");
		}

		checkParsing(tokens, token_number, line);

		//UTWORZENIE ZMIENNYCH

		if (stringEquals(tokens[1], "DS") || stringEquals(tokens[1], "DC")) {
			longest_var = _max(longest_var, strlen(tokens[0]));

			V_ADD(variables, node, makeNode(tokens[0], memory.size));
			V_ADD_STRING(variable_names, char[MAX_LENGTH], tokens[0]);

			if (stringEquals(tokens[1], "DC")) {
				V_ADD(memory, int, atoi(tokens[token_number - 1]));
				V_ADD(memory_history, int, atoi(tokens[token_number - 1]));
			} else {
				V_ADD(memory, int, UNKNOWN);
				V_ADD(memory_history, int, UNKNOWN);
			}

			//INICJALIZACJA TABLICY

			if (!stringEquals(tokens[2], "INTEGER")) {
				int array_size = atoi(tokens[2]) - 1;
				int base_value = (stringEquals(tokens[1], "DC") ? atoi(tokens[4]) : UNKNOWN);
				while (array_size--) {
					V_ADD(memory, int, base_value);
					V_ADD(memory_history, int, base_value);
					V_ADD_STRING(variable_names, char[MAX_LENGTH], "?");
				}
			}
		} else {
			//WPISANIE ROZKAZU DO PROGRAMU, ZAPISANIE ETYKIET

			int if_label = !isCommand(tokens[0]);
			if (if_label) {
				longest_label = _max(longest_label, strlen(tokens[0]));
				V_ADD(labels, node, makeNode(tokens[0], current_line));
				V_ADD_STRING(label_names, char[MAX_LENGTH], tokens[0]);
			} else V_ADD_STRING(label_names, char[MAX_LENGTH], "");

			command comm = makeCommand(
				tokens[if_label],
				tokens[if_label + 1],
				(token_number > if_label + 2 ? tokens[if_label + 2] : ""),
				(token_number > if_label + 3 ? tokens[if_label + 3] : "")
			);

			int comm_length = strlen(comm.arg1) + (strlen(comm.arg2) ? strlen(comm.arg2) + 1 : 0) + (strlen(comm.arg3) ? strlen(comm.arg3) + 2 : 0);
			longest_command = _max(longest_command, comm_length);

			V_ADD(program, command, comm);
			current_line++;
		}
	}

	whitespace = _max(40, longest_command + longest_label + numLength(program.size) + 13);

	current_line = 0;

	qsort(variables.data, variables.size, sizeof(node), compareNodes);
	qsort(labels.data, labels.size, sizeof(node), compareNodes);

	printf(RESET);

	//SYMULACJA

	while (current_line < program.size) {
		prev_line = current_line;
		char code[MAX_LENGTH];
		strcpy(code, program.data[current_line].code);

		if (strlen(code) > 1 && code[1] == 'R') {
			executeRegistry(program.data[current_line]);
		} else if (code[0] == 'J') {
			checkJumpExecution(program.data[current_line]);
			executeJump(program.data[current_line]);
		} else {
			checkCommandExecution(program.data[current_line]);
			executeCommand(program.data[current_line]);
		}

		current_line++;

		if (step_by_step) printOutput();
	}

	//WYŚWIETLENIE KOŃCOWYCH STANÓW

	printMenu(REGISTRY);
	readString(choice);
	printf("%s\n\n\n", RESET);
	if (stringEquals(choice, "t") || stringEquals(choice, "T")) {
		for (int i = 0; i <= 17; i++) {
			printf(MARGIN);
			printRegistry(i);
			printf("\n");
		}
		printf("\n%s» Naciśnij %s[ENTER]%s, aby przejść dalej.\n%s%s",
			MARGIN, CYAN, RESET, MOVE_UP(1), "\033[47C");
		getchar();
	}

	printMenu(MEMORY);
	readString(choice);
	printf("%s\n\n\n", RESET);
	if (stringEquals(choice, "t") || stringEquals(choice, "T")) {
		for (int i = 0; i < memory.size; i++) {
			printf(MARGIN);
			printMemory(i);
			printf("\n");
		}
		printf("\n%s» Naciśnij %s[ENTER]%s, aby przejść dalej.\n%s%s",
			MARGIN, CYAN, RESET, MOVE_UP(1), "\033[47C");
		getchar();
	}

	//SPRZĄTANIE

	freeMemory();

	printMenu(END);
	getchar();
	printf("\033[?25h%s%s", RESET, fill(100, "\n"));

	return 0;
}