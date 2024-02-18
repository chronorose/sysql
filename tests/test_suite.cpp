#include "../src/Engine.cpp"
#include "../src/Parser.cpp"
#include <string>

using namespace std;

// ====================LEXER==================== \\

TEST(Test0, Test0) {
    string query = "";
    Lexer lexer(query);
    lexer.lex();
    ASSERT_EQ(lexer.lexemes.size(), 1);
}
TEST(SelectTest1, SelectTest1) {
    string query = "select * from ttt";
    Lexer lexer(query);
    lexer.lex();
    vector<LexemeType> expected = {
        LexemeType::Select,
        LexemeType::Star,
        LexemeType::From,
        LexemeType::Identifier,
    };
    for (size_t i = 0; i < lexer.lexemes.size() - 1; i++) {
        EXPECT_EQ(expected[i], lexer.lexemes[i].type);
    }
}
TEST(SelectTest2, SelectTest2) {
    string query = "select column from table_name";
    Lexer lexer(query);
    lexer.lex();
    vector<LexemeType> expected = {
        LexemeType::Select,
        LexemeType::Identifier,
        LexemeType::From,
        LexemeType::Identifier,
    };
    for (size_t i = 0; i < lexer.lexemes.size() - 1; i++) {
        EXPECT_EQ(expected[i], lexer.lexemes[i].type);
    }
}
TEST(SelectTest3, SelectTest3) {
    string query = "select c_name from tabble";
    Lexer lexer(query);
    lexer.lex();
    vector<LexemeType> expected = {
        LexemeType::Select,
        LexemeType::Identifier,
        LexemeType::From,
        LexemeType::Identifier,
    };
    for (size_t i = 0; i < lexer.lexemes.size() - 1; i++) {
        EXPECT_EQ(expected[i], lexer.lexemes[i].type);
    }
}
TEST(Identifiers0, Identifiers0) {
    string query = "aaa";
    Lexer lexer(query);
    lexer.lex();
    vector<LexemeType> expected = {
        LexemeType::Identifier,
    };
    for (size_t i = 0; i < lexer.lexemes.size() - 1; i++) {
        EXPECT_EQ(expected[i], lexer.lexemes[i].type);
    }
}
TEST(Identifiers1, Identifiers1) {
    string query = "a a a sdfdsfsd 3456 from";
    Lexer lexer(query);
    lexer.lex();
    vector<LexemeType> expected = {
        LexemeType::Identifier,
        LexemeType::Identifier,
        LexemeType::Identifier,
        LexemeType::Identifier,
        LexemeType::Number,
        LexemeType::From,
    };
    for (size_t i = 0; i < lexer.lexemes.size() - 1; i++) {
        EXPECT_EQ(expected[i], lexer.lexemes[i].type);
    }
}
TEST(Identifiers2, Identifiers2) {
    string query = "abcd567 88 abcde *";
    Lexer lexer(query);
    lexer.lex();
    vector<LexemeType> expected = {
        LexemeType::Identifier,
        LexemeType::Number,
        LexemeType::Identifier,
        LexemeType::Star,
    };
    for (size_t i = 0; i < lexer.lexemes.size() - 1; i++) {
        EXPECT_EQ(expected[i], lexer.lexemes[i].type);
    }
}
TEST(Insert0, Insert0) {
    string query = "insert into";
    Lexer lexer(query);
    lexer.lex();
    vector<LexemeType> expected = {
        LexemeType::InsertInto,
    };
    for (size_t i = 0; i < lexer.lexemes.size() - 1; i++) {
        EXPECT_EQ(expected[i], lexer.lexemes[i].type);
    }
}
TEST(Insert1, Insert1) {
    string query = "insert into values (\"Hello\", 123456, \"World*\")";
    Lexer lexer(query);
    lexer.lex();
    vector<LexemeType> expected = {
        LexemeType::InsertInto,
        LexemeType::Values,
        LexemeType::LeftParen,
        LexemeType::String,
        LexemeType::Comma,
        LexemeType::Number,
        LexemeType::Comma,
        LexemeType::String,
        LexemeType::RightParen,
    };
    for (size_t i = 0; i < lexer.lexemes.size() - 1; i++) {
        EXPECT_EQ(expected[i], lexer.lexemes[i].type);
    }
}
TEST(CreateTable0, CreateTable0) {
    string query = "         create table (Id, int, primary key; Name, string, unique)           ";
    Lexer lexer(query);
    lexer.lex();
    vector<LexemeType> expected = {
        LexemeType::Create,
        LexemeType::Table,
        LexemeType::LeftParen,
        LexemeType::Identifier,
        LexemeType::Comma,
        LexemeType::Int,
        LexemeType::Comma,
        LexemeType::PrimaryKey,
        LexemeType::Semicolon,
        LexemeType::Identifier,
        LexemeType::Comma,
        LexemeType::StringDataType,
        LexemeType::Comma,
        LexemeType::Unique,
        LexemeType::RightParen,
    };
    for (size_t i = 0; i < lexer.lexemes.size() - 1; i++) {
        EXPECT_EQ(expected[i], lexer.lexemes[i].type);
    }
}

// ====================PARSER==================== \\

TEST(ParserTest, CreateParse) {
  string str { "create database db;\n" };
  Lexer lexer = Lexer(str);
  Parser parser = Parser(lexer);
  Parsed parsed = parser.parse();
  map<string, string> expectMap = {{"object", "1"}, {"name", "db;"}, {"fields", ""}};
  EXPECT_EQ(parsed.command.getMap(), expectMap);
}

// ====================DB==================== \\
    
namespace fs = filesystem;
TEST(CreateDb0, CreateDb0) {
    engine::Engine *engine = new engine::Engine();
    EXPECT_EQ(engine->getRootDir().exists(), 1);
    string dbName = "first_db";
    engine->createDb(dbName);
    cout << engine->getRootDir().path() / fs::path(dbName) << endl;
    EXPECT_EQ(fs::directory_entry(engine->getRootDir().path() / fs::path(dbName)).exists(), 1);
    dbName = "second_db";
    engine->createDb(dbName);
    EXPECT_EQ(fs::directory_entry(engine->getRootDir().path() / fs::path(dbName)).exists(), 1);
    delete engine;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
