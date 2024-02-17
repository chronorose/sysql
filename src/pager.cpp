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
    // long root = fhdr.getRoot();
    // long num = fhdr.getNum();
    // long free = fhdr.getFree();
    writeBytes(os, fhdr.getRoot());
    writeBytes(os, fhdr.getNum());
    writeBytes(os, fhdr.getFree());
    // writeLong(os, fhdr.getRoot());
    // writeLong(os, fhdr.getNum());
    // writeLong(os, fhdr.getFree());
    // os.write((char *)&root, 8);
    // os.write((char *)&num, 8);
    // os.write((char *)&free, 8);
    return os;
}

istream& operator>>(istream& is, FileHeader& fhdr) {
    is.seekg(8);
    char* buffer = new char[8];
    is.read(buffer, 8);
    fhdr.setRoot(*(long*)buffer);
    is.read(buffer, 8);
    fhdr.setNum(*(long*)buffer);
    is.read(buffer, 8);
    fhdr.setFree(*(long*)buffer);
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

    public:
    Page(long offset) {
        this->offset = offset;
        this->parent = 0;
        this->type = PgType::Leaf;
        this->left_sibling = 0;
        this->right_sibling = 0;
    }    
};

// ostream& operator<<(ostream& os, Page pg) {
    
// }

int main() {
    Pager pager(string("Kek"));
    pager.write_initial();
}
