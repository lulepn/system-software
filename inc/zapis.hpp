#ifndef _zapis_h_
#define _zapis_h_

#include <iostream>

using namespace std;

class Zapis {
  string name;
  int offset;
  int section;
  int relType; // 0 aps 1 pcrel
public:
  Zapis(string nam, int offs, int sec, int rel) { name = nam; offset = offs; section = sec; relType = rel;}
  string getName() { return name; }
  int getOffs() { return offset; }
  int getRel() { return relType; }

  friend ostream& operator<<(ostream& os, const Zapis& rec);
};

#endif