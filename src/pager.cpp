#include <fstream>
#include <iostream>

using namespace std;

enum class PgType {
    Leaf,
    Node  
};

class FileHeader {
    long num_;
    long root_;
    long free_;
    public:
    FileHeader() {
        this->num_ = 0;
        this->root_ = 0;
        this->free_ = 32;
    }
    void setNum(long num) {
        this->num_ = num;
    }
    void setRoot(long root) {
        this->root_ = root;
    }
    void setFree(long free) {
        this->free_ = free;
    }
    long getNum() const {
        return this->num_;
    }
    long getRoot() const {
        return this->root_;
    }
    long getFree() const {
        return this->free_;
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
    writeBytes(os, fhdr.getRoot());
    writeBytes(os, fhdr.getNum());
    writeBytes(os, fhdr.getFree());
    return os;
}

template<typename T> T readBytes(istream& is, char* buffer) {
    is.read(buffer, sizeof(T));
    return *(T*)buffer;
}

istream& operator>>(istream& is, FileHeader& fhdr) {
    is.seekg(8);
    char* buffer = new char[8];
    fhdr.setRoot(readBytes<long>(is, buffer));
    fhdr.setNum(readBytes<long>(is, buffer));
    fhdr.setFree(readBytes<long>(is, buffer));
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
        file_.open(this->fileName_, ios::binary | fstream::in);
    }
    void open_write() {
        file_.open(this->fileName_, ios::binary | fstream::out);
    }
    void write_initial() {
        if (!file_.is_open()) {
            open_write();
        }
        writeBytes(file_, "sysql db");
        FileHeader fhdr;
        file_ << fhdr;
        file_.close();
    }

    FileHeader getHeader() {
        if (!file_.is_open()) {
            open_read();
        }
        FileHeader fhdr;
        file_ >> fhdr;
        file_.close();
        return fhdr;
    }

    long getFreeOffset() {
        return getHeader().getFree();
    }

    void writePage() {
        if (!file_.is_open()) {
            open_write();
        }
        // long offset = getFreeOffset();

    }

    ~Pager() {
        file_.close();
    }
};

class Page {
    long offset;
    long parent;
    PgType type;
    long left_sibling;
    long right_sibling;
    long num;

    public:
    Page(long offset) {
        this->offset = offset;
        this->parent = 0;
        this->type = PgType::Leaf;
        this->left_sibling = 0;
        this->right_sibling = 0;
        this->num = 0;
    }    
    long getOffset() const {
        return this->offset;
    }
    long getNum() const {
        return this->num;
    }
    void setNum(long num) {
        this->num = num;
    }
    void setOffset(long offset) {
        this->offset = offset;
    }
    long getParent() const {
        return this->parent;
    }
    void setParent(long parent) {
        this->parent = parent;
    }
    PgType getType() const {
        return this->type;
    }
    void setType(PgType type) {
        this->type = type;
    }
    long getLeft() const {
        return this->left_sibling;
    }
    void setLeft(long left) {
        this->left_sibling = left;
    }
    long getRight() const {
        return this->right_sibling;
    }
    void setRight(long right) {
        this->right_sibling = right;
    }
};

ostream& operator<<(ostream& os, Page pg) {
    os.seekp(pg.getOffset());   
    writeBytes(os, pg.getType());
    writeBytes(os, pg.getParent());
    writeBytes(os, pg.getLeft());
    writeBytes(os, pg.getRight());
    writeBytes(os, pg.getNum());
    return os;
}

istream& operator>>(istream& is, Page& pg) {
    is.seekg(pg.getOffset());
    char* buffer = new char[8];
    pg.setType(readBytes<PgType>(is, buffer));
    pg.setParent(readBytes<long>(is, buffer));
    pg.setLeft(readBytes<long>(is, buffer));
    pg.setRight(readBytes<long>(is, buffer));
    pg.setNum(readBytes<long>(is, buffer));
    delete[] buffer;
    return is;
}

int main() {
    Pager pager(string("Kek"));
    pager.write_initial();
}
