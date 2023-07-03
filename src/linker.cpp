#include "../inc/linker.hpp"

ofstream *Linker::file = NULL;
int Linker::lc = 0;
int Linker::currSecInd = 0;
int Linker::size = 0;

Sekcija* Linker::currSection = 0;
TabelaRelokacija* Linker::currRelTable = 0;

list<Sekcija*> Linker::sectTable = list<Sekcija*>();
list<Simbol*> Linker::symTable = list<Simbol*>();

void Linker::setOutput(string outputName) {
    file = new ofstream(outputName);
}

void Linker::link(int num) {
  
  //printAll();

  updateSymTable();
  resolveAdresses();
  
  list<Sekcija *>::iterator itSek = sectTable.begin();
  for (int i = 0; i < sectTable.size(); i++) {
      Sekcija *sek = *itSek;
      *file << *sek;
      advance(itSek,1);
  }

  file->close();

  //printAll();
}

void Linker::resolveAdresses() {
  RelokacioniZapis* tmp;

  while(currRelTable->records->size() != 0) {
    tmp = currRelTable->getRecord();
    //cout << *tmp << "\n";
    string symName = tmp->getSimbol();
    string sect = tmp->getSection();
    Sekcija* tmpSek = findSection(sect);
    Simbol* simb = findSymbol(symName);
    int offs = tmp->getOffset();
    int value, valueLow, valueHigh;
    value = simb->getValue();
    if(tmp->getType() == 0) {
      valueLow = value % 256;
      valueHigh = value / 256;
    } else {
      valueLow = value % 256;
      valueHigh = value / 256;
    }
    //cout << symName << " " << sect << " " << offs << " " << value << " " << valueLow << " " << valueHigh << "\n";
    list<int>::iterator it = tmpSek->data->begin();
    int i;
    for (i = 0; i < tmpSek->data->size(); i++) {
        if(offs == i) {
            *it = valueLow;
        }
        if(offs == i - 1) {
            *it = valueHigh;
            break;
        }

        advance(it, 1);
    }
  }
}

void Linker::updateSymTable() {

  list<Simbol*>::iterator iter = symTable.begin();
  list<Sekcija *>::iterator itSek = sectTable.begin();
  // azuriranje adresa
  iter = symTable.begin();
  for (int i = 0; i < symTable.size(); i++) {
      Simbol *sim = *iter;
      if(sim->getGlobal() != "extern") {
        int offs = sim->getValue();
        string tm = sim->getSecName();
        Sekcija* sekSimbola = findSection(tm);
        int adr = sekSimbola->getCursor();
        //cout << sim->getName() << " " << sim->getValue() << " " << tm << " " << sekSimbola->getName() << " " << adr << "\n";
        sim->setValue(offs + adr);
      }
      advance(iter, 1);
  }
}

void Linker::getData(string** input, int size) {
  int cnt = 0;
  int secLc = 0;
  int lastLc = 0;
  int temp;
  int strCnt;

  Sekcija* tempSek;
  string byte;
  string help = *input[0];

  help = help.substr(1,string::npos);
  help = removeSpaces(help);
  strCnt = help.find_first_of("L");
  help = help.substr(0,strCnt);
  if(findSection(help) == nullptr) {
    tempSek = new Sekcija(help);
    sectTable.push_back(tempSek);
    currSection = tempSek;
    currSection->setCursor(lc);
  }
  else {
    tempSek = findSection(help);
    currSection = tempSek;
    secLc = currSection->getSize();
    lastLc = secLc;
  }
  
  cnt ++;
  // sadrzaj sekcija dok ne dodje do tabele simbola
  while(1) {
    // dok ne dodje do sledece sekcije
    while(1) {
      help = *input[cnt];
      if((help == "$TABELA SIMBOLA") || (help[0] == '%'))
        break;
      help = removeSpaces(help);
      while(help.size() != 0) {
        byte = help.substr(0,2);
        temp = hexStringToInt(byte);
        help = help.substr(2,string::npos);
        currSection->addByte(temp);
        secLc++;
        lc++;
      }

      cnt++;
    }

    currSection->setSize(secLc);
    if(help == "$TABELA SIMBOLA")
      break;
    help = help.substr(1,string::npos);
    help = removeSpaces(help);
    strCnt = help.find_first_of("L");
    help = help.substr(0,strCnt);
    secLc = 0;
    currSection = new Sekcija(help);
    currSection->setCursor(lc);
    sectTable.push_back(currSection);
    cnt++;
  }

  //cout << "SEKCIJE DODATE\n";
  cnt = cnt + 2;

  while(1) {
    // tabela simbola dok ne dodje do relokacionih zapisa
    help = *input[cnt];
    help = removeSpaces(help);
    if(help == "&RELOKACIONIZAPISI")
      break;
    
    strCnt = help.find_first_of("|");
    string name = help.substr(0,strCnt);
    help = help.substr(strCnt + 1,string::npos);

    strCnt = help.find_first_of("|");
    string tmp = help.substr(0,strCnt);
    int type = 0;
    if(tmp == "section")
      type = 0;
    else 
      type = 1;
    help = help.substr(strCnt + 1,string::npos);

    strCnt = help.find_first_of("|");
    tmp = help.substr(0,strCnt);
    int val;
    if(tmp == "UND")
      val = -1;
    else
      val = hexStringToInt(tmp);
    help = help.substr(strCnt + 1,string::npos);

    strCnt = help.find_first_of("|");
    string section = help.substr(0,strCnt);
    help = help.substr(strCnt + 1,string::npos);

    strCnt = help.find_first_of("|");
    string scope = help.substr(0,strCnt);
    help = help.substr(strCnt + 1,string::npos);

    strCnt = help.find_first_of("|");
    string index = help.substr(0,strCnt);
    help = help.substr(strCnt + 1,string::npos);

    if(!checkIfSymbolExists(name,1) && !checkIfSymbolExists(name,0))
      symTable.push_back(new Simbol(name,type,val+lastLc,section,scope,1));
    else {
      if(scope != "extern" && type != 0) {
        findSymbol(name)->setGlobal("global");
        findSymbol(name)->setSecName(section);
        findSymbol(name)->setValue(val+lastLc);
      }
    }
    cnt++;
  
  }

  //cout << "TABELA SIMBOLA AZURIRANA\n";
  cnt++;
  help = *input[cnt];
  string currSec;
  if(currRelTable == 0)
    currRelTable = new TabelaRelokacija("TABELA RELOKACIJA");

  while(1) {
    // relokacioni zapisi dok ne dodje do kraja fajla
    if(help == "###")
      break;
    if(help[0] == '%') {
      help = help.substr(1,string::npos);
      currSec = help;
      cnt++;
      help = *input[cnt];
    } 
    else {
      help = removeSpaces(help);
      strCnt = help.find_first_of("|");
      string tmp = help.substr(0,strCnt);
      help = help.substr(strCnt+1,string::npos);
      int offs = hexStringToInt(tmp);

      strCnt = help.find_first_of("|");
      tmp = help.substr(0,strCnt);
      help = help.substr(strCnt+1,string::npos);
      int type;
      if(tmp == "R_386_32")
        type = 0;
      else
        type = 1;

      strCnt = help.find_first_of("|");
      tmp = help.substr(0,strCnt);
      help = help.substr(strCnt+1,string::npos);
      int index = hexStringToInt(tmp);
      currRelTable->addRecord(new RelokacioniZapis(offs+lastLc,type,index-1,currSec,help));

      cnt++;
      help = *input[cnt];
    }
  }

  //cout << "RELOKACIONI ZAPISI DODATI\n";
}

Sekcija* Linker::findSection(string name) {
  //cout << sectTable.size() << "\n";
  list<Sekcija *>::iterator itSek = sectTable.begin();
  for (int i = 0; i < sectTable.size(); i++) {
      Sekcija* sek = *itSek;
      //cout << sek->getName() << " " << name << "\n";
      if(sek->getName() == name) 
        return sek;
      advance(itSek,1);
  }
  return nullptr;
}

Simbol* Linker::findSymbol(string name) {
    Simbol* tmp;
    list<Simbol*>::iterator it = symTable.begin();
    for(int i = 0; i < symTable.size(); i++) {
        tmp = *it;
        advance(it,1);
        if(tmp->getName() == name)
            return tmp;
    }
    return 0;
}

string Linker::removeSpaces(string str){
    int index;
    string ret = str;
    while ((index = ret.find_first_of(" \t")) != string::npos){
        ret = ret.substr(0,index).append(ret.substr(index+1, string::npos));
    }
    return ret;
}

void Linker::printAll() {

  cout << "--------------------------- SEKCIJE ---------------------------\n";
  list<Sekcija *>::iterator itSek = sectTable.begin();
  for (int i = 0; i < sectTable.size(); i++) {
      Sekcija *sek = *itSek;
      cout << *sek << "\n";
      advance(itSek,1);
  }

  cout << "\n---------------------------Tabela simbola---------------------------\n";
  cout << "   NAME      |   TYPE   | VALUE | SECTION | SCOPE | INDEX \n";
  list<Simbol*>::iterator iter = symTable.begin();
  for (int i = 0; i < symTable.size(); i++) {
      Simbol *sim = *iter;
      cout << *sim;
      advance(iter, 1);
  }

  cout << *currRelTable;
}

int Linker::hexStringToInt(string str) {
    int ret = 0;
    int tmp;
    int power = 0;
    for(int i = 0; i < str.size(); i++) {
        if(str[str.size()-1-i] == '1')
            tmp = 1;
        else if(str[str.size()-1-i] == '2')
            tmp = 2;
        else if(str[str.size()-1-i] == '3')
            tmp = 3;
        else if(str[str.size()-1-i] == '4')
            tmp = 4;
        else if(str[str.size()-1-i] == '5')
            tmp = 5;
        else if(str[str.size()-1-i] == '6')
            tmp = 6;
        else if(str[str.size()-1-i] == '7')
            tmp = 7;
        else if(str[str.size()-1-i] == '8')
            tmp = 8;
        else if(str[str.size()-1-i] == '9')
            tmp = 9;
        else if(str[str.size()-1-i] == 'A' || str[str.size()-1-i] == 'a')
            tmp = 10;
        else if(str[str.size()-1-i] == 'B' || str[str.size()-1-i] == 'b')
            tmp = 11;
        else if(str[str.size()-1-i] == 'C' || str[str.size()-1-i] == 'c')
            tmp = 12;
        else if(str[str.size()-1-i] == 'D' || str[str.size()-1-i] == 'd')
            tmp = 13;
        else if(str[str.size()-1-i] == 'E' || str[str.size()-1-i] == 'e')
            tmp = 14;
        else if(str[str.size()-1-i] == 'F' || str[str.size()-1-i] == 'f')
            tmp = 15;
        else tmp = 0;
        ret = ret + tmp * pow(16,power);
        power++;
    }
    return ret;
}

bool Linker::checkIfSymbolExists(string name, short type) {
    Simbol* tmp;
    list<Simbol*>::iterator it = symTable.begin();
    for(int i = 0; i < symTable.size(); i++) {
        tmp = *it;
        advance(it, 1);
        if (tmp->getName() == name && tmp->getType() == type)
            return true;
    }
    return false;
}
