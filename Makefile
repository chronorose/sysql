CC := g++ -Wall -Wextra -Werror
lexer := Lexer
parser := Parser
pager := Pager
engine := Engine
cmake := cmake -S . -B build 
rm := rm -f

.PHONY: all test clean lexer parser 

test:
	$(cmake)
	cd build/ && make
	cd build/ && ./test_suite

$(pager): $(test)
	$(CC) src/$(pager).cpp -o build/pager -fsanitize=address
	./build/pager

$(lexer): $(test)
	clear && $(CC) src/$(lexer).cpp -o build/lexer -fsanitize=address
	./build/lexer

$(parser): $(test) 
	$(CC) src/$(parser).cpp -o build/parser -fsanitize=address
	./build/parser

$(engine): $(test)
	$(CC) src/$(engine).cpp -o build/engine -fsanitize=address
	./build/engine

clean:
	rm -rf ./build/
