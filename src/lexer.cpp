#include <cctype>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

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


    Eof, None,
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

        case LexemeType::Comma:
            return "Comma";
        case LexemeType::Dot:
            return "Dot";
        case LexemeType::Semicolon:
            return "Semicolon";

        case LexemeType::Number:
            return "Number";

        case LexemeType::None:
            return "None";
        case LexemeType::Eof:
            return "Eof";
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
        size_t start;
        string inputBuffer;

        vector<Lexeme> lexemes;

        void addLexeme(Lexeme lexeme) {
            lexemes.push_back(lexeme);
        }

        bool isEnd() { return cursor >= inputBuffer.length(); }
        void moveCursor() {
            cursor++;
        }
        bool isKeyword(string key) {
            return keywords.find(key) != keywords.end();
        }
        char advanceCursor() {
            return inputBuffer[cursor++];
        }
        char peek() {
            return isEnd() ? '\0' : inputBuffer.at(cursor);
        }
        char peekNext() {
            return cursor + 1 >= inputBuffer.length() ? '\0' : inputBuffer.at(cursor + 1);
        }
        bool match(char expected) {
            if(isEnd()) {
                return false;
            }
            if (peek() != expected)
                return false;
            moveCursor();
            return true;
        }
        void identifier() {
            while (isalpha(peek()) || isdigit(peek()))
                advanceCursor();
        }
        void number() {
            while (isdigit(peek())) advanceCursor();
            if (peek() == '.') {
                if (!isdigit(peekNext()))
                    return;
                while (isdigit(peek())) advanceCursor();
            }
        }
        void skipWhitespace() {
            for (;;) {
                char c = peek();
                switch (c) {
                    case ' ':
                    case '\r':
                    case '\t':
                        advanceCursor();
                        break;
                    case '\n':
                        advanceCursor();
                        break;
                    default:
                        return;
                }
        }
    }
        Lexeme makeLexeme(LexemeType type, string value) {
            return Lexeme(type, value);
        }
        Lexeme scanLexeme() {
            skipWhitespace();
            start = cursor;
            if (isEnd())
                return makeLexeme(LexemeType::Eof, "Eof");
            char ch = advanceCursor();
            switch (ch) {
                case '*':
                    return makeLexeme(LexemeType::Star, "*");
                case ';':
                    return makeLexeme(LexemeType::Semicolon, ";");
                    break;
                case ',':
                    return makeLexeme(LexemeType::Comma, ",");
                default:
                    if (isalpha(ch)) {
                        identifier();
                        string substring = inputBuffer.substr(start, cursor - start);
                        if (!isKeyword(substring)) {
                            return makeLexeme(LexemeType::Identifier, substring);
                        } else {
                            return makeLexeme(keywords[substring], substring);
                        }
                    } else if(isdigit(ch)) {
                        number();
                        string substring = inputBuffer.substr(start, cursor - start);
                        return makeLexeme(LexemeType::Number, substring);
                    } else {
                        cout << "Unexpected character " << ch << endl;
                    }
            }
            return makeLexeme(LexemeType::None, "");
        }
        void lex() {
            for (;;) {
                Lexeme lexeme = scanLexeme();
                addLexeme(lexeme);
                if (lexeme.type == LexemeType::Eof)
                    break;
            }
        }
        void printLexemes() {
            for(size_t i = 0; i < lexemes.size(); i++) {
                lexemes.at(i).print();
            }
        }
        Lexer(string inputBuffer) {
            this->cursor = 0;
            this->start = 0;
            this->inputBuffer = inputBuffer;
        }
        Lexer() {
            this->start = 0;
            this->cursor = 0;
            this->inputBuffer = "";
        }
};
string readQueryFromFile(string filePath) {
    ifstream fs(filePath);
    if (!fs) {
        cout << "File " << filePath << " not opened" << endl;
    }
    stringstream strStream;
    strStream << fs.rdbuf();
    fs.close();
    return strStream.str();
}

int main(int argc, char* argv[]) {
    string filePath = argc > 1 ? argv[1] : "src/queries.txt";
    string query = readQueryFromFile(filePath);
    Lexer lexer(query);
    lexer.lex();
    lexer.printLexemes();
    return 0;
}
