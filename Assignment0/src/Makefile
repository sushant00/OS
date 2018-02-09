all : myshell cat date ls mkdir rm

myshell: myshell.c 
	gcc -o myshell myshell.c

cat: cat.c
	gcc -o cat cat.c

date: date.c
	gcc -o date date.c

ls: ls.c
	gcc -o ls ls.c

mkdir: mkdir.c
	gcc -w -o mkdir mkdir.c

rm: rm.c
	gcc -o rm rm.c


.PHONY: clean


clean:
	rm -f myshell cat date ls mkdir rm
	
run: myshell
	./myshell
