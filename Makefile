TARGET  = shell
CC   = gcc
CFLAGS = -Wall -Wextra -g

SRC = main.c Hw2.c
OBJS = $(SRC:.c=.o)
HDRS = Hw2.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
# shell : main.o Hw2.o
# 	g++ -Wall -Wextra -g -o shell main.o Hw2.o

run: $(TARGET)
	./$(TARGET)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

tar:
	tar -cvf $(TARGET).tar $(SRCS) $(HDRS) makefile 
	ls -l $(TARGET).tar



# CC = g++

# all: program

# program: main.o Hw2.o
# 	g++ -o program main.o Hw2.o

# main.o: main.c
# 	g++ -c main.c

# clean:
# 	rm -f *.o program
