#include "data_structures.h"

//FUNKCJE POMOCNICZE

int stringEquals(char s1[], char s2[]) {
	return (strcmp(s1, s2) == 0);
}

int isCommand(char code[]) {
	return(
		stringEquals(code, "DS") || stringEquals(code, "DC") || stringEquals(code, "A")  || stringEquals(code, "AR") ||
		stringEquals(code, "S")  || stringEquals(code, "SR") || stringEquals(code, "M")  || stringEquals(code, "MR") ||
		stringEquals(code, "D")  || stringEquals(code, "DR") || stringEquals(code, "C")  || stringEquals(code, "CR") ||
		stringEquals(code, "L")  || stringEquals(code, "LR") || stringEquals(code, "LA") || stringEquals(code, "ST") ||
		stringEquals(code, "J")  || stringEquals(code, "JP") || stringEquals(code, "JN") || stringEquals(code, "JZ")
	);
}

int sgn(int v) {
	return (v > 0) - (v < 0);
}

int _max(int a, int b) {
	return (a > b ? a : b);
}

int numLength(int a) {
	return (a > 0 ? floor(log10(a)) : 0);
}

//MAPA - WYSZUKIWANIE BINARNE ZMIENNYCH PO KLUCZU

node makeNode(char key[], int value) {
	node n;
	strcpy(n.key, key);
	n.value = value;
	return n;
}

int compareNodes(const void* a, const void* b) {
	node* node_a = (node*)a;
	node* node_b = (node*)b;
	return strcmp(node_a->key, node_b->key);
}

int find(map m, char key[]) {
	int left = 0, right = m.size - 1, mid = (left + right) / 2;
	while (left != right) {
		int compare = strcmp(m.data[mid].key, key);
		if (compare > 0) right = mid;
		else if (compare == 0) return mid;
		else left = mid + 1;

		mid = (left + right) / 2;
	}
	if (!stringEquals(m.data[mid].key, key)) return -1;
	return mid;
}