#include "Pager.cpp"
#include <map>
#include <filesystem>
namespace Engine {
    namespace fs = std::filesystem;
    class Engine {
        string defaultSysqlFolderName = ".sysql.db";
        public:
        Engine() {
            fs::directory_entry dir(fs::current_path() / defaultSysqlFolderName);
            if (!dir.exists())
                if (!fs::create_directory(dir.path())) {
                    cout << "Couldn't create .sysqldb directory" << endl;
                    exit(1);
            }
        }
    };
    class Table {
    };
    class Database {
        string dbName_;
        string dbFileName_;
        Pager *pager_;
        map<Table, string> tableFiles_;
        public:
        FileHeader getDbHeader() { return pager_->getHeader(); }
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
    void createDb(string dbName) {
        Database *db = new Database(dbName);
        delete db;
    }
    void openDb(string dbName) {
        dbName = "";
        return;
    }
}

int main() {
    Engine::Engine *engine = new Engine::Engine();
    delete engine;
    return 0;
}
