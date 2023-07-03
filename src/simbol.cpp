#include "../inc/simbol.hpp"
#include "../inc/asembler.hpp"

int Simbol::indexCounter = 0;

Simbol::Simbol(string nam, int sec, short typ, int val, short rwxx, string secN) {
  index = indexCounter; 
  indexCounter = indexCounter + 1;
  name = nam;
  secName = secN;
  value = val;
  type = typ;
  section = sec;
  global = "local";
  size = 0;
  rwx = rwxx; 
}

string Simbol::convertRWX(short rwx) {
  if(rwx == 0)
    return "0";
  if(rwx == 1)
    return "r  ";
  else if(rwx == 2)
    return " w ";
  else if(rwx == 3)
    return "  x";
  else if(rwx == 4)
    return "rw ";
  else if(rwx == 5)
    return "r x";
  else if(rwx == 6)
    return " wx";
  else 
    return "rwx";
}

ostream& operator<<(ostream& os, const Simbol& sym){

    os<<sym.name;
    if (sym.name.size()<13){
        for (int i=0; i<13-sym.name.size();i++)
        os<<" ";
    }

    os << "|" << (sym.type?"symbol":"section") ;
    if (sym.type == 0) os << "   ";
    else os<<"    ";

    if(sym.value != -1)
      os << "|" << hex<<sym.value;
    else
      os << "|" << "UND";
    if(sym.value == -1) os << "    ";
    else if (sym.value < 16) os << "      ";
    else if (sym.value < 256) os << "     ";
    else os << "    ";

    
    os << "|" << sym.secName;
    for(int i = 0; i < 9 - sym.secName.size(); i++)
    os << " ";
    

    if (sym.global == "global")
      os << "|" << "global ";
    else if (sym.global == "extern")
      os << "|" << "extern ";
    else os << "|local  ";

    os << "|" << sym.index;
    if (sym.index < 16) os << "      ";
    else if (sym.index < 256) os << "     ";
    else os << "    ";

    os << "\n";
    return os;
}