#include "../inc/tabelaObracanjaUnapred.hpp"

ostream& operator<<(ostream &os,const TabelaObracanjaUnapred &table) {
    os<<"\n-------" << " TABELA OBRACANJA UNAPRED " <<"-------\n";
    os<<"   INDEX     |   OFFSET   |  SECTION \n";
    list<Zapis*>::iterator it = table.records->begin();
    for (int i = 0; i < table.records->size(); i++) {
        Zapis *rec = *it;
        os<<*rec;
        advance(it, 1);
    }
    return os;
}