#include "Parser.cpp"
#include <dirent.h>
#include <filesystem>
#include <map>
#include <sys/types.h>
// #include <gtest/gtest.h>
namespace fs = std::filesystem;

int fileCount(fs::path path) {
    DIR* dp;
    int i = 0;
    struct dirent* ep;
    dp = opendir(path.c_str());
    if (dp) {
        while ((ep = readdir(dp)))
            i++;
        closedir(dp);
    }
    return i;
}

class Table {};

class Database {
    string dbName_;
    map<Table, string> tableFiles_;

  public:
    void create(fs::directory_entry& root) {
        fs::directory_entry dbDir = fs::directory_entry(root.path() / dbName_);
        if (dbDir.exists()) {
            cout << "Database \"" << dbName_ << "\" already exists" << endl;
        } else {
            if (!fs::create_directory(root.path() / dbName_)) {
                cout << "Couldn't create database" << endl;
                exit(1);
            } else {
                cout << "Database \"" << dbName_ << "\" successfully created" << endl;
            }
        }
    }

    void drop(fs::directory_entry& root) {
        fs::directory_entry dbDir = fs::directory_entry(root.path() / dbName_);
        int rmEntries = remove_all(fs::path(root.path() / dbName_));
        if (rmEntries == 0) {
            cout << "Database \"" << dbName_ << "\" doesn't exist" << endl;
        } else if (rmEntries - 1 == fileCount(root.path() / dbName_)) {
            cout << "Database \"" << dbName_ << "\" successfully deleted" << endl;
        }
    }

    string getDbName() {
        return dbName_;
    }

    Database(string& dbName) {
        dbName_ = dbName;
    }
};

class Engine {
    string defaultSysqlFolderName = ".sysql";
    fs::directory_entry rootDir_;
    Database* currentDb_;

  public:
    fs::directory_entry getRootDir() {
        return rootDir_;
    }

    Engine() {
        currentDb_ = nullptr;
        rootDir_ = fs::directory_entry(fs::current_path() / defaultSysqlFolderName);
        if (!rootDir_.exists())
            if (!fs::create_directory(rootDir_.path())) {
                cout << "Couldn't create .sysql directory" << endl;
                exit(1);
            }
        rootDir_.refresh();
    }

    void dropDb(string dbName) {
        Database db(dbName);
        db.drop(rootDir_);
    }
    vector<string> listAllDb() {
        vector<string> dbs {};
        for(auto const& dir_entry : fs::directory_iterator(rootDir_.path())) {
            dbs.push_back(dir_entry.path().filename());
        }
        return dbs;
    }
    void createDb(string dbName) {
        Database db(dbName);
        db.create(rootDir_);
    }

    void connectDb(string dbName) {
        if (!fs::directory_entry(rootDir_.path() / dbName).exists()) {
            cout << "Couldn't connect to \"" << dbName << "\", database doesn't exist" << endl;
            delete currentDb_;
            return;
        }
        delete currentDb_;
        currentDb_ = new Database(dbName);
        return;
    }

    void evalQuery(Parsed* token) {
        switch (token->type) {
            case CommandType::Create: {
                Create* create = static_cast<Create*>(token->command);
                if (static_cast<Object>(create->object) == Object::DataBase) {
                    createDb(create->name);
                } else {
                    // TODO for tables
                }
                delete create;
                break;
            }
            case CommandType::Drop: {
                Drop* drop = static_cast<Drop*>(token->command);
                if (static_cast<Object>(drop->object) == Object::DataBase) {
                    dropDb(drop->name);
                }
                delete drop;
                break;
            }
            case CommandType::Use: {
            }
            case CommandType::Select: {
            }
            case CommandType::Insert: {
            }
            case CommandType::Quit: {
                                        exit(0);
            }
            case CommandType::List: {
                                        List *list = static_cast<List*>(token->command);
                                        if (static_cast<Object>(list->object) == Object::DataBase) {
                                            for (auto db : listAllDb()) {
                                                cout << db << endl;
                                            }
                                        } else {
                                            cout << "*All tables*\n";
                                        }
                                        delete list;
                                        break;
            }
            default: {
            }
        }
    }

    void run() {
        Lexer lexer = {};
        Parser parser;
        for (;;) {
            string line{};
            cout << "sysql ~> ";
            char c;
            while (cin.get(c)) {
                line.push_back(c);
                if (line.length() > 1 && c == '\n' && line.at(line.length() - 2) == ';') {
                    break;
                }
                if (c == '\n') {
                    cout << ((currentDb_ == nullptr) ? "" : "(" + currentDb_->getDbName() + ")")
                         << "   ... > ";
                }
            }
            try {
                Parsed tkn = parser.parse(line.substr(0, line.length() - 2));
                evalQuery(&tkn);
            } catch (ParseException& e) {
                cout << e.what();
            }
        }
    }

    ~Engine() {
        delete currentDb_;
    }
};

int main() {
    Engine* engine = new Engine();
    engine->run();
    delete engine;
    return 0;
}
