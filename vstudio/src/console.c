#include "console.h"
#include "interpreter.h"
#include "error_handling.h"

//PRZYGOTOWANIE KONSOLI

#ifdef _WIN32
#include <windows.h>

static HANDLE stdoutHandle;

void setupConsole() {
	DWORD outMode = 0;
	stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(stdoutHandle, &outMode);
	outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(stdoutHandle, outMode);
	SetConsoleOutputCP(65001);

	printf("\033[?25l");
	init();
}

console_size getConsoleSize() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	console_size cs;
	cs.rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	cs.cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	return cs;
}

#else

#include <sys/ioctl.h>
#include <unistd.h>

void setupConsole() {
	printf("\033[?25l");
	init();
}

console_size getConsoleSize() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	console_size cs;
	cs.rows = w.ws_row;
	cs.cols = w.ws_col;
	return cs;
}

#endif

void init() {
	step_by_step = 1;
	prev_line = current_line = 0;
	longest_var = longest_label = longest_command = 0;
	code_scroll = memory_scroll = prev_memory_change = 0;
	prev_size = getConsoleSize();
}

//FUNKCJE POMOCNICZE

char* fill(int n, char c[]) {
	static char str[1000];
	n = n*strlen(c);
	if(n > 1000) n = 1000;
	memset(str, 0, sizeof str);
	for(int i = 0; i < n; i++)
		str[i] = c[i%strlen(c)];
	return str;
}

void readString(char s[]) {
	fgets(input, MAX_INPUT, stdin);
	int len = strlen(input);
	for (int i = 0; i < len - 1; i++)
		s[i] = input[i];
}

//WYPISYWANIE

void printMenu(enum menu n) {
	console_size cs = getConsoleSize();
	printf("%s%s%s", RESET, CLEAR, fill(cs.rows/3, "\n"));
	printf("%s╓───────────────────────────────╖\n", MARGIN);
	printf("%s║ Interpreter pseudo-assemblera ║\n", MARGIN);
	printf("%s╙───────────────────────────────╨%s\n\n", MARGIN, fill(cs.cols - 43, "─"));
	switch(n) {
		case START:
			printf("%sZalecane jest uruchomienie konsoli w %strybie pełnoekranowym%s.\n%sPodaj nazwę pliku zawierającego %skod źródłowy%s (z rozszerzeniem) :",
				MARGIN, CYAN, RESET, MARGIN, CYAN, RESET);
			break;
		case REGISTRY:
			printf("%s%sProgram zakończył działanie.%s\n%sCzy ma być wyświetlona zawartość rejestrów? %sT/N%s",
				MARGIN, MINT, RESET, MARGIN, HIGHLIGHT_BLINK, RESET);
			break;
		case MEMORY:
			printf("%sCzy ma być wyświetlona zawartość pamięci? %sT/N%s",
				MARGIN, HIGHLIGHT_BLINK, RESET);
			break;
		case LOADING:
			printf("%sWczytywanie %s...%s",
				MARGIN, "\033[5m", RESET);
			break;
		case SMALL_CONSOLE:
			printf("%s%sKonsola jest zbyt mała.%s Naciśnij %s[ENTER]%s po powiększeniu jej, aby przejść dalej.",
				MARGIN, HIGHLIGHT, RESET, CYAN, RESET);
			break;
		case _ERROR:
			printf("%s%s╳%s  Wystąpił błąd : \"%s%s%s\" podczas próby wykonania :\n\n%s   %s%s%s", 
				MARGIN, HIGHLIGHT, RESET, HIGHLIGHT, error_message, RESET, MARGIN, HIGHLIGHT, breakpoint, RESET);
			break;
		case END:
			printf("%sNaciśnij %s[ENTER]%s, aby wyjść.",
				MARGIN, CYAN, RESET);
			break;
		
	}
	printf("%s%s%s", (n <= MEMORY ? "\n\n\n\n" : "\n\n"), MARGIN, fill(cs.cols - 10, "─"));
	printf("%s%s", fill(cs.rows/2 - (n <= MEMORY), "\n"), MOVE_RIGHT(5));
	if(n <= MEMORY) printf("%s»%s ", fill(cs.rows/2+2 - (n <= MEMORY), MOVE_UP(1)), HIGHLIGHT);
}

void expandCommand(command c) {
	printf("%s   ╰── %s", MARGIN, SAVE);
	if (strlen(c.code) > 1 && c.code[1] == 'R') {
		char operator[MAX_INPUT];
		if (stringEquals(c.code, "AR")) strcpy(operator, "+");
		else if(stringEquals(c.code, "SR")) strcpy(operator, "-");
		else if(stringEquals(c.code, "MR")) strcpy(operator, "*");
		else if(stringEquals(c.code, "DR")) strcpy(operator, "/");

		if (stringEquals(c.code, "CR")) {
			printf("R[%s] - R[%s] %s 0", 
				c.arg1, c.arg2, (state < 0 ? "<" : state > 0 ? ">" : "="));
		} else if(stringEquals(c.code, "LR")) {
			printf("R[%s] ← R[%s]", 
				c.arg1, c.arg2);
		} else {
			printf("R[%s] ← R[%s] %s R[%s]", 
				c.arg1, c.arg1, operator, c.arg2);
		}
	} else if (c.code[0] == 'J')   {
		printf("%d → %d (%s)", 
			prev_line, current_line, program.data[prev_line].arg1);
	} else {
		char operator[MAX_INPUT];
		if (stringEquals(c.code, "A")) strcpy(operator, "+");
		else if(stringEquals(c.code, "S")) strcpy(operator, "-");
		else if(stringEquals(c.code, "M")) strcpy(operator, "*");
		else if(stringEquals(c.code, "D")) strcpy(operator, "/");

		if(strlen(c.arg3)) {
			strcat(c.arg2, "(");
			strcat(c.arg2, c.arg3);
			strcat(c.arg2, ")");
		}

		if (stringEquals(c.code, "C")) {
			printf("R[%s] - %s %s 0", 
				c.arg1, c.arg2, (state < 0 ? "<" : state > 0 ? ">" : "="));
		} else if(stringEquals(c.code, "L")) {
			printf("R[%s] ← %s", 
				c.arg1, c.arg2);
		} else if(stringEquals(c.code, "LA")) {
			printf("R[%s] ← %d", 
				c.arg1, prev_arg2);
		} else if(stringEquals(c.code, "ST")) {
			printf("%d. [%s] ← R[%s]", 
				prev_arg2, c.arg2, c.arg1);
		} else {
			printf("R[%s] ← R[%s] %s %s", 
				c.arg1, c.arg1, operator, c.arg2);
		}
	}
	printf("%s%s", RESTORE, fill(whitespace - 6, MOVE_RIGHT(1)));
}

void printLine(int i) {
	if(i > program.size) {
		printf("%s", fill(whitespace + 6, MOVE_RIGHT(1)));
		return;
	}

	if(i == prev_line + 1) {
		command comm = program.data[i - 1];
		expandCommand(comm);
		return;
	} else if(i > prev_line + 1) i -= 1;

	command comm = program.data[i];
	int current = (i == prev_line);

	if(current) printf("%s %s\033[7m%s%s%s", 
		SAVE, MARGIN, fill(longest_label + longest_command + numLength(program.size) + 7, " "), RESET, RESTORE);
	printf(" %s%s%s%d.%s%s%s", 
		MARGIN, SAVE, (current ? "\033[7m" : MINT), i, (current ? "" : RESET), RESTORE, fill(numLength(program.size) + 3, MOVE_RIGHT(1)));
	printf("%s%s%s%s", 
		ADD_COND(!current, HIGHLIGHT), label_names.data[i], RESTORE, fill(longest_label + numLength(program.size) + 4, MOVE_RIGHT(1)));
	printf("%s%s%s%s %s%s%s%s%s%s%s",
		ADD_COND(!current, CYAN),
		comm.code,
		(strlen(comm.code) > 1 ? "" : " "),
		(current ? "" : RESET),
		comm.arg1,

		ADD_COND(strlen(comm.arg2), ","),
		ADD_IF(comm.arg2),

		ADD_COND(strlen(comm.arg3), "("),
		ADD_IF(comm.arg3),
		ADD_COND(strlen(comm.arg3), ")"),

		RESET
	);

	printf("%s%s", RESTORE, fill(whitespace, MOVE_RIGHT(1)));
}

void printRegistry(int i) {
	printf(SAVE);
	if(i < 14) {
		int new_value = !(registry[i] == registry_history[i]);

		printf("%sR%s [ %s%d%s ] %s= %s",
			MINT, RESET, CYAN, i, RESET, (i < 10 ? " " : ""), ADD_COND(new_value, HIGHLIGHT_BLINK));
		if (registry[i] == UNKNOWN) printf(" ?");
		else printf("%s%d", (registry[i] >= 0 ? " " : ""), registry[i]);
		printf("%s%s", ADD_COND(new_value, " ←"), RESET);

		registry_history[i] = registry[i];
	} else if(i == 14) {
		printf("%sR%s [ %s14%s ] =  %s[ADRES SEKCJI DANYCH]%s",
			MINT, RESET, CYAN, RESET, "\033[38;5;250;48;5;235m", RESET);
	} else if(i == 15) {
		printf("%sR%s [ %s15%s ] =  %s[ADRES SEKCJI ROZKAZÓW]%s",
			MINT, RESET, CYAN, RESET, "\033[38;5;250;48;5;235m", RESET);
	} else if(i == 17) {
		int new_value = ((strlen(program.data[prev_line].code) > 1 && program.data[prev_line].code[0] == 'J') || program.data[prev_line].code[0] == 'C');
		printf("REJESTR STANU PROGRAMU = %s%d%s%s",
			ADD_COND(new_value, HIGHLIGHT_BLINK),
			state,
			ADD_COND(new_value, " ←"),
			RESET
		);
	}
	printf("%s%s", RESTORE, fill(whitespace + 7, MOVE_RIGHT(1)));
}

void printMemory(int i) {
	if(i >= memory.size) return;

	int new_value = !(memory.data[i] == memory_history.data[i]);

	printf("%s%s%d.%s%s%s", 
		SAVE, MINT, i * 4, RESET, RESTORE, fill(numLength(memory.size * 4) + 3, MOVE_RIGHT(1)));
	printf("%s[ %s%s%s ]%s%s", 
		SAVE, CYAN, variable_names.data[i], RESET, RESTORE, fill(longest_var + 5, MOVE_RIGHT(1)));

	printf("= %s", ADD_COND(new_value, HIGHLIGHT_BLINK));
	if (memory.data[i] == UNKNOWN) printf(" ?");
	else printf("%s%d", (memory.data[i] >= 0 ? " " : ""), memory.data[i]);
	printf("%s%s", ADD_COND(new_value, " ←"), RESET);

	memory_history.data[i] = memory.data[i];
}

void printOutput() {
	char code[MAX_INPUT];
	strcpy(code, program.data[current_line].code);

	console_size cs = getConsoleSize();
	if(cs.cols < 3 * whitespace || cs.rows < 15) {
		printMenu(SMALL_CONSOLE);
		getchar();
		return;
	}

	int new_size = (prev_size.rows != cs.rows || prev_size.cols != cs.cols);
	prev_size = cs;
	printf("%s%s", ADD_COND(new_size, CLEAR), "\033[0;0f");
	int whitespace_mod = whitespace - 15;

	printf("%s╓─────────────╖%s╓────────────────────╖%s╓──────────────────╖\n", MARGIN, fill(whitespace_mod, " "), fill(whitespace_mod, " "));
	printf("%s║   Program   ╠%s╣   Stan rejestrów   ╠%s╣   Stan pamięci   ║\n", MARGIN, fill(whitespace_mod, "═"), fill(whitespace_mod, "═"));
	printf("%s╙─────────────╜%s╙────────────────────╜%s╙──────────────────╜\n\n", MARGIN, fill(whitespace_mod, " "), fill(whitespace_mod, " "));

	int limit = cs.rows - 9;
	for(int i = 0; i < limit; i++) {
		printf(CLEAR_LINE);
		if(prev_line - code_scroll >= limit - 1) code_scroll = prev_line - limit + 2;
		else if(prev_line - code_scroll < 0) code_scroll = prev_line;

		printLine(i + code_scroll);
		printRegistry(i);

		if(prev_memory_change - memory_scroll >= limit) memory_scroll = prev_memory_change - limit + 1;
		else if(prev_memory_change - memory_scroll < 0) memory_scroll = prev_memory_change;

		printMemory(i + memory_scroll);
		printf("\n");
	}

	printf("\n%s» Naciśnij %s[ENTER]%s, aby przejść dalej, lub %s[x]%s, aby zakończyć działanie programu.\n%s%s",
		MARGIN, CYAN, RESET, HIGHLIGHT, RESET, MOVE_UP(1), "\033[87C");

	readString(choice);
	if (stringEquals(choice, "X") || stringEquals(choice, "x")) {
		step_by_step = 0;
		printMenu(LOADING);
	}
}