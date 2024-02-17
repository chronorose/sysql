#include <vector>
#include <string>

using namespace std;

enum class Object { DataBase = 1, Table };

struct Field {
  int type;
  string name;
};
class Command {};

class Create : public Command {
public:
  Object object;
  string name;
  vector<Field> fields;
};

class Select : public Command {
public:
  vector<string> tables;
  vector<string> selectedFields;
};

class Insert : public Command {
public:
  string table;
};

