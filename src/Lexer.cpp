#include "Lexer.hpp"



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
        case LexemeType::Delete:
            return "Delete";
        case LexemeType::InsertInto:
            return "InsertInto";
        case LexemeType::PrimaryKey:
            return "PrimaryKey";


        case LexemeType::Values:
            return "Values";

        case LexemeType::Int:
            return "Int";
        case LexemeType::String:
            return "String";
        case LexemeType::Long:
            return "Long";
        case LexemeType::Double:
            return "Double";

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
        size_t back;
        string inputBuffer;

        vector<Lexeme> lexemes;

        void addLexeme(Lexeme lexeme) {
            lexemes.push_back(lexeme);
        }
        void error(string errorMsg) {
            cout << errorMsg << endl;
            exit(1);
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
        void readIdentifier() {
            while (isalpha(peek()) || isdigit(peek()))
                advanceCursor();
        }
        void readNumber() {
            while (isdigit(peek())) advanceCursor();
            if (peek() == '.') {
                if (!isdigit(peekNext()))
                    return;
                while (isdigit(peek())) advanceCursor();
            }
        }
        void readString() {
            while (peek() != '"' && !isEnd()) {
                advanceCursor();
            }
            if (isEnd()) {
                error("Unterminated string");
            }
            advanceCursor();
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
                case ',':
                    return makeLexeme(LexemeType::Comma, ",");
                case '(':
                    return makeLexeme(LexemeType::LeftParen, "(");
                case ')':
                    return makeLexeme(LexemeType::RightParen, ")");
                case '{':
                    return makeLexeme(LexemeType::LeftBracket, "{");
                case '}':
                    return makeLexeme(LexemeType::RightBracket, "}");
                case '"' : {
                    readString();
                    string substr = inputBuffer.substr(start + 1, cursor - start - 2);
                    return makeLexeme(LexemeType::String, substr);
                }
                default:
                    if (isalpha(ch)) {
                        readIdentifier();
                        string substring = inputBuffer.substr(start, cursor - start);
                        if (halvedKeywords[substring] == LexemeType::Primary && 
                                matchNextLexeme("key")
                                ) {
                            return makeLexeme(LexemeType::PrimaryKey, "primary key");
                        } else if(halvedKeywords[substring] == LexemeType::Insert &&
                                matchNextLexeme("into")) {
                                return makeLexeme(LexemeType::InsertInto, "insert into");
                                }
                        if (!isKeyword(substring)) {
                            return makeLexeme(LexemeType::Identifier, substring);
                        } else {
                            return makeLexeme(keywords[substring], substring);
                        }
                    } else if (isdigit(ch)) {
                        readNumber();
                        string substring = inputBuffer.substr(start, cursor - start);
                        return makeLexeme(LexemeType::Number, substring);
                    } else {
                        cout << "Unexpected character " << ch << endl;
                    }
            }
            return makeLexeme(LexemeType::None, "");
        }
        bool matchNextLexeme(string expected) {
            back = cursor;
            Lexeme lexeme = scanLexeme();
            if (lexeme.value == expected)
                return true;
            cursor = back;
            return false;
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
            this->back = 0;
            this->start = 0;
            this->inputBuffer = inputBuffer;
        }
        Lexer() {
            this->start = 0;
            this->cursor = 0;
            this->back = 0;
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
