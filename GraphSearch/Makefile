FLAGS = -Wall -Wextra -pedantic-errors -Wshadow -std=c++11
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

.PHONY: all
all: search

.PHONY: run
run: search
	./search

search: search.cpp
	g++ -o search search.cpp $(FLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f search
