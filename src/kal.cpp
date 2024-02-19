#include <iostream>
#include <vector>

using namespace std;

class Data {
    public:
    int fld;
    virtual ~Data() = default;
};

class Int: public Data {
    public:
    int fld;
};

class Long: public Data {
    public:
    long fld;
};

class Doube: Data {
    public:
    double fld;
};


int main() {
    vector<Data*> dt;
    Long* i = new Long;
    i->fld = 32;
    dt.push_back(i);
    auto* kek = dynamic_cast<Long*>(dt.at(0));
    cout << kek->fld;
}
