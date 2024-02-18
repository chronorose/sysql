#include "Pager.cpp"
#include <map>
#include <cstring>
// #include "Parser.cpp"
namespace Engine {
    class Engine {
    };
    class Table {
    };
    class Database {
        string dbName_;
        string dbFileName_;
        Pager *pager_;
        map<Table, string> tableFiles_;
        public:
        bool checkSum() {
            char buffer[8];
            pager_->open_read();
            char *checkSum = readBytes<char *>(pager_->file_, buffer);
            if (strcmp(checkSum, "sysql db") == 0)
                return true;
            return false;
        }
        FileHeader getDbHeader() {
            return pager_->getHeader();
        }
        string getDbName() { return dbName_; }
        Database(string dbName) {
            dbName_ = dbName;
            dbFileName_ = dbName + ".sysql";
            pager_ = new Pager(dbFileName_);
            pager_->write_initial();
        }
        ~Database() {
            delete pager_;
        }
    };
}

int main() {
    string dbName = "users";
    cout << dbName <<endl;
    Engine::Database *db = new Engine::Database(dbName);

    if (!db->checkSum()) {
        cout << "Not a sysdb file" << endl;
    } else {
        FileHeader hdr = db->getDbHeader();
        cout << "Sysdb file!" << endl;
        cout << hdr.root;
    }

    delete db;
    return 0;
}
