#ifndef _relokacioniZapis_h_
#define _relokacioniZapis_h_

#include <iostream>
#include <string>

using namespace std;

class RelokacioniZapis{
  int offset;
  short type; //0 - aps, 1 - rel
  int value;
  string section;
  string simbol;
public:
    RelokacioniZapis(int ofs, short typ, int val, string sim) { offset = ofs; type = typ; value = val; section = "###"; simbol = sim; }
    RelokacioniZapis(int ofs, short typ, int val, string sec, string sim) { offset = ofs; type = typ; value = val; section = sec; simbol = sim; }

    int getOffset() { return offset; }
    short getType() { return type; }
    int getValue() { return value; }
    string getSection() { return section;}
    string getSimbol() { return simbol; }

    friend ostream& operator<<(ostream& os, const RelokacioniZapis& rec);
};

#endif