#include <cctype>
#include <iostream>
#include <map>
#include <vector>

using namespace std;


enum class LexemeType {

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

map<string, LexemeType> keywords {
    { "select", LexemeType::Select },
    { "from", LexemeType::From },
    { "table", LexemeType::Table },
    { "database", LexemeType::Database },
    { "create", LexemeType::Create },

};

string LexemeTypeToString(LexemeType type) {
    switch (type) {
        case LexemeType::Identifier:
            return "Identifier";
        case LexemeType::Select:
            return "Select";
        case LexemeType::From:
            return "From";
        case LexemeType::Table:
            return "Table";
        case LexemeType::Database:
            return "Database";
        case LexemeType::Create:
            return "Create";
        case LexemeType::None:
            return "None";
        default:
            return "Type not in enum";
    }
    return string("TODO");
}


class Lexeme {
    public:
        LexemeType type;
        string value;
        void print() {
            cout << "Type: " << LexemeTypeToString(type) << endl;
            cout << "Value: " << value << endl;
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
    public:

        size_t cursor;
        string inputBuffer;

        vector<Lexeme> lexemes;
        void addLexeme(LexemeType type, string value) {
            Lexeme lexeme(type, value);
            lexemes.push_back(lexeme);
        }

        bool isEnd() { return cursor == inputBuffer.length(); }
        bool isKeyword(string key) {
            return keywords.find(key) != keywords.end();
        }
        void moveCursor() { cursor++; }
        char current() {
            return isEnd() ? '\0' : inputBuffer.at(cursor);
        }
        void lex() {
            while (getline(cin, inputBuffer)) {
                while (!isEnd()) {
                    string buffer = "";
                    if (isspace(current())) {
                        moveCursor();
                        continue;
                    }
                    switch(current()) {
                        case '*':
                            addLexeme(LexemeType::Star, "*");
                            moveCursor();
                            break;
                        case '.':
                            addLexeme(LexemeType::Dot, ".");
                            moveCursor();
                            break;
                        case ',':
                            addLexeme(LexemeType::Comma, ",");
                            moveCursor();
                            break;
                        case '(':
                            addLexeme(LexemeType::LeftParen, "(");
                            moveCursor();
                            break;
                        case ')':
                            addLexeme(LexemeType::RightParen, ")");
                            moveCursor();
                            break;
                        case '{':
                            addLexeme(LexemeType::LeftBracket, "{");
                            moveCursor();
                            break;
                        case '}':
                            addLexeme(LexemeType::RightBracket, "}");
                            moveCursor();
                            break;
                    }
                    if (isalpha(current())) {
                        buffer.push_back(current());
                        moveCursor();
                        while (!isspace(current()) && current() != 0) {
                            buffer.push_back(current());
                            moveCursor();
                        }
                        if (isKeyword(buffer)) {
                            addLexeme(keywords[buffer], buffer);
                        } else {
                            addLexeme(LexemeType::Identifier, buffer);
                        }
                    }
                }
                moveCursor();
                printLexemes();
                inputBuffer = "";
                cursor = 0;
                lexemes.clear();
            }
        }
        void printLexemes() {
            for(size_t i = 0; i < lexemes.size(); i++) {
                lexemes.at(i).print();
            }
        }
        Lexer() {
            this->cursor = 0;
            this->inputBuffer = "";
        }
};

int main() {
    Lexer lexer;
    lexer.lex();
    return 0;
}
