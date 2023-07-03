#ifndef _emulator_h_
#define _emulator_h_

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <bitset>

using namespace std;

class Emulator{
  static ofstream *file;
  static short memory[65536];
  static int last;
  static int pc; // r7
  static int psw; 
  static int sp; // r6
  static int r[6];

public:
  static void Emulate();
  static void runProgram();
  static void getData(string** input, int size);
  static void setOutput(string outputName);

  static int hexStringToInt(string str);
  static string removeSpaces(string str);

  static string getAdrrMode(int byte);
  static string getAdrrModeJump(int byte);

  static int getHigherReg(int byte);
  static int getLowerReg(int byte);

  static void printMemory();
};

#endif