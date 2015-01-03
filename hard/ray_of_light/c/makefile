CC = gcc
CFLAGS = -g -std=c99 -pedantic-errors -Wall
PROGRAM = ray_of_light
RUN_ARGS = input/in.txt

all:
	$(CC) src/$(PROGRAM).c -o bin/$(PROGRAM).o $(CFLAGS)

run:
	./bin/$(PROGRAM).o ${RUN_ARGS}

clean:
	rm bin/$(PROGRAM).o
