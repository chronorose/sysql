#include "Command.cpp"
#include "Lexer.cpp"
#include <exception>
#include <pstl/glue_execution_defs.h>
#include <string>
#include <system_error>
#include <type_traits>

using namespace std;

enum class CommandType { Select, Insert, Create };
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
  enum class Types { Int = 1, Data, String };

  void cheackSemicolon(int index) {
    if (!(index + 1 == len_ && lexemes_[index].type == LexemeType::Semicolon)) {
      throw ParseException();
    }
  }

  Command parseSelect() {
    Select select = Select();
    int i = 1;
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
    cheackSemicolon(i);
    return select;
  }

  Command parseInsert() {
    Insert insert = Insert();
    int i = 1;
    if (i < len_ && lexemes_[i].type == LexemeType::Identifier) {
      insert.table = lexemes_[i++].value;
    } else {
      throw ParseException();
    }
    if (!(i < len_ && lexemes_[i++].type == LexemeType::LeftParen)) {
      throw ParseException();
    }
    for (; i + 1 < len_ && lexemes_[i].type == LexemeType::String; i++) {
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
    int j = 0;
    for (; i + 1 < len_ && (lexemes_[i].type == LexemeType::String || lexemes_[i].type == LexemeType::Number); i++) {
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
    cheackSemicolon(i);
    return insert;
  }

  Command parseCreate() {
    Create create = Create();
    int i = 1;
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
      int j = 0;
      for (; i + 2 < len_ && lexemes_[i].type == LexemeType::Identifier && lexemes_[i + 1].type == LexemeType::Comma; i++) {
        auto lexemeName = lexemes_[i++];
        i++;
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
        while (i + 2 < len_ && lexemes_[i++].type == LexemeType::Comma) {
          switch (lexemes_[i++].type) {
            case LexemeType::PrimaryKey:
              create.fields[j - 1].options.push_back(Option::PrimaryKey);
              break;
            case LexemeType::Unique:
              create.fields[j - 1].options.push_back(Option::Unique);
              break;
            default:
              throw ParseException();
          }
          if (lexemes_[i].type == LexemeType::Semicolon) {
            i++;
            break;
          }
        }
      }
      if (!(i < len_ && lexemes_[i++].type == LexemeType::RightParen)) {
        throw ParseException();
      }
      break;
    default:
      throw ParseException();
    }
    cheackSemicolon(i);
    return create;
  }

public:
  Parser(Lexer lexer) {
    lexer.lex();
    lexemes_ = lexer.lexemes;
    len_ = lexemes_.size();
  }

  Parsed parse() {
    Parsed parsed = Parsed();
    if (len_ == 0) {
      throw ParseException();
    }
    switch (lexemes_[0].type) {
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
    default:
      throw ParseException();
    }
    return parsed;
  }
};
