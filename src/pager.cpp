#include <fstream>
#include <iostream>

using namespace std;

class Page;
ostream& operator<<(ostream& os, Page& pg);
istream& operator>>(istream& is, Page& pg);

enum class PgType {
    Leaf,
    Node  
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

template<typename T> char* toBytes(T* val) {
    return (char*)val;
}

template<typename T> void writeBytes(ostream& os, T val) {
    os.write(toBytes(&val), sizeof(val));
}

ostream& operator<<(ostream& os, FileHeader fhdr) {
    os.seekp(8);
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
    is.seekg(8);
    char* buffer = new char[8];
    fhdr.root = readBytes<long>(is, buffer);
    fhdr.num = readBytes<long>(is, buffer);
    fhdr.free = readBytes<long>(is, buffer);
    delete[] buffer;
    return is;
}

// root -> 8;
// num of pages -> 16;
// free space for page -> 24;

class Pager {
    string fileName_;
    fstream file_;
    public:
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
        writeBytes(file_, "sysql db");
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

    void writePage(Page& pg) {
        open_write();
        long offset = getFreeOffset();
        file_.seekp(offset);
        file_ << pg;
        setFreeOffset(offset + 4096);
    }

    ~Pager() {
        file_.close();
    }
};

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

int main() {
    Pager pager(string("Kek"));
    pager.write_initial();
}
