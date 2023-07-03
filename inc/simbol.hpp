#ifndef _simbol_h_
#define _simbol_h_

#include <iostream>
#include <string>

using namespace std;

class Simbol{
  static int indexCounter;
  string name;
  string secName;
  int section;
  int index;
  short type; // 0 za sekciju, 1 za simbol
  int size;
  int value;
  string global;
  short rwx; 

public:

  Simbol(string nam, int sec, short typ, int val, short rwxx, string secN);
  Simbol(string nam, short typ, int val, string sect, string scope, int linker) {
    name = nam; type = typ; value = val; secName = sect; global = scope; index = indexCounter; indexCounter = indexCounter + 1; section = -1;
  }

  // geteri
  string getName() { return name;}
  string getSecName() { return secName;}
  int getValue() { return value;}
  int getIndex() { return index;}
  int getSection() { return section;}
  int getSize() { return size;}
  short getType() { return type; }
  bool isGlobal() { if(global == "global") return true; else return false; }
  string getGlobal() { return global; }
  short getRwx() { return rwx; }
  int getIndexCnt() { return indexCounter; }
  
  // seteri
  void setRwx(short set) { if(set > 7) set = 7; if(set < 1) set = 1; rwx = set; }
  void setGlobal(string str) { global = str; }
  void setSize(int sizee) { size = sizee; }
  void setValue(int val) { value = val; }
  void setSection(int sec) { section = sec; }
  void setSecName(string name) { secName = name;}

  // iz short u string ( r = 1; w = 2; x = 3; rw = 4; rx = 5; wx = 6; rwx = 7; )
  static string convertRWX(short rwx);

  // upis
  friend ostream& operator<<(ostream& os, const Simbol& sym);

};


#endif