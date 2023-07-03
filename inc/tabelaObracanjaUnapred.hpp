#ifndef _tabelaObracanjaUnapred_h_
#define _tabelaObracanjaUnapred_h_

#include <iostream>
#include <list>
#include "../inc/zapis.hpp"

using namespace std;

class TabelaObracanjaUnapred {

public:
  list<Zapis*> *records;
  TabelaObracanjaUnapred() { records = new list<Zapis*>();}

  friend ostream& operator<<(ostream& os, const TabelaObracanjaUnapred& table);
};

#endif