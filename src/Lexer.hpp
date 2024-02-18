#pragma once
#include <cctype>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include "Lexer.hpp"

using namespace std;

enum class LexemeType {

    Identifier = 0,
    Number,


    // Single character lexemes
    Whitespace, Comma, Dot,
    Semicolon, LeftParen, RightParen,
    LeftBracket, RightBracket, Letter,
    Plus, Minus, Star, Equal, Quote,

    // Keywords
    Select, From, Create, Table, Database,
    Delete, InsertInto, PrimaryKey, Values,
    Use, Drop, Or, And, Where,

    // Halved keywords
    Primary, Key,
    Insert, Into,

    // Data types
    Int, Long, Double, String, StringDataType,

    // Modificators
    Unique, AutoIncrement,

    // Starts with .
    ListDatabases, ListTables,

    Eof, None,
};

map<string, LexemeType> halvedKeywords {
    {"primary", LexemeType::Primary},
    {"key", LexemeType::Key},
    {"insert", LexemeType::Insert},
    {"into", LexemeType::Into},
};

map<string, LexemeType> keywords {
    { "select", LexemeType::Select },
    { "create", LexemeType::Create },
    { "delete", LexemeType::Delete },
    { "drop", LexemeType::Drop },
    { "where", LexemeType::Where },
    { "insert into", LexemeType::InsertInto },
    { "primary key", LexemeType::PrimaryKey },

    { "table", LexemeType::Table },
    { "database", LexemeType::Database },

    { "from", LexemeType::From },
    { "values", LexemeType::Values },

    { "int", LexemeType::Int },
    { "long", LexemeType::Long },
    { "double", LexemeType::Double },
    { "string", LexemeType::StringDataType },


    { "auto_increment", LexemeType::AutoIncrement },
    { "unique", LexemeType::Unique },
    { "use", LexemeType::Use },

    { "and", LexemeType::And },
    { "or", LexemeType::Or },

    { ".databases", LexemeType::ListDatabases },
    { ".tables", LexemeType::ListTables },

};
