#include "../inc/tabelaRelZapisa.hpp"

TabelaRelokacija::~TabelaRelokacija() {
    list<RelokacioniZapis*>::iterator it = records->begin();
    for (int i = 0; i < records->size(); i++) {
        RelokacioniZapis *rec = *it;
        advance(it, 1);
        delete rec;
    }
    delete records;

}

ostream& operator<<(ostream &os,const TabelaRelokacija &table) {
    os << "%" << table.section <<"\n";
    //os<<" OFFSET |   TYPE   | INDEX \n";
    list<RelokacioniZapis*>::iterator it = table.records->begin();
    for (int i = 0; i < table.records->size(); i++) {
        RelokacioniZapis *rec = *it;
        os<<*rec;
        advance(it, 1);
    }
    return os;
}