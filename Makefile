all: spyware

spyware:
	gcc monitor.c -o monitor

clean:
	rm spyware trusted

