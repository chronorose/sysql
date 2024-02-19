#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>

using namespace std;

#define SYSQL_HDR "sysql db"
#define SYSQL_HDR_SIZE 8
#define FILE_HDR_SIZE 8
#define TABLE_HDR_START 16

#define PAGE_SIZE 4096

class Page;
ostream& operator<<(ostream& os, Page& pg);
istream& operator>>(istream& is, Page& pg);

// class DBData {
//     int data;
//     public:
//     int getData() {
//         return this->data;
//     }
//     void setData();
// };

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
//     void setData(int data) {
//         this->data = data;
//     }
//     int getData();
// };
// class Long: public Data<long> {
//     long data;
//     public:
//     void setData(long data) {
//         this->data = data;
//     }
//     long getData();
// };
// class Double: public Data<double> {
//     double data;
//     public:
//     void setData(double data) {
//         this->data = data;
//     }
//     double getData();
// };
template <typename T> class DataStand {
    T data;

  public:
    T getData() {
        return this->data;
    }

    void setData(T data) {
        this->data = data;
    }
};

// class String: public Data<char*> {
//     char* data;
//     public:
//     String() {
//         this->data = new char[256];
//     }
//     void setData(char* data) {
//         strncpy(this->data, data, 256);
//     }
//     char* getData() {
//         return this->data;
//     }
//     ~String() {
//         delete[] this->data;
//     }
// };

enum class PgType { Leaf, Node };

enum class ColumnType { Int = 4, Long, Double, String };

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

    TableHeader() {
        this->columns = new vector<ColumnType>;
        this->vecSize = 0;
        this->rowNum = 0;
    }

    TableHeader(vector<ColumnType>* columns) {
        this->columns = new vector<ColumnType>;
        *this->columns = *columns;
        this->vecSize = this->columns->size();
        this->rowNum = 0;
    }

    TableHeader(vector<ColumnType>* columns, long rowNum) {
        this->columns = new vector<ColumnType>;
        *this->columns = *columns;
        this->vecSize = this->columns->size();
        this->rowNum = rowNum;
    }

    ~TableHeader() {
        delete this->columns;
    }
};

class Row {
  public:
    vector<DataStand<>>* rowData;
    vector<ColumnType>* typeData;

    Row(vector<ColumnType>* types) {
        this->rowData = new vector<DBData>;
        this->typeData = types;
    }

    ~Row() {
        delete this->rowData;
    }
};

template <typename T> char* toBytes(T* val) {
    return (char*) val;
}

template <typename T> void writeBytes(ostream& os, T val) {
    os.write(toBytes(&val), sizeof(val));
}

template <typename T> void writeBytes(ostream& os, DBData val) {
    T value = val.getData();
    os.write(ToBytes(&value), sizeof(value));
}

template <typename T> void writeBytes(ostream& os, vector<T>* vec) {
    auto iter{vec->begin()};
    while (iter != vec->end()) {
        writeBytes(os, *iter);
        iter++;
    }
}

ostream& operator<<(ostream& os, Row* row) {
    auto iter{row->rowData->begin()};
    while (iter != row->rowData->end()) {
        writeBytes(os, iter);
        iter++;
    }
    return os;
}

ostream& operator<<(ostream& os, TableHeader& thdr) {
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

template <typename T> T readBytes(istream& is, char* buffer) {
    is.read(buffer, sizeof(T));
    return *(T*) buffer;
}

template <typename T> T readBytes(istream& is, char* buffer, size_t size) {
    is.read(buffer, size);
    return *(T*) buffer;
}

// readBytes for vectors; Input: input stream, vector to which we push values, size of it;
template <typename T> void readBytes(istream& is, vector<T>* vec, size_t size) {
    Buffer buf(8);
    for (size_t i = 0; i < size; i++) {
        vec->push_back(readBytes<T>(is, buf.buffer));
    }
}

istream& operator>>(istream& is, Row& row) {
    auto iter{row.typeData->begin()};
    row.rowData->clear();
    Buffer buf(256);
    bool str = false;
    while (iter != row.typeData->end()) {
        str = false;
        switch (*iter.base()) {
            case ColumnType::Int:
                Int val_int;
                val_int.setData(readBytes<int>(is, buf.buffer));
                row.rowData->push_back(val_int);
                break;
            case ColumnType::Long:
                Long val_long;
                val_long.setData(readBytes<long>(is, buf.buffer));
                row.rowData->push_back(val_long);
                break;
            case ColumnType::String:
                str = true;
                break;
            case ColumnType::Double:
                Double val_double;
                val_double.setData(readBytes<double>(is, buf.buffer));
                row.rowData->push_back(val_double);
                break;
        }
        if (str) {
            String val_string{};
            val_string.setData(readBytes<char*>(is, buf.buffer, 256));
        }
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
    fstream file_;

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

    long getRowOffset() {
        open_read();
        TableHeader thdr{};
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

  public:
    Pager() {
        this->fileName_ = nullptr;
    }

    Pager(string fileName) {
        this->fileName_ = fileName;
    }

    void redirect(string fileName) {
        this->fileName_ = fileName;
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
        Row row1(vec);
        file_ >> row1;
        auto i = row1.rowData->at(1);
        cout << i.getData();
        // cout << row1.rowData->at(1).getData();
    }

    template <typename T> void writeToPage(Page& pg, T val) {
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

    void createTable(string dbName, vector<ColumnType>& columnTypes) {
        redirect(dbName);
        open_write();
        TableHeader tblhdr(&columnTypes);
        write_initial();
        open_write();
        TableHeader tblhdr(columnTypes);
        file_ << tblhdr;
        file_.close();
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
    Int i;
    i.setData(32);
    row->rowData->push_back(i);
    Int r;
    i.setData(64);
    row->rowData->push_back(r);
    Int z;
    i.setData(123);
    row->rowData->push_back(z);
    pager.writeRow(1, row, &vec);
    // Row rowing(&vec);
    // rowing = pager.readRow(1);
    // cout << rowing << endl;
}
