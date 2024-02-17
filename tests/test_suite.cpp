#include <gtest/gtest.h>
#include <string>
#include <map>
#include "../src/parser.cpp"

using namespace std;

TEST(eyo_test, BasicAssertions) {
  EXPECT_EQ(7 * 6, 42);
}

TEST(ParserTest, CreateParse) {
  string str { "create database db;\n" };
  Lexer lexer = Lexer(str);
  Parser parser = Parser(lexer);
  Parsed parsed = parser.parse();
  map<string, string> expectMap = {{"object", "1"}, {"name", "db;"}, {"fields", ""}};
  EXPECT_EQ(parsed.command.getMap(), expectMap);
}

