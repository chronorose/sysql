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
    void getData();
    void setData();
};

template<typename T> class Data: DBData {
    T data;  
    public:
    T getData() {
        return this->data;
    }
    void setData(T data) {
        this->data = data;
    }
};

class Int: Data<int> {};
class Long: Data<long> {};
class Double: Data<double> {};
class String: Data<char*> {
    char* data;
    public:
    String() {
        this->data = new char[256];
    }
    void setData(char* data) {
        strncpy(this->data, data, 256);
    }
    ~String() {
        delete[] this->data;
    }
};

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
    vector<DBData>* rowData;
    public:
    Row() {
        this->rowData = new vector<DBData>;
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

template<typename T> void writeBytes(ostream& os, vector<T>* vec) {
    auto iter { vec->begin() };
    while (iter != vec->end()) {
        writeBytes(os, iter);
    }
}

ostream& operator<<(ostream& os, Row row) {
    
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

// readBytes for vectors; Input: input stream, vector to which we push values, size of it;
template<typename T> void readBytes(istream& is, vector<T>* vec, size_t size) {
    Buffer buf(8);
    for(size_t i = 0; i < size; i++) {
        vec->push_back(readBytes<T>(is, buf.buffer));
    }
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

