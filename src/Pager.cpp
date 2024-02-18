#include <fstream>
#include <iostream>
#include <vector>
#include <string.h>

using namespace std;

#define SYSQL_HDR "sysql db"
#define SYSQL_HDR_SIZE 8
#define FILE_HDR_SIZE 8
#define TABLE_HDR_START 16

#define PAGE_SIZE 4096

class Page;
ostream& operator<<(ostream& os, Page& pg);
istream& operator>>(istream& is, Page& pg);

class DBData {
    public:
    int data;
    int getData() {
        return this->data;
    }
    void setData();
    virtual ~DBData() = default;
};

class Int: public DBData {
    public:
    int data;
    Int(int data) {
        this->data = data;
    }
};

// template<typename T> class Data: public DBData {
//     T data;  
//     public:
//     T getData() {
//         return this->data;
//     }
//     void setData(T data) {
//         this->data = data;
//     }
// };

// class Int: public Data<int> {
//     int data;
//     public:
//     Int(int data) {
//         this->data = data;
//     }
//     void setData(int data) {
//         this->data = data;
//     }
//     int getData() {
//         return this->data;
//     }
// };
// class Long: public Data<long> {
//     long data;
//     public:
//     Long(long data) {
//         this->data = data;
//     }
//     void setData(long data) {
//         this->data = data;
//     }
//     long getData() {
//         return this->data;
//     }
// };
// class Double: public Data<double> {
//     double data;
//     public:
//     Double(double data) {
//         this->data = data;
//     }
//     void setData(double data) {
//         this->data = data;
//     }
//     double getData() {
//         return this->data;
//     }
// };

enum class PgType {
    Leaf,
    Node  
};

enum class ColumnType {
    Int, Long, Double, String
};

class Buffer {
    public:
    char* buffer;
    Buffer(size_t size) {
        this->buffer = new char[size];
    }
    ~Buffer() {
        delete[] this->buffer;
    }
};

class FileHeader {
    public:
    long num;
    long root;
    long free;
    FileHeader() {
        this->num = 0;
        this->root = 0;
        this->free = 32;
    }
};

class TableHeader {
    public:
    long vecSize;
    long rowNum;
    vector<ColumnType>* columns;
    TableHeader(vector<ColumnType>& columns) {
        *this->columns = columns;
        this->vecSize = this->columns->size();
        this->rowNum = 0;
    }
    TableHeader(vector<ColumnType>& columns, long rowNum) {
        this->columns = &columns;
        this->vecSize = this->columns->size();
        this->rowNum = rowNum;
    }
    TableHeader() {
        this->columns = new vector<ColumnType>;
        this->vecSize = 0;
        this->rowNum = 0;
    }
    ~TableHeader() {
        delete this->columns;
    }
};

class Row {
    public:
    vector<DBData*>* rowData;
    vector<ColumnType>* typeData;
    Row(vector<ColumnType>* types) {
        this->rowData = new vector<DBData*>;
        this->typeData = types;
    }        
    ~Row() {
        delete this->rowData;
    }
};

template<typename T> char* toBytes(T* val) {
    return (char*)val;
}

template<typename T> void writeBytes(ostream& os, T val) {
    os.write(toBytes(&val), sizeof(val));
}

template<typename T> void writeBytes(ostream& os, DBData val) {
    T value = val.getData();
    os.write(ToBytes(&value), sizeof(value));
}

template<typename T> void writeBytes(ostream& os, vector<T>* vec) {
    auto iter { vec->begin() };
    while (iter != vec->end()) {
        writeBytes(os, iter);
    }
}

ostream& operator<<(ostream& os, Row* row) {
    for (size_t i = 0; i < row->rowData->size(); i++) {
        writeBytes(os, row->rowData->at(i)->getData());
        cout << row->rowData->at(i)->getData() << endl;
    }
    // while (iter != row->rowData->end()) {
    //     writeBytes(os, iter);
    //     iter++;
    // }
    return os;
}

ostream& operator<<(ostream& os, TableHeader thdr) {
    os.seekp(SYSQL_HDR_SIZE);
    writeBytes(os, thdr.vecSize);
    writeBytes(os, thdr.rowNum);
    writeBytes(os, thdr.columns);
    return os;
}

ostream& operator<<(ostream& os, FileHeader fhdr) {
    os.seekp(SYSQL_HDR_SIZE);
    writeBytes(os, fhdr.root);
    writeBytes(os, fhdr.num);
    writeBytes(os, fhdr.free);
    return os;
}

template<typename T> T readBytes(istream& is, char* buffer) {
    is.read(buffer, sizeof(T));
    return *(T*)buffer;
}

template<typename T> T readBytes(istream& is, char* buffer, size_t size) {
    is.read(buffer, size);
    return *(T*)buffer;
}

// readBytes for vectors; Input: input stream, vector to which we push values, size of it;
template<typename T> void readBytes(istream& is, vector<T>* vec, size_t size) {
    Buffer buf(8);
    for(size_t i = 0; i < size; i++) {
        vec->push_back(readBytes<T>(is, buf.buffer));
    }
}

istream& operator>>(istream& is, Row& row) {
    auto iter { row.typeData->begin() };
    row.rowData->clear();
    Buffer buf(256);
    bool str = false;
    while (iter != row.typeData->end()) {
        if (*iter.base() == ColumnType::Int) {
            cout << readBytes<int>(is, buf.buffer);
            // Int* val_int = new Int(readBytes<int>(is, buf.buffer));
            // cout << val_int->getData();
            // row.rowData->push_back(val_int);
        }
        // switch(*iter.base()) {
        //     case ColumnType::Int:
        //         Int* val_int;
        //         val_int->setData(readBytes<int>(is, buf.buffer));
        //         row.rowData->push_back(val_int);
        //         break;
        //     case ColumnType::Long:
        //         Long* val_long;
        //         val_long->setData(readBytes<long>(is, buf.buffer));
        //         row.rowData->push_back(val_long);
        //         break;
        //     case ColumnType::String:
        //         str = true;
        //         break;
        //     case ColumnType::Double:
        //         Double* val_double;
        //         val_double->setData(readBytes<double>(is, buf.buffer));
        //         row.rowData->push_back(val_double);
        //         break;
        // }
        // if (str) {
        //     String* val_string {};
        //     val_string->setData(readBytes<char*>(is, buf.buffer, 256));
        //     row.rowData->push_back(val_string);
        // }
        iter++;
    }
    return is;
}

istream& operator>>(istream& is, TableHeader& thdr) {
    is.seekg(SYSQL_HDR_SIZE);
    Buffer buf(FILE_HDR_SIZE);
    thdr.vecSize = readBytes<long>(is, buf.buffer);
    thdr.rowNum = readBytes<long>(is, buf.buffer);
    // uncosistent calling scheme because it is a vector;
    readBytes(is, thdr.columns, thdr.vecSize);
    return is;
}

istream& operator>>(istream& is, FileHeader& fhdr) {
    is.seekg(SYSQL_HDR_SIZE);
    Buffer buf(FILE_HDR_SIZE);
    fhdr.root = readBytes<long>(is, buf.buffer);
    fhdr.num = readBytes<long>(is, buf.buffer);
    fhdr.free = readBytes<long>(is, buf.buffer);
    return is;
}

class Page {
public:
    long offset;
    long parent;
    PgType type;
    long left_sibling;
    long right_sibling;
    long num;
    Page(long offset) {
        this->offset = offset;
        this->parent = 0;
        this->type = PgType::Leaf;
        this->left_sibling = 0;
        this->right_sibling = 0;
        this->num = 0;
    }    
};

ostream& operator<<(ostream& os, Page& pg) {
    os.seekp(pg.offset);   
    writeBytes(os, pg.type);
    writeBytes(os, pg.parent);
    writeBytes(os, pg.left_sibling);
    writeBytes(os, pg.right_sibling);
    writeBytes(os, pg.num);
    return os;
}

istream& operator>>(istream& is, Page& pg) {
    is.seekg(pg.offset);
    Buffer buffer(FILE_HDR_SIZE);
    pg.type = readBytes<PgType>(is, buffer.buffer);
    pg.parent = readBytes<long>(is, buffer.buffer);
    pg.left_sibling = readBytes<long>(is, buffer.buffer);
    pg.right_sibling = readBytes<long>(is, buffer.buffer);
    pg.num = readBytes<long>(is, buffer.buffer);
    return is;
}

class Pager {
    string fileName_;
    // fstream file_;
    public:
    // Maybe bad idea
    fstream file_;
    Pager(string fileName) {
        this->fileName_ = fileName;
    }
    void redirect(string fileName) {
        this->fileName_ = fileName;
    }
    void open_read() {
        if (!file_.is_open()) {
            file_.open(this->fileName_, ios::binary | fstream::in);
        }
    }
    void open_write() {
        if (!file_.is_open()) {
            file_.open(this->fileName_, ios::binary | fstream::out);
        }
    }
    void write_initial() {
        open_write();
        writeBytes(file_, SYSQL_HDR);
        FileHeader fhdr;
        file_ << fhdr;
        file_.close();
    }

    long getRowOffset() {
        open_read();
        TableHeader thdr {};
        file_ << thdr;
        file_.close();
        return thdr.vecSize + 24;
    }

    TableHeader getTHdr() {
        open_read();
        TableHeader thdr{};
        file_ << thdr;
        file_.close();
        return thdr;
    }

    Row readRow(size_t pos) {
        open_read();
        TableHeader thdr = getTHdr();
        file_.seekp(thdr.vecSize * (pos + 1));
        Row row(thdr.columns);
        file_ >> row;
        return row;
    } 

    void writeRow(size_t pos, Row row) {
        open_write();
        TableHeader thdr = getTHdr();
        file_.seekp(thdr.vecSize * (pos + 1));
        file_ << row;
        file_.close();
    }

    FileHeader getHeader() {
        open_read();
        FileHeader fhdr;
        file_ >> fhdr;
        file_.close();
        return fhdr;
    }

    long getFreeOffset() {
        return getHeader().free;
    }

    void setFreeOffset(long free) {
        FileHeader fhdr = getHeader();
        fhdr.free = free;
        open_write();
        file_ << fhdr;
        file_.close();
    }

    void createPage() {
        open_write();
        Page pg(getFreeOffset());
        setFreeOffset(pg.offset + PAGE_SIZE);
        file_ << pg;
    }

    void rewritePage(Page& pg) {
        open_write();
        file_.seekp(pg.offset);
        file_ << pg;
        file_.close();
    }

    void write_initial() {
        open_write();
        writeBytes(file_, SYSQL_HDR);
        FileHeader fhdr;
        file_ << fhdr;
        file_.close();
    }

    Row readRow(size_t pos) {
        open_read();
        TableHeader thdr = getTHdr();
        file_.seekp(thdr.vecSize * (pos + 1));
        Row row(thdr.columns);
        file_ >> row;
        return row;
    } 

    void writeRow(size_t pos, Row* row, vector<ColumnType>* vec) {
        // cout << "ok";
        TableHeader thdr = getTHdr();
        open_write();
        file_.seekp(thdr.vecSize * (pos + 1));
        file_ << row;
        file_.close();
        open_read();
        file_.seekg(thdr.vecSize * (pos + 1));
        Row row1(vec);
        file_ >> row1;
        // auto i = row1.rowData->at(0);
        // cout << i->getData();
       // cout << row1.rowData->at(1).getData();
    }

    template<typename T> void writeToPage(Page& pg, T val) {
        if (pg.num < 253) {
            open_write();
            file_.seekp(pg.offset + pg.num++ * 16);
            writeBytes(file_, val);
            file_.close();
        }
    }
    bool checkHDR() {
        file_.seekg(0);
        char* buf = new char[SYSQL_HDR_SIZE];
        string hdrFromFile = readBytes<string>(file_, buf);
        string hdr(SYSQL_HDR);
        delete[] buf;
        return hdrFromFile == hdr;
    }
    void createDB(string dbName, vector<ColumnType>& columnTypes) {
        redirect(dbName);
        write_initial();
        TableHeader tblhdr(columnTypes);
    }
    ~Pager() {
        file_.close();
    }
};

int main() {
    Pager pager("kek");
    vector<ColumnType> vec;
    vec.push_back(ColumnType::Int);
    vec.push_back(ColumnType::Int);
    vec.push_back(ColumnType::Int);
    pager.createTable("kek", vec);
    Row* row = new Row(&vec);
    // Row row(&vec);
    Int* i = new Int(32);
    row->rowData->push_back(i);
    Int* r = new Int(64);
    r->data = 72;
    row->rowData->push_back(r);
    Int* z = new Int(72);
    z->data = 72;
    row->rowData->push_back(z);
    pager.writeRow(1, row, &vec);
    // Row rowing(&vec);
    // rowing = pager.readRow(1);
    // cout << rowing << endl;
}
