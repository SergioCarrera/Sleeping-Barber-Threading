monitor.o: monitor.o
	gcc -c monitor.c

output: sleepingStylistMon.c
	gcc -lpthread -lrt sleepingStylistMon.c monitor.o

clean:	rm *.o output
