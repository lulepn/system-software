#ifndef _tabelaRelZapisa_h_
#define _tabelaRelZapisa_h_

#include <iostream>
#include <string>
#include <list>
#include "../inc/relokacioniZapis.hpp"

using namespace std;

class TabelaRelokacija {
    string section; 
public:
    list<RelokacioniZapis*> *records;
    TabelaRelokacija(string name) { section = name; records = new list<RelokacioniZapis*>(); }
    void addRecord(RelokacioniZapis *record) { records->push_back(record); }
    RelokacioniZapis* getRecord() { RelokacioniZapis* ret = records->front(); records->pop_front(); return ret; }
    ~TabelaRelokacija();

    friend ostream& operator<<(ostream& os, const TabelaRelokacija& table);
};

#endif