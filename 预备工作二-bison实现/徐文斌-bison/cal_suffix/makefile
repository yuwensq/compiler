objects = cal cal.tab.c cal.tab.h
.PHONY: clean
cal : cal.tab.c cal.tab.h
	gcc -o cal cal.tab.c cal.tab.h
cal.tab.c cal.tab.h : cal.y
	bison -d cal.y
clean:
	-rm $(objects)