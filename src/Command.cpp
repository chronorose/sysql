#include <vector>
#include <string>
#include <map>

using namespace std;

enum class Object { DataBase = 1, Table };
enum class Type { Int, Long, Double, String };
enum class Option {PrimaryKey, Unique, AutoIncrement };

struct Field {
  Type type;
  string name;
  vector<Option> options;
};

struct Record {
  string name;
  string value;
};

class Command {
  public:
    map<string, string> getMap() {
      return{};
    }
};

class Create : public Command {
private:
  string fieldsToString() {
    string strFields = "";
    for (auto field: fields) {
      strFields += to_string((int)(field.type)) + " " + field.name + " "; 
    }
    return strFields;
  }
public:
  Object object;
  string name;
  vector<Field> fields;

  map<string, string> getMap() {
    return{
      {"object", to_string((int)(object))},
      {"name", name},
      {"fields", fieldsToString()}
    };
  }
};

class Select : public Command {
private:
  string fieldsToString() {
    string strFields = "";
    for (auto field: selectedFields) {
      strFields += field + " "; 
    }
    return strFields;
  }

public:
  string table;
  vector<string> selectedFields;

  map<string, string> getMap() {
    return{
      {"table", table},
      {"selectedFields", fieldsToString()}
    };
  }
};

class Insert : public Command {
private:
  string recordsToString() {
    string strRecords = "";
    for (auto record: records) {
      strRecords += record.value + " " + record.name + " "; 
    }
    return strRecords;
  }

public:
  string table;
  vector<Record> records;

  map<string, string> getMap() {
    return{
      {"table", table},
      {"records", recordsToString()}
    };
  }
};

