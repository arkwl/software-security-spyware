all: spyware

spyware:
	gcc monitor.c -o monitor -g

clean:
	rm monitor

