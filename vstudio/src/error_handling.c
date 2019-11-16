#include "error_handling.h"
#include "interpreter.h"
#include "console.h"

//FUNKCJE POMOCNICZE

int stringToInt(char num[]) {
	char* end;
	errno = 0;
	const long test = strtol(num, &end, 10);
	if (end == num || '\0' != *end || ((LONG_MIN == test || LONG_MAX == test) && ERANGE == errno) || (test > INT_MAX) || (test < INT_MIN)) {
		return CONV_ERROR;
	}
	else return (int)test;
}

int checkRegister(char r[]) {
	int test = stringToInt(r);
	return (test != CONV_ERROR && test >= 0 && test <= 15);
}

void reconstructLine(command c, char line[]) {
	char temp[MAX_LENGTH];
	memset(temp, 0, sizeof temp);
	if (strlen(label_names.data[current_line])) {
		strcat(temp, label_names.data[current_line]);
		strcat(temp, " ");
	}
	strcat(temp, c.code);
	strcat(temp, " ");
	strcat(temp, c.arg1);
	if (strlen(c.arg2)) {
		strcat(temp, ",");
		strcat(temp, c.arg2);
	}
	if (strlen(c.arg3)) {
		strcat(temp, "(");
		strcat(temp, c.arg3);
		strcat(temp, ")");
	}
	strcpy(line, temp);
}

//WYŚWIETLANIE BŁĘDU

void error(enum error_code code, char c[]) {
    strcpy(breakpoint, c);
    if(code == FILE_NOT_FOUND) strcpy(error_message, "Nie znaleziono pliku");
    else if(code == LINE_TOO_LONG) strcpy(error_message, "Zbyt długa linia");
    else if(code == INVALID_COMMAND) strcpy(error_message, "Nieprawidłowy rozkaz");
    else if(code == INVALID_SYNTAX) strcpy(error_message, "Nieprawidłowa składnia");
    else if(code == DIVISION_BY_ZERO) strcpy(error_message, "Dzielenie przez 0");
    else if(code == LABEL_MISSING) strcpy(error_message, "Nie znaleziono etykiety");
    else if(code == VARIABLE_MISSING) strcpy(error_message, "Nie znaleziono zmiennej");
    else if(code == INVALID_ADDRESS) strcpy(error_message, "Nieprawidłowy adres");

    printf(CLEAR);
    printMenu(_ERROR);
	printf("\n\n\n\033[?25h%s", RESET);
	getchar();
    freeMemory();

	exit(0);
}

//SPRAWDZENIE SKŁADNI

void checkParsing(char tokens[][MAX_LENGTH], int token_number, char line[]) {
    if(token_number < 2) error(INVALID_SYNTAX, line);
    if(!isCommand(tokens[0]) && !isCommand(tokens[1])) error(INVALID_COMMAND, line);
    int if_label = !isCommand(tokens[0]);

    if(!if_label && (stringEquals(tokens[0], "DS") || stringEquals(tokens[0], "DC"))) error(INVALID_SYNTAX, line);
    else if(stringEquals(tokens[if_label], "DS")) {
        if(stringToInt(tokens[0]) != CONV_ERROR) error(INVALID_SYNTAX, line);
        else if(token_number == 4) {
            if ((stringToInt(tokens[if_label+1]) == CONV_ERROR) || (!stringEquals(tokens[if_label+2], "INTEGER"))) 
                error(INVALID_SYNTAX, line);
        } else if(token_number == 3) {
            if(!stringEquals(tokens[if_label+1], "INTEGER"))
                error(INVALID_SYNTAX, line);
        } else error(INVALID_SYNTAX, line);
    } else if(stringEquals(tokens[if_label], "DC")) {
        if(stringToInt(tokens[0]) != CONV_ERROR) error(INVALID_SYNTAX, line);
        else if(token_number == 5) {
            if ((stringToInt(tokens[if_label+1]) == CONV_ERROR) || (!stringEquals(tokens[if_label+2], "INTEGER")) ||
            (stringToInt(tokens[if_label+3]) == CONV_ERROR)) 
                error(INVALID_SYNTAX, line);
        } else if(token_number == 4) {
            if(!stringEquals(tokens[if_label+1], "INTEGER") || stringToInt(tokens[if_label+2]) == CONV_ERROR)
                error(INVALID_SYNTAX, line);
        } else error(INVALID_SYNTAX, line);
    } else if(strlen(tokens[if_label]) > 1 && tokens[if_label][1] == 'R') {
        if(token_number != if_label + 3) error(INVALID_SYNTAX, line);
        else {
            if(!checkRegister(tokens[if_label+1]) || !checkRegister(tokens[if_label+2]))
                error(INVALID_SYNTAX, line);
        }
    } else if(tokens[if_label][0] == 'J') {
        if(token_number != if_label + 2) error(INVALID_SYNTAX, line);
    } else {
        if(token_number < if_label + 3) error(INVALID_SYNTAX, line);
        else if(!checkRegister(tokens[if_label + 1])) error(INVALID_SYNTAX, line);
        else if(token_number == if_label + 4 && !checkRegister(tokens[if_label + 3])) error(INVALID_SYNTAX, line);
        else if(token_number > if_label + 4) error(INVALID_SYNTAX, line);
    }     
}

//SPRAWDZENIE EGZEKUCJI ROZKAZÓW

void checkRegistryExecution(command c) {
    if(stringEquals(c.code, "DR") && registry[atoi(c.arg2)] == 0){
        char line[MAX_LENGTH];
        reconstructLine(c, line);
        error(DIVISION_BY_ZERO, line);
    } 
}

void checkJumpExecution(command c) {
    if(find(labels, c.arg1) == -1) {
        char line[MAX_LENGTH];
        reconstructLine(c, line);
        error(LABEL_MISSING, line);
    }
}

void checkCommandExecution(command c) {
    char line[MAX_LENGTH];
    reconstructLine(c, line);

	int id2 = find(variables, c.arg2);
    int test = stringToInt(c.arg2);
    if(test == CONV_ERROR && id2 == -1) error(VARIABLE_MISSING, line);
    else if(id2 == -1 && (test > memory.size * 4 || test < 0))  error(INVALID_ADDRESS, line);
    else {
        if(id2 == -1) id2 = test;
        id2 *= 4;
        if(strlen(c.arg3)) id2 += registry[atoi(c.arg3)];
        id2 /= 4;
        
        if(stringEquals(c.code, "D") && memory.data[id2] == 0) error(DIVISION_BY_ZERO, line);
    }
}