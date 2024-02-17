#include "../src/Lexer.cpp"
#include <gtest/gtest.h>

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
