#ifndef CONSOLE_H
#define CONSOLE_H

#include "data_structures.h"
#include "interpreter.h"

//KODY UCIECZKI ANSI

#define RESET           "\033[0m"
#define CLEAR           "\033[2J"
#define CLEAR_LINE		"\033[2K"
#define MOVE_RIGHT(N)   "\033["#N"C"
#define MOVE_UP(N)      "\033["#N"A"
#define SAVE			"\0337"
#define RESTORE			"\0338"

#define HIGHLIGHT       "\033[38;5;197;1m"
#define HIGHLIGHT_BLINK "\033[38;5;197;1;5m"
#define CYAN 		    "\033[38;5;122;1m"
#define MINT            "\033[38;5;80m"

#define MARGIN          "     "

#define ADD_IF(string) (string ? string : "")
#define ADD_COND(condition, string) (condition ? string : "")
#define UNKNOWN 2147483647

//FUNKCJE POMOCNICZE

typedef struct {
	int rows, cols;
} console_size;

void init();
void setupConsole();
console_size getConsoleSize();
char* fill(int, char[]);
void readString(char[]);

//ZMIENNE REGULUJĄCE WYŚWIETLANIE

char input[MAX_LENGTH];
char file_name[MAX_LENGTH];
char choice[MAX_LENGTH];

int step_by_step;
int prev_line;
int longest_var, longest_label, longest_command;
int code_scroll, memory_scroll;

int whitespace;
console_size prev_size;

//FUNKCJE WYŚWIETLAJĄCE

enum menu {
	START,
	REGISTRY,
	MEMORY,
	LOADING,
	SMALL_CONSOLE,
	_ERROR, //vstudio definiuje własny ERROR
	END
};

void printMenu(enum menu);
void expandCommand(command);

void printLine();
void printRegistry();
void printMemory();
void printOutput();

#endif