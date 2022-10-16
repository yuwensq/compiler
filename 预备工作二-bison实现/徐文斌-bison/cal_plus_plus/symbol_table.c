#include "symbol_table.h"
int item_num;

double getval(int item_index) {
	if (item_index >= item_num) {
		printf("Wrong! item index is too big.");
		return 0;
	}
	struct Symbol_table_item item = symbol_table[item_index];
	return item.val;
}

void setval(int item_index, double val) {
	if (item_index >= item_num) {
		printf("Wrong! item index is too big.");
	}
	symbol_table[item_index].val = val;
}

int lookup(char* id) {
	for (int i = 0; i < item_num; i++) {
		char* content = symbol_table[i].content;
		if (strcmp(content, id) == 0) {
			return i;
		}
	}
	return -1;
}

int insert(char* id, int token) {
	symbol_table[item_num].content = id;
	symbol_table[item_num].val = 0;
	symbol_table[item_num++].token = token;
	return item_num - 1;
}