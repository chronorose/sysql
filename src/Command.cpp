#include <map>
#include <string>
#include <vector>

using namespace std;

enum class Object { DataBase, Table };
enum class Type { Int, Long, Double, String };
enum class Option { PrimaryKey, Unique, AutoIncrement };
enum class Operator { Equal, NotEqual, Greater, Less, GreaterOrEqual, LessOrEqual, And, Or };

struct Field {
    Type type;
    string name;
    vector<Option> options;
};

struct Record {
    string name;
    string value;
};

class Condition {
  public:
    string field;
    Operator op;
    string value;
    Type type;
};

class Node {
  public:
    Condition condition;
    Node* next;
    Operator op;
};

class ConditionTree {
  public:
    Node head;
};

class Command {
  public:
    string getMap() {
        return {};
    }
};

class List : public Command {
  public:
    
};
class Use : public Command {
  public:
    string name;
    string getMap() {
        return "USE: name: " + name;
    }
};

class Create : public Command {
  private:
    string fieldsToString() {
        string strFields = "";
        for (auto field : fields) {
            strFields += to_string((int) (field.type)) + " " + field.name + " ";
        }
        return strFields;
    }

  public:
    Object object;
    string name;
    vector<Field> fields;

    string getMap() {
        return "CREATE: object " + to_string((int) (object)) + " name " + name + " fields " + fieldsToString();
    }
};

class Select : public Command {
  private:
    string fieldsToString() {
        string strFields = "";
        for (auto field : selectedFields) {
            strFields += field + " ";
        }
        return strFields;
    }

  public:
    string table;
    vector<string> selectedFields;
    Node condition;

    string getMap() {
        return "SELECT: table " + table + "i selectedFielda s" +  fieldsToString();
    }
};

class Insert : public Command {
  private:
    string recordsToString() {
        string strRecords = "";
        for (auto record : records) {
            strRecords += record.value + " " + record.name + " ";
        }
        return strRecords;
    }

  public:
    string table;
    vector<Record> records;

    string getMap() {
        return "INSERT: table " + table + " records " + recordsToString();
    }
};

class Drop : public Command {
  public:
    string name;
    Object object;
    
    string getMap() {
        return "DROP: name: " + name + to_string((int) object);
    }
};
