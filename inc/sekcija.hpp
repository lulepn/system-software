#ifndef _sekcija_h_
#define _sekcija_h_

#include <iostream>
#include <string>
#include <list>

using namespace std;

class Sekcija{
  string name;
  int cursor;
  int size;

public:
  list<int> *data;
  Sekcija(string namee) { name = namee; cursor = 0; data = new list<int>(); }
  string getName() { return name; }
  ~Sekcija() { delete data; }

  void setSize(int siz) { size = siz;}
  int getSize() { return size; }
  void setCursor(int cnt) { cursor = cnt; }
  int getCursor() { return cursor; }

  // dodavanje 1B/2B/4B u sekciju
  void addByte(int byte);
  void addWord(int word);
  void addInt(int intt);

  // upis
  friend ostream& operator<<(ostream &os, const Sekcija &sec);

};

#endif