#ifndef _parser_h_
#define _parser_h_

#include <iostream>
#include <string.h>
#include <fstream>

using namespace std;

class Parser{
  ifstream file;
  string** lines;
  int size;

public:
  Parser(string fileName);
  ~Parser();
  string** getParsed();
  int getSize();

};

#endif