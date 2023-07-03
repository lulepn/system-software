#ifndef _asembler_h_
#define _asembler_h_

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <iterator>
#include <cmath>
#include "../inc/simbol.hpp"
#include "../inc/sekcija.hpp"
#include "../inc/tabelaRelZapisa.hpp"
#include "../inc/tabelaObracanjaUnapred.hpp"

using namespace std;

enum Directives{
  GLOBAL,
  EXTERN,
  SECTION,
  WORD,
  SKIP,
  END,
  DIRECTIVE_NOT_FOUND
};

enum Instructions{
  HALT,
  INT,
  IRET,
  CALL,
  RET,
  JMP,
  JEQ,
  JNE,
  JGT,
  PUSH,
  POP,
  XCHG,
  ADD,
  SUB,
  MUL,
  DIV,
  CMP,
  NOT,
  AND,
  OR,
  XOR,
  TEST,
  SHL,
  SHR,
  LDR,
  STR,
  ILLEGAL_INSTRUCTION
};

class Asembler{
    static ofstream *file;
    static int lc;
    static bool end;
    static int currSecInd;
    static Sekcija* currSection;
    static TabelaRelokacija* currRelTable;
    static TabelaObracanjaUnapred* tabelaObracanja;

    // tabele
    static list<Simbol*> symTable;
    static list<Sekcija*> sectTable;
    static list<TabelaRelokacija*> relTableList;
    

public:

    static void setOutput(string outputName);

    // ispisi
    static void printSymbolTable();
    static void printSections();

    // formatiranje stringova
    static string removeSpaces(string str);
    static string removeComments(string str);

    // string u enum
    static Instructions hashInstruction(string str);
    static Directives hashDirective(string str);

    // glavne funkcije
    static void assemble(string** input, int size);

    static void checkIfLabel(string line);
    static void checkIfDirective(string line);
    static void checkIfInstruction(string line);

    // pomocne funkcije za simbole
    static bool checkIfSymbolExists(string name, short type);
    static Simbol* findSymbol(string name);
    static Simbol* findSymbolByIndex(int index);

    static int checkAdressingMode(string str);
    static int checkAdressingModeJump(string str);
    static void writeOperandJump(string str, int adrMode);

    static int getRegisterIndex(string str);

    static int hexStringToInt(string hex);

    static void resolveSymbols();

};

#endif