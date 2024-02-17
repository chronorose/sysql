#include <fstream>
#include <iostream>

using namespace std;

class FileHeader {
    long num_;
    long root_;
    long free_;
    public:
    FileHeader() {
        this->num_ = 0;
        this->root_ = 32;
        this->free_ = 0;
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

ostream& operator<<(ostream& os, FileHeader fhdr) {
    os.seekp(8);
    long root = fhdr.getRoot();
    long num = fhdr.getNum();
    long free = fhdr.getFree();
    os.write((char *) & root, 8);
    os.write((char *) & num, 8);
    os.write((char *) & free, 8);
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
    void open_in() {
        file_.open(this->fileName_, ios::binary | fstream::in);
    }
    void open_out() {
        file_.open(this->fileName_, ios::binary | fstream::out);
    }
    void write_initial() {
        if (!file_.is_open()) {
            open_out();
        }
        file_.write("sysql db", 8);
        FileHeader fhdr;
        file_ << fhdr;
    }
    ~Pager() {
        file_.close();
    }
};

int main() {
    Pager pager(string("Kek"));
    pager.write_initial();
}
