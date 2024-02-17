CC := g++ -Wall -Wextra -Werror
lexer := lexer
parser := parser
pager := pager
cmake := cmake -S . -B build 
rm := rm -f

.PHONY: all test clean lexer parser

test:
	$(cmake)
	cd build/ && make
	cd build/ && ./test_suite

pager: $(test)
	$(CC) src/$(pager).cpp -o build/pager -fsanitize=address
	./build/pager

lexer: $(test)
	$(CC) src/$(lexer).cpp -o build/lexer -fsanitize=address
	./build/lexer

parser: $(test)
	$(CC) src/$(parser).cpp -o build/parser -fsanitize=address
	./build/parser

clean:
	rm -rf ./build/
