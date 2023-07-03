#include "../inc/sekcija.hpp"

void Sekcija::addByte(int byte) {
    data->push_back(byte);
}

void Sekcija::addWord(int word){
    data->push_back(word & 255);
    data->push_back((word>>8)&255);
}

void Sekcija::addInt(int intt){
    data->push_back(intt & 255);
    data->push_back((intt>>8)& 255);
    data->push_back((intt>>16)& 255);
    data->push_back((intt>>24)& 255);
}

ostream& operator<<(ostream &os, const Sekcija &sec){
    os<<"%"<<sec.name<<"\n";
    //cout << sec.cursor << "  " << sec.size << "\n";
    list<int>::iterator it = sec.data->begin();
    int i;
    for (i = 0; i < sec.data->size(); ) {
        int byte= *it;
        if (byte<16 && byte>=0) os<<"0";
        if (byte<0) {
            unsigned char temp=byte;
            byte=temp;
        }
        os << hex<<byte<<" ";

        i++;
        if (i%8==0) os<<"\n";
        advance(it, 1);
    }
    if (i%8!=0) os<<"\n";
    return os; 
}