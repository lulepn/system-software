#include "../inc/zapis.hpp"

ostream& operator<<(ostream &os, const Zapis &rec) {
    os << rec.name;
    if (rec.name.size()<13){
        for (int i=0; i<13-rec.name.size();i++)
        os<<" ";
    }
    os << "|" << rec.offset;
    if (rec.offset < 16) os << "           ";
    else if (rec.offset< 256) os << "          ";
    else os << "         ";
    os << "|" << rec.section;
    os << "\n";
    return os;
}