objects = cal cal.tab.c cal.tab.h
.PHONY: clean
cal : cal.tab.c cal.tab.h
	gcc -o $(objects) symbol_table.c symbol_table.h
cal.tab.c cal.tab.h : cal.y
	bison -d cal.y
clean:
	-rm $(objects)