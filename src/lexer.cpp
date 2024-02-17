#include <iostream>
#include <map>

using namespace std;


enum LexemeType {

    Identifier = 0,
    Number,
    String,


    // Single character lexemes
    Whitespace, Comma, Dot,
    Semicolon, LeftParen, RightParen,
    LeftBracket, RightBracket, Letter,
    Plus, Minus, Star, Equal,

    // Keywords
    Select, From, Create, Table, Database,


    None
};

map<LexemeType, string> keywords {
    { Select, "select" },
    { From, "from" },
    { Table, "table" },
    { Database, "database" },
    { Database, "database" },

};

string LexemeTypeToString(LexemeType type) {
    // switch (type) {
    //     case Identifier:
    //         return "Identifier";
    //     case Whitespace:
    //         return "Whitespace";
    //     case Comma:
    //         return "Comma";
    //     case String:
    //         return "String";
    //     case Dot:
    //         return "Dot";
    //     case LeftParen:
    //         return "LeftParen";
    //     case RightParen:
    //         return "RightParen";
    //     case LeftBracket:
    //         return "LeftBracket";
    //     case RightBracket:
    //         return "RightBracket";
    //     case None:
    //         return "None";
    //     default:
    //         return "Type not in enum";
    // }
    return string("TODO");
}


class Lexeme {
    public:
        LexemeType type;
        string value;
        void print() {
            cout << "Type: " << LexemeTypeToString(type) << endl;
            cout << "Value: " << value;
        }
        Lexeme(LexemeType type, string value) {
            this->type = type;
            this->value = value;
        }
        Lexeme() {
            type = LexemeType::None;
        }
};

class Scanner {
    public:
        size_t currentIndex;
        string inputBuffer;
        void scan() {
            while (getline(cin, inputBuffer)) {
                cout << inputBuffer << endl;
            }
        }
        Scanner() {
            currentIndex = 0;
            inputBuffer = "";
        }
};

class Lexer {
    public:
        Scanner scanner;
        void lex() {
            scanner.scan();
        }
        Lexer(Scanner scanner) {
            this->scanner = scanner;
        }
};

int main() {
    Scanner scanner;
    Lexer lexer(scanner);
    lexer.lex();
    return 0;
}
