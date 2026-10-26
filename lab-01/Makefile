name_printer.o: name_printer.c name_printer.h
	gcc -c name_printer.c

main.o: main.c name_printer.h
	gcc -c main.c

helloworld: main.o name_printer.o
	gcc -o helloworld main.o name_printer.o

clean:
	rm -f helloworld *.o

gettime:
	date
