CC := g++
lexer := lexer
parser := parser
cmake := cmake -S . -B build 
rm := rm -f

.PHONY: all test clean lexer parser

test:
	$(cmake)
	cd build/
	make
	./test_suite
	cd ..

lexer: $(test)
	$(CC) src/$(lexer).cpp -o build/lexer -fsanitize=address
	./build/lexer

parser: $(test)
	$(CC) src/$(parser).cpp -o build/parser -fsanitize=address

clean:
	rm -rf ./build/
