#include <cwchar>
#include <iostream>

using namespace std;

enum LexemeType {
    Identifier = 0,
    Keyword,

    Whitespace,
    Punct,

    LeftParen,
    RightParen,

    LeftBracket,
    RightBracket,
    None
};

string LexemeTypeToString(LexemeType type) {
    switch (type) {
        case Identifier:
            return "Identifier";
        case Keyword:
            return "Keyword";
        case Whitespace:
            return "Whitespace";
        case Punct:
            return "Punct";
        case LeftParen:
            return "LeftParen";
        case RightParen:
            return "RightParen";
        case LeftBracket:
            return "LeftBracket";
        case RightBracket:
            return "RightBracket";
        case None:
            return "None";
        default:
            return "Type not in enum";
    }
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
class Lexer {
    size_t currentIndex;
    public:
        Lexer() {
            cout << "Hello from Lexer" << endl;
        }
};

int main() {
    Lexer lexer;
    Lexeme lex(LexemeType::Keyword, "Keyword");
    lex.print();
    return 0;
}
