#include "../inc/relokacioniZapis.hpp"

ostream& operator<<(ostream &os, const RelokacioniZapis &rec) {
    os<<rec.offset;
    if (rec.offset<10) os<<"       ";
    else if (rec.offset<100) os<<"      ";
    else os<<"     ";
    os<<"|"<<(rec.type?"R_386_PC32":"R_386_32  ")<<"|"<<rec.value;
    if(rec.section != "###")
        os << "|" << rec.section;
    os << "|" << rec.simbol << "\n";

    return os;
}