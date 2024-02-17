#include <pstl/glue_execution_defs.h>
#include <system_error>
#include <type_traits>
#include <vector>
#include <string>
#include <exception>
#include "lexer.cpp"


using namespace std;

enum class CommandType { Select, Insert, Create };
enum class Object { DataBase = 1, Table };
enum class Types {Int = 1, Data, String };

struct Field {
  int type;
  string name;
};

class ParseException: exception {};

class Command {};

class Create: public Command {
  public:
    Object object;
    string name;
    vector<Field> fields;
};

class Select: public Command {
  public:
    vector<string> tables;
    vector<string> selectedFields;
};

class Insert: public Command {
  public:
    string table;
};

class Parsed {
  public:
    Command command;
    CommandType type;
};

class Parser {
  private:
    Lexer lexer_;
    int len_;

    Command parseSelect() {
      Select select = Select();
      int i = 1;
      for (; i < len_ && lexer_.vec[i].type == id; i++) {
        select.selectedFields.push_back(lexer_.vec[i].value);
      }
      if (select.selectedFields.size() == 0) {
        throw ParseException();
      }
      if (i < len_ && lexer_.vec[i].type == LexemeType::From) {
        for (; i < len_ && lexer_.vec[i].type == id; i++) {
          select.tables.push_back(lexer_.vec[i].value);
        }
      } else {
        throw ParseException();
      }
      return select;
    }

    Command parseInsert() {
      Insert insert = Insert();

    }

    Command parseCreate() {
      Create create = Create();
      int i = 1;
      if (i >= len_) {
        throw ParseException();
      }
      switch (lexer_.vec[i++].type)
      {
        case LexemeType::Database:
          if (i < len_ && lexer_.vec[i].type == LexemeType::String) {
            create.object = Object::DataBase;
            create.name = lexer_.vec[i++].value;
          } else {
            throw ParseException();
          }
          if (i >= len_) {
            throw ParseException();
          }
          break;
        case LexemeType::Table:
          if (i < len_ && lexer_.vec[i].type == LexemeType::String) {
            create.object = Object::Table;
            create.name = lexer_.vec[i++].value;
          } else {
            throw ParseException();
          }
          for (; i + 2 < len_ && lexer_.vec[i + 2].type == LexemeType::Comma; i += 3) {
            auto lexemeName = lexer_.vec[i];
            auto lexemeType = lexer._vec[i + 1];
            if (lexemeName.type == LexemeType::String && 
                lexemeType.type == LexemeType::String) {
              Field field;
              switch (lexemeType.value) {
                case "Int":
                  field.type = Types::Int;
                  break;
                case "Data":
                  field.type = Types::Data;
                  break;
                case "String":
                  field.type = Types::String;
                  break;
                default:
                  throw ParseEcveption();
              }
              field.name = lexemeName.value;
              create.fields.push_back(field);
            }
          }
          //TODO
          //
          break;
        default:
          throw ParseException();
      }
    }

  public:
    Parser (Lexer lexer) {
      lexer_ = lexer;
      len_ = lexer.vec.size();
    }

    Parsed parse() {
      Parsed parsed = Parsed();
      if (len_ == 0) {
        throw ParseException();
      }
      switch (lexer_.vec[0].type) {
        case LexemeType::Select:
          parsed.command = parseSelect();
          parsed.type = CommandType::Select;
          break;
        case LexemeType::Insert:
          parsed.command = parseInsert();
          parsed.type = CommandType::Insert;
          break;
        case LexemeType::Create:
          parsed.command = parseCreate();
          parsed.type = CommandType::Create;
          break;
        default:
          throw ParseException();
      }
      return parsed;
    }
};

