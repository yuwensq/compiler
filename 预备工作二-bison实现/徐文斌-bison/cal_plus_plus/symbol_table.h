#define TABLESIZE 100
struct Symbol_table_item {
	char* content;
	int token;
	double val;
} symbol_table[TABLESIZE];
double getval(int);
void setval(int, double);
int lookup(char*);
int insert(char*, int);
