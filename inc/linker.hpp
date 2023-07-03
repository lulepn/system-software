#ifndef _linker_h_
#define _linker_h_
#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <cmath>
#include "../inc/sekcija.hpp"
#include "../inc/simbol.hpp"
#include "../inc/relokacioniZapis.hpp"
#include "../inc/tabelaRelZapisa.hpp"

using namespace std;

class Linker{
  static ofstream *file;
  static int size;
  static int lc;
  static int currSecInd;

  static Sekcija* currSection;
  static TabelaRelokacija* currRelTable;

  static list<Sekcija*> sectTable;
  static list<Simbol*> symTable;

public:
  static void setOutput(string outputName);
  static void link(int num);
  static void getData(string** input, int size);
  static void updateSymTable();
  static void resolveAdresses();

  static string removeSpaces(string str);

  static void printAll();

  static Sekcija* findSection(string name);
  static Simbol* findSymbol(string name);

  static int hexStringToInt(string str);
  static bool checkIfSymbolExists(string name, short type);
};

#endif