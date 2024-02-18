#include <fstream>
#include <iostream>
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

enum class PgType {
    Leaf,
    Node  
};

enum class ColumnType {
    Int, Long, Double, String
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
    vector<ColumnType>* columns;
    TableHeader(vector<ColumnType>& columns) {
        this->columns = &columns;
        this->vecSize = this->columns->size();
    }
};

template<typename T> char* toBytes(T* val) {
    return (char*)val;
}

template<typename T> void writeBytes(ostream& os, T val) {
    os.write(toBytes(&val), sizeof(val));
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

istream& operator>>(istream& is, FileHeader& fhdr) {
    is.seekg(SYSQL_HDR_SIZE);
    char* buffer = new char[FILE_HDR_SIZE];
    fhdr.root = readBytes<long>(is, buffer);
    fhdr.num = readBytes<long>(is, buffer);
    fhdr.free = readBytes<long>(is, buffer);
    delete[] buffer;
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
    char* buffer = new char[8];
    pg.type = readBytes<PgType>(is, buffer);
    pg.parent = readBytes<long>(is, buffer);
    pg.left_sibling = readBytes<long>(is, buffer);
    pg.right_sibling = readBytes<long>(is, buffer);
    pg.num = readBytes<long>(is, buffer);
    delete[] buffer;
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
