CC = gcc
CXX = g++
CFLAGS = -Wall -g -Iinclude
CXXFLAGS = -Wall -g -Iinclude
LDFLAGS = -lpthread -lm

SRC_C = $(wildcard src/*.c)
OBJ_C = $(SRC_C:src/%.c=obj/%.o)
OBJ_CPP = obj/main.o

TARGET = main

all: $(TARGET)

$(TARGET): $(OBJ_C) $(OBJ_CPP)
	$(CXX) $^ -o $@ $(LDFLAGS)

obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/main.o: main.cpp | obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj:
	mkdir -p obj

clean:
	rm -rf obj $(TARGET)

.PHONY: all clean
