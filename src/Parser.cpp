#include "Command.cpp"
#include "Lexer.cpp"
#include "Lexer.hpp"
#include <exception>
#include <iostream>
#include <pstl/glue_execution_defs.h>
#include <string>
#include <system_error>
#include <type_traits>

using namespace std;

enum class CommandType { Use, Select, Insert, Create, Drop };

class ParseException : exception {};

class Parsed {
  public:
    Command command;
    CommandType type;
};

class Parser {
  private:
    vector<Lexeme> lexemes_;
    int len_;
    int i;

    void checkEnd() {
        if (i < len_) {
            throw ParseException();
        }
    }
    Node parseCondition() {
      Node tree = Node();
      Node* current = &tree;
      for (; i + 2 < len_ ;) {
        if (lexemes_[i].type == LexemeType::Identifier) {
          current->condition.field = lexemes_[i++].value;
        } else {
          throw ParseException();
        }
        switch (lexemes_[i++].type) {
          case LexemeType::Equal:
            current->condition.op = Operator::Equal;
            break;
          default:
            throw ParseException();
        }
        switch (lexemes_[i].type) {
          case LexemeType::String:
            current->condition.type = Type::String;
            current->condition.value = lexemes_[i++].value;
            break;
          case LexemeType::Number:
            current->condition.type = Type::Int;
            current->condition.value = lexemes_[i++].value;
            break;
          default:
            throw ParseException();
        }
        if (i >= len_) {
          return tree;
        }
        switch (lexemes_[i++].type) {
          case LexemeType::And:
            current->op = Operator::And;
            break;
          case LexemeType::Or:
            current->op = Operator::Or;
            break;
          default:
            throw ParseException();
        }
        current->next = new Node();
        current = current->next;
      }
      throw ParseException();
    }

    Command parseSelect() {
        Select select = Select();
        if (i < len_ && lexemes_[i].type == LexemeType::Star) {
            select.selectedFields.push_back(lexemes_[i++].value);
        } else {
            for (; i + 1 < len_ && lexemes_[i].type == LexemeType::Identifier; i++) {
                select.selectedFields.push_back(lexemes_[i++].value);
                if (lexemes_[i].type != LexemeType::Comma) {
                    break;
                }
            }
        }
        if (select.selectedFields.size() == 0) {
            throw ParseException();
        }
        if (i + 2 < len_ && lexemes_[i].type == LexemeType::From &&
            lexemes_[i + 1].type == LexemeType::Identifier) {
            select.table = lexemes_[i + 1].value;
        } else {
            throw ParseException();
        }
        if (i < len_ && lexemes_[i++].type == LexemeType::Where) {
          select.condition = parseCondition();
        } else {
          checkEnd();
        }
        return select;
    }

    Command parseInsert() {
        Insert insert = Insert();
        if (i < len_ && lexemes_[i].type == LexemeType::Identifier) {
            insert.table = lexemes_[i++].value;
        } else {
            throw ParseException();
        }
        if (!(i < len_ && lexemes_[i++].type == LexemeType::LeftParen)) {
            throw ParseException();
        }
        for (; i + 1 < len_ && lexemes_[i].type == LexemeType::Identifier; i++) {
            Record record;
            record.name = lexemes_[i++].value;
            insert.records.push_back(record);
            if (lexemes_[i].type != LexemeType::Comma) {
                break;
            }
        }
        if (!(i + 2 < len_ && lexemes_[i++].type == LexemeType::RightParen &&
              lexemes_[i++].type == LexemeType::Values &&
              lexemes_[i++].type == LexemeType::LeftParen)) {
            throw ParseException();
        }
        size_t j = 0;
        for (; i + 1 < len_ &&
               (lexemes_[i].type == LexemeType::String || lexemes_[i].type == LexemeType::Number || lexemes_[i].type == LexemeType::Identifier);
             i++) {
            if (j >= insert.records.size()) {
                throw ParseException();
            }
            insert.records[j++].value = lexemes_[i++].value;
            if (lexemes_[i].type != LexemeType::Comma) {
                break;
            }
        }
        if (!(j == insert.records.size() && i < len_ &&
              lexemes_[i++].type == LexemeType::RightParen)) {
            throw ParseException();
        }
        checkEnd();
        return insert;
    }

    Command parseCreate() {
        Create create = Create();
        int j = 0;
        if (i >= len_) {
            throw ParseException();
        }
        switch (lexemes_[i++].type) {
            case LexemeType::Database:
                if (i < len_ && lexemes_[i].type == LexemeType::Identifier) {
                    create.object = Object::DataBase;
                    create.name = lexemes_[i++].value;
                } else {
                    throw ParseException();
                }
                break;
            case LexemeType::Table:
                if (i < len_ && lexemes_[i].type == LexemeType::Identifier) {
                    create.object = Object::Table;
                    create.name = lexemes_[i++].value;
                } else {
                    throw ParseException();
                }
                if (!(i < len_ && lexemes_[i++].type == LexemeType::LeftParen)) {
                    throw ParseException();
                }
                for (; i + 2 < len_ && lexemes_[i].type == LexemeType::Identifier; i++) {
                    auto lexemeName = lexemes_[i++];
                    auto lexemeType = lexemes_[i++];
                    Field field;
                    switch (lexemeType.type) {
                        case LexemeType::Int:
                            field.type = Type::Int;
                            break;
                        case LexemeType::Long:
                            field.type = Type::Long;
                            break;
                        case LexemeType::Double:
                            field.type = Type::Double;
                            break;
                        case LexemeType::StringDataType:
                            field.type = Type::String;
                            break;
                        default:
                            throw ParseException();
                    }
                    field.name = lexemeName.value;
                    create.fields.push_back(field);
                    j++;

                    while (i + 1 < len_) {
                        if (lexemes_[i].type == LexemeType::PrimaryKey) {
                            create.fields[j - 1].options.push_back(Option::PrimaryKey);
                        } else if (lexemes_[i].type == LexemeType::Unique) {
                            create.fields[j - 1].options.push_back(Option::Unique);
                        } else if (lexemes_[i].type == LexemeType::AutoIncrement) {
                            create.fields[j - 1].options.push_back(Option::AutoIncrement);
                        } else {
                            break;
                        }
                        i++;
                    }
                    if (lexemes_[i].type != LexemeType::Comma) {
                        break;
                    }
                }
                if (!(i < len_ && lexemes_[i++].type == LexemeType::RightParen)) {
                    throw ParseException();
                }
                break;
            default:
                throw ParseException();
        }
        checkEnd();
        return create;
    }

    Command parseUse() {
      Use use = Use();
      if (!(i + 1 < len_ && lexemes_[i].type == LexemeType::Database && lexemes_[i + 1].type == LexemeType::Identifier)) {
        throw ParseException();
      }
      use.name = lexemes_[i + 1].value;
      i += 2;
      checkEnd();
      return use;
    }

    Command parseDrop() {
      Drop drop = Drop();
      if (i >= len_) {
        throw ParseException();
      }
      switch (lexemes_[i++].type) {
        case LexemeType::Database:
          drop.object = Object::DataBase;
          break;
        case LexemeType::Table:
          drop.object = Object::Table;
          break;
        default:
          throw ParseException();
      }
      if (i < len_ && lexemes_[i].type == LexemeType::Identifier) {
        drop.name = lexemes_[i++].value;
      } else {
        throw ParseException();
      }
      checkEnd();
      return drop;
    }

  public:
    Parser(Lexer lexer) {
        // lexer.lex();
        lexemes_ = lexer.lex();
        len_ = lexemes_.size();
        lexer.printLexemes();
        i = 0;
    }
    Parser(string query) {
        // lexer.lex();
        Lexer lexer(query);
        lexemes_ = lexer.lex();
        len_ = lexemes_.size();
        lexer.printLexemes();
        i = 0;
    }
    Parser() {
        i = 0;
    }

    vector<Parsed> parse(string query) {
        Lexer lexer(query);
        lexemes_ = lexer.lex();
        lexer.printLexemes();
        i = 0;
        len_ = lexemes_.size();
        vector<Parsed> parseds;
        while (i < len_ && lexemes_[i].type != LexemeType::Eof) {
            Parsed parsed = Parsed();
            switch (lexemes_[i++].type) {
                case LexemeType::Using:
                  parsed.command = parseUse();
                  parsed.type = CommandType::Use;
                  break;
                case LexemeType::Select:
                    parsed.command = parseSelect();
                    parsed.type = CommandType::Select;
                    break;
                case LexemeType::InsertInto:
                    parsed.command = parseInsert();
                    parsed.type = CommandType::Insert;
                    break;
                case LexemeType::Create:
                    parsed.command = parseCreate();
                    parsed.type = CommandType::Create;
                    break;
                case LexemeType::Drop:
                    parsed.command = parseDrop();
                    parsed.type = CommandType::Drop;
                    break;
                default:
                    throw ParseException();
            }
            parseds.push_back(parsed);
        }
        return parseds;
    }
};

int main() {
    string str = readQueryFromFile("kal");
    Lexer lexer = Lexer(str);
    Parser parser = Parser(lexer);
    vector<Parsed> parsed = parser.parse();
    cout << parsed.size() << "\n";
}
