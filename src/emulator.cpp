#include "../inc/emulator.hpp"

ofstream *Emulator::file = NULL;
int Emulator::last = 0;
int Emulator::pc = 0;
int Emulator::r[6] = {0};
int Emulator::sp = 0;
int Emulator::psw = 0;
short Emulator::memory[65536] = {0};

void Emulator::setOutput(string outputName) {
    file = new ofstream(outputName);
}

void Emulator::Emulate() {
  //printMemory();
  pc = memory[0] + memory[1]*256;
  runProgram();
  //printMemory();
}

void Emulator::runProgram() {
  int byte;
  int temp;
  int adr;
  int adrMode;
  int reg1, reg2;
  int dataHigh, dataLow;
  string instruction;
  string mode;
  bool endFlag = true;
  // izvrsavanje programa
  while(endFlag) {
    byte = memory[pc];
    //cout << pc << ":TRENUTNI BAJT " << byte << "\n";
    switch(byte) {
    case 0: // HALT // OK
      endFlag = false;
      //cout << "HALT\n";
      pc++;
      break;
    case 16:  // INT // OK
      pc++;
      byte = memory[pc];
      reg1 = getHigherReg(byte);
      pc++;
      sp = sp - 2;
      memory[sp+1] = pc / 256;
      memory[sp] = pc % 256;
      sp = sp - 2;
      memory[sp+1] = psw / 256;
      memory[sp] = psw % 256;
      adr = r[reg1] % 8;
      adr = adr * 2;
      if(adr == 8) adr = 6;
      else if(adr == 6) adr = 4;
      else if(adr == 4) adr = 2;
      pc = memory[adr];
      pc = pc + memory[adr+1] * 256;
      //cout << "INT " << adr << "\n";
      break;
    case 32:  // IRET // OK
      psw = memory[sp + 1] * 256;
      psw = psw + memory[sp];
      sp = sp + 2;
      pc = memory[sp + 1] * 256;
      pc = pc + memory[sp];
      sp = sp + 2;
      //cout << "IRET\n";
      break;
    case 48:  // CALL // OK
      pc++;
      byte = memory[pc];
      reg1 = getLowerReg(byte);
      pc++;
      adrMode = memory[pc];
      mode = getAdrrModeJump(adrMode);
      if(mode == "immed") {
        pc++;
        dataLow = memory[pc];
        pc++;
        dataHigh = memory[pc];
        sp = sp - 2;
        pc++;
        memory[sp+1] = pc / 256;
        memory[sp] = pc % 256;
        pc = dataLow + dataHigh*256;
      }
      else if(mode == "regdir") {
        sp = sp - 2;
        pc++;
        memory[sp + 1] = pc / 256;
        memory[sp] = pc % 256;
        //cout << reg1 << " " << r[reg1];
        pc = r[reg1];
      }
      else if(mode == "regind") {
        dataLow = memory[r[reg1]];
        dataHigh = memory[r[reg1] + 1];
        sp = sp - 2;
        pc++;
        memory[sp+1] = pc / 256;
        memory[sp] = pc % 256;
        pc = dataLow + dataHigh*256;
      }
      else if(mode == "regind pom") {
        pc++;
        dataLow = memory[pc];
        pc++;
        dataHigh = memory[pc];
        adr = dataLow + dataHigh*256 + r[reg1];
        sp = sp - 2;
        pc++;
        memory[sp + 1] = pc / 256;
        memory[sp] = pc % 256;
        dataLow = memory[adr];
        dataHigh = memory[adr+1];
        adr = dataLow + dataHigh*256;
        pc = adr;
      }
      else if(mode == "memdir") {
        pc++;
        dataLow = memory[pc];
        pc++;
        dataHigh = memory[pc];
        adr = dataHigh*256 + dataLow;
        sp = sp - 2;
        pc++;
        memory[sp+1] = pc / 256;
        memory[sp] = pc % 256;
        dataLow = memory[adr];
        dataHigh = memory[adr + 1];
        pc = dataHigh*256 + dataLow;
      }
      else if(mode == "pcrel") {
        pc++;
        dataLow = memory[pc];
        pc++;
        dataHigh = memory[pc];
        adr = dataHigh*256 + dataLow;
        sp = sp - 2;
        pc++;
        memory[sp+1] = pc / 256;
        memory[sp] = pc % 256;
        
        pc = dataHigh*256 + dataLow;
      }
      //cout << "CALL " << mode << "\n";
      break;
    case 64:  // RET // OK
      pc = memory[sp+1] * 256;
      pc = pc + memory[sp];
      sp = sp + 2;
      //cout << "RET\n";
      break;
    case 80:  // JMP // OK
      pc++;
      byte = memory[pc];
      reg1 = getLowerReg(byte);
      pc++;
      adrMode = memory[pc];
      mode = getAdrrModeJump(adrMode);
      if(mode == "immed") {
        pc++;
        dataLow = memory[pc];
        pc++;
        dataHigh = memory[pc];
        pc = dataLow + dataHigh*256;
      }
      else if(mode == "regdir") {
        pc = r[reg1];
      }
      else if(mode == "regind") {
        dataLow = memory[r[reg1]];
        dataHigh = memory[r[reg1] + 1];
        pc = dataLow + dataHigh*256;
      }
      else if(mode == "regind pom") {
        pc++;
        dataLow = memory[pc];
        pc++;
        dataHigh = memory[pc];
        adr = dataLow + dataHigh*256 + r[reg1];
        pc = adr;
      }
      else if(mode == "memdir") {
        pc++;
        dataLow = memory[pc];
        pc++;
        dataHigh = memory[pc];
        adr = dataHigh*256 + dataLow;
        dataLow = memory[adr];
        dataHigh = memory[adr + 1];
        pc = dataHigh*256 + dataLow;
      }
      else if(mode == "pcrel") {
        pc++;
        dataLow = memory[pc];
        pc++;
        dataHigh = memory[pc];
        adr = dataHigh*256 + dataLow;
        dataLow = memory[pc + adr];
        dataHigh = memory[pc + 1 + adr];
        pc = dataHigh*256 + dataLow;
      }
      //cout << "JMP " << mode << "\n";
      break;
    case 81:  // JEQ // OK
      pc++;
      byte = memory[pc];
      reg1 = getLowerReg(byte);
      pc++;
      adrMode = memory[pc];
      mode = getAdrrModeJump(adrMode);
      byte = 1 & psw;
      if(byte == 1) {
        if(mode == "immed") {
          pc++;
          dataLow = memory[pc];
          pc++;
          dataHigh = memory[pc];
          pc = dataLow + dataHigh*256;
        }
        else if(mode == "regdir") {
          pc = r[reg1];
        }
        else if(mode == "regind") {
          dataLow = memory[r[reg1]];
          dataHigh = memory[r[reg1] + 1];
          pc = dataLow + dataHigh*256;
        }
        else if(mode == "regind pom") {
          pc++;
          dataLow = memory[pc];
          pc++;
          dataHigh = memory[pc];
          adr = dataLow + dataHigh*256 + r[reg1];
          pc = adr;
        }
        else if(mode == "memdir") {
          pc++;
          dataLow = memory[pc];
          pc++;
          dataHigh = memory[pc];
          adr = dataHigh*256 + dataLow;
          dataLow = memory[adr];
          dataHigh = memory[adr + 1];
          pc = dataHigh*256 + dataLow;
        }
        else if(mode == "pcrel") {
          pc++;
          dataLow = memory[pc];
          pc++;
          dataHigh = memory[pc];
          adr = dataHigh*256 + dataLow;
          dataLow = memory[pc + adr];
          dataHigh = memory[pc + 1 + adr];
          pc = dataHigh*256 + dataLow;
        }
      } 
      else {
        if(mode == "regdir" || mode == "regind")
          pc++;
        else {
          pc = pc + 3;
        }
      }
      //cout << "JEQ" << mode << "\n";
      break;
    case 82:  // JNE // OK
      pc++;
      byte = memory[pc];
      reg1 = getLowerReg(byte);
      pc++;
      adrMode = memory[pc];
      mode = getAdrrModeJump(adrMode);
      byte = 1 & psw;
      if(byte == 0) {
        if(mode == "immed") {
          pc++;
          dataLow = memory[pc];
          pc++;
          dataHigh = memory[pc];
          pc = dataLow + dataHigh*256;
        }
        else if(mode == "regdir") {
          pc = r[reg1];
        }
        else if(mode == "regind") {
          dataLow = memory[r[reg1]];
          dataHigh = memory[r[reg1] + 1];
          pc = dataLow + dataHigh*256;
        }
        else if(mode == "regind pom") {
          pc++;
          dataLow = memory[pc];
          pc++;
          dataHigh = memory[pc];
          adr = dataLow + dataHigh*256 + r[reg1];
          pc = adr;
        }
        else if(mode == "memdir") {
          pc++;
          dataLow = memory[pc];
          pc++;
          dataHigh = memory[pc];
          adr = dataHigh*256 + dataLow;
          dataLow = memory[adr];
          dataHigh = memory[adr + 1];
          pc = dataHigh*256 + dataLow;
        }
        else if(mode == "pcrel") {
          pc++;
          dataLow = memory[pc];
          pc++;
          dataHigh = memory[pc];
          adr = dataHigh*256 + dataLow;
          dataLow = memory[pc + adr];
          dataHigh = memory[pc + 1 + adr];
          pc = dataHigh*256 + dataLow;
        }
      } 
      else {
        if(mode == "regdir" || mode == "regind")
          pc++;
        else {
          pc = pc + 3;
        }
      }
      //cout << "JNE" << mode << "\n";
      break;
    case 83:  // JGT
      break;
    case 176: // PUSH // OK
      pc++;
      byte = memory[pc];
      reg1 = getLowerReg(byte);
      temp = r[reg1];
      //cout << r[reg1] << " " << temp << endl;
      //cout << "PUSH NA ADRESU :" << sp - 2 << " r" << reg1 << " = " << (temp/256) << (temp%256) << "\n";
      sp = sp - 2;
      memory[sp+1] = temp / 256;
      memory[sp] = temp % 256;
      pc = pc + 2;
      break;
    case 160: // POP // OK
      pc++;
      byte = memory[pc];
      reg1 = getHigherReg(byte);
      r[reg1] = memory[sp+1] * 256;
      r[reg1] = r[reg1] + memory[sp];
      //cout << "POP SA ADRESE :" << sp << " r" << reg1 << " = " << r[reg1] << "\n";
      sp = sp + 2;
      pc = pc + 2;
      break;
    case 96:  // XCHG // OK
      pc++;
      byte = memory[pc];
      reg1 = getHigherReg(byte);
      reg2 = getLowerReg(byte);
      temp = r[reg1];
      r[reg1] = r[reg2];
      r[reg2] = temp;
      pc++;
      break;
    case 112: // ADD // OK
      pc++;
      byte = memory[pc];
      reg1 = getHigherReg(byte);
      reg2 = getLowerReg(byte);
      r[reg1] = r[reg1] + r[reg2];
      pc++;
      //cout << "ADD " << reg1 << " " << reg2 << endl;
      break;
    case 113: // SUB // OK
      pc++;
      byte = memory[pc];
      reg1 = getHigherReg(byte);
      reg2 = getLowerReg(byte);
      r[reg1] = r[reg1] - r[reg2];
      pc++;
      //cout << "SUB " << reg1 << " " << reg2 << endl;
      break;
    case 114: // MUL // OK
      pc++;
      byte = memory[pc];
      reg1 = getHigherReg(byte);
      reg2 = getLowerReg(byte);
      r[reg1] = r[reg1] * r[reg2];
      pc++;
      //cout << "MUL " << reg1 << " " << reg2 << endl;
      break;
    case 115: // DIV // OK
      pc++;
      byte = memory[pc];
      reg1 = getHigherReg(byte);
      reg2 = getLowerReg(byte);
      if(r[reg2] != 0) 
        r[reg1] = r[reg1] / r[reg2];
      pc++;
      //cout << "DIV " << reg1 << " " << reg2 << endl;
      break;
    case 116: // CMP // OK
      pc++;
      byte = memory[pc];
      reg1 = getHigherReg(byte);
      reg2 = getLowerReg(byte);
      temp = r[reg1] - r[reg2];
      if(temp == 0) // ZERO FLAG
        psw = psw | 1;
      if(temp > 32767 || temp < -32768) // OVERFLOW FLAG
        psw = psw | 2;
      if(r[reg2] > r[reg1]) // CARRY FLAG
        psw = psw | 4;
      if(temp < 0)  // NEGATIVE FLAG
        psw = psw | 8;
      pc++;
      break;
    case 128: // NOT // OK
      pc++;
      byte = memory[pc];
      reg1 = getHigherReg(byte);
      r[reg1] = ~r[reg1];
      pc++;
      break;
    case 129: // AND // OK
      pc++;
      byte = memory[pc];
      reg1 = getHigherReg(byte);
      reg2 = getLowerReg(byte);
      r[reg1] = r[reg1] & r[reg2];
      pc++;
      break;
    case 130: // OR // OK
      pc++;
      byte = memory[pc];
      reg1 = getHigherReg(byte);
      reg2 = getLowerReg(byte);
      r[reg1] = r[reg1] | r[reg2];
      pc++;
      break;
    case 131: // XOR // OK
      pc++;
      byte = memory[pc];
      reg1 = getHigherReg(byte);
      reg2 = getLowerReg(byte);
      r[reg1] = r[reg1] ^ r[reg2];
      pc++;
      break;
    case 132: // TEST // OK
      pc++;
      byte = memory[pc];
      reg1 = getHigherReg(byte);
      reg2 = getLowerReg(byte);
      temp = r[reg1] & r[reg2];
      if(temp == 0) // ZERO FLAG
        psw = psw | 1;
      if(temp < 0)  // NEGATIVE FLAG
        psw = psw | 8;
      pc++;
      break;
    case 148: // SHL
      break;
    case 149: // SHR
      break;
    case 164: // LDR // OK
      pc++;
      byte = memory[pc];
      reg1 = getHigherReg(byte); // odredisni
      reg2 = getLowerReg(byte); // izvorisni
      pc++;
      adrMode = memory[pc];
      mode = getAdrrMode(adrMode);
      if (mode == "immed") {
        pc++;
        dataLow = memory[pc];
        pc++;
        dataHigh = memory[pc];
        if(reg1 != 6)
          r[reg1] = dataHigh * 256 + dataLow;
        else {
          sp = dataHigh * 256 + dataLow;
          //cout << sp << endl;
        }
      } 
      else if(mode == "regdir") {
        r[reg1] = r[reg2];
      } 
      else if(mode == "regind") {
        dataLow = memory[r[reg2]];
        dataHigh = memory[r[reg2]+1];
        r[reg1] = dataHigh*256 + dataLow;
      } 
      else if(mode == "pcrel") {
        pc++;
        dataLow = memory[pc];
        pc++;
        dataHigh = memory[pc];
        adr = dataHigh * 256 + dataLow;
        
        if(reg2 == 7) {
          r[reg1] = memory[pc + 1 + adr];
          r[reg1] = r[reg1] + memory[pc + 2 + adr] * 256;
        }
        else if(reg2 == 6) {
          //cout << "IZ REGISTRA :" << "SP" << " " << sp << endl;
          r[reg1] = memory[sp + adr];
          r[reg1] = r[reg1] + memory[sp + 1 + adr] * 256;
        }
        else
          r[reg1] = memory[r[reg2] + adr];
          r[reg1] = r[reg1] + memory[r[reg2] + adr + 1] * 256;
      } 
      else if(mode == "memdir") {
        pc++;
        dataLow = memory[pc];
        pc++;
        dataHigh = memory[pc];
        adr = dataHigh * 256 + dataLow;
        r[reg1] = memory[adr];
        r[reg1] = r[reg1] + memory[adr + 1] * 256;
      }
      pc++;
      //cout << "LDR r" << reg1 << " " << mode << "\n";
      break;
    case 180: // STR // OK
      pc++;
      byte = memory[pc];
      reg1 = getHigherReg(byte); // izvorisni
      reg2 = getLowerReg(byte); // odredisni
      pc++;
      adrMode = memory[pc];
      mode = getAdrrMode(adrMode);
      if (mode == "immed") {
        cout << "GRESKA : STORE IMMED\n";
        exit(-1);
      } 
      else if(mode == "regdir") {
        r[reg2] = r[reg1];

      } 
      else if(mode == "regind") {
        memory[r[reg2]] = r[reg1] % 256;
        memory[r[reg2] + 1] = r[reg1] / 256;
        //cout << "STORE NA " << r[reg2] << " " << memory[r[reg2] + 1] << memory[r[reg2]] << endl;
      } 
      else if(mode == "pcrel") {
        pc++;
        dataLow = memory[pc];
        pc++;
        dataHigh = memory[pc];
        adr = dataHigh * 256 + dataLow;
        if(reg2 == 7) {
          memory[pc + 1 + adr] = r[reg1] % 256;
          memory[pc + 2 + adr] = r[reg1] / 256;
        }
        else {
          memory[r[reg2] + adr] = r[reg1];
          memory[r[reg2] + adr + 1] = r[reg1];
        }
      } 
      else if(mode == "memdir") {
        pc++;
        dataLow = memory[pc];
        pc++;
        dataHigh = memory[pc];
        adr = dataHigh * 256 + dataLow;
        //cout << "ADRESA MEMDIR : "<< adr << endl;
        //cout << "VREDNOST : " << r[reg1] << endl;
        memory[adr] = r[reg1] % 256;
        memory[adr + 1] = r[reg1] / 256;
      }
      //cout << "STR r" << reg1 << " " << mode << endl;
      pc++;
      break;
    default:
      exit(1);
    }
  }
  pc = pc + 8;
  
  cout << "------------------------------------------------\n";
  cout << "Emulated processor executed halt instruction\n";
  cout << "Emulated processor state: psw=0b" << bitset<16>(psw) << "\n";
  if(r[0] < 16)
    cout << "r0=0x000" << hex<<r[0];
  else if(r[0] < 256)
    cout << "r0=0x00" << hex<<r[0];
  else if(r[0] < 3840)
    cout << "r0=0x0" << hex<<r[0];
  else 
    cout << "r0=0x" <<hex<<r[0];
  
  if(r[1] < 16)
    cout << "    r1=0x000" << hex<<r[1];
  else if(r[1] < 256)
    cout << "    r1=0x00" << hex<<r[1];
  else if(r[1] < 3840)
    cout << "    r1=0x0" << hex<<r[1];
  else 
    cout << "    r1=0x" <<hex<<r[1];

  if(r[2] < 16)
    cout << "    r2=0x000" << hex<<r[2];
  else if(r[2] < 256)
    cout << "    r2=0x00" << hex<<r[2];
  else if(r[2] < 3840)
    cout << "    r2=0x0" << hex<<r[2];
  else 
    cout << "    r2=0x" <<hex<<r[2];

  if(r[3] < 16)
    cout << "    r3=0x000" << hex<<r[3] << "\n";
  else if(r[3] < 256)
    cout << "    r3=0x00" << hex<<r[3] << "\n";
  else if(r[3] < 3840)
    cout << "    r3=0x0" << hex<<r[3] << "\n";
  else 
    cout << "    r3=0x" <<hex<<r[3] << "\n";


  if(r[4] < 16)
    cout << "r4=0x000" << hex<<r[4];
  else if(r[4] < 256)
    cout << "    r4=0x00" << hex<<r[4];
  else if(r[4] < 3840)
    cout << "    r4=0x0" << hex<<r[4];
  else 
    cout << "    r4=0x" <<hex<<r[4];


  if(r[5] < 16)
    cout << "    r5=0x000" << hex<<r[5];
  else if(r[5] < 256)
    cout << "    r5=0x00" << hex<<r[5];
  else if(r[5] < 3840)
    cout << "    r5=0x0" << hex<<r[5];
  else 
    cout << "    r5=0x" <<hex<<r[5];

  if(sp < 16)
    cout << "    r6=0x000" << hex<<sp;
  else if(sp < 256)
    cout << "    r6=0x00" << hex<<sp;
  else if(sp < 3840)
    cout << "    r6=0x0" << hex<<sp;
  else 
    cout << "    r6=0x" <<hex<<sp;

  if(pc < 16)
    cout << "    r7=0x000" << hex<<pc << "\n";
  else if(pc < 256)
    cout << "    r7=0x00" << hex<<pc << "\n";
  else if(pc < 3840)
    cout << "    r7=0x0" << hex<<pc << "\n";
  else 
    cout << "    r7=0x" <<hex<<pc << "\n";
}

string Emulator::getAdrrMode(int byte) {
  switch(byte) {
    case 0:
      return "immed";
    case 1:
      return "regdir";
    case 2:
      return "regind";
    case 3:
      return "pcrel";
    case 4: 
      return "memdir";
  }
  return "error";
}

string Emulator::getAdrrModeJump(int byte) {
  switch(byte) {
    case 0:
      return "immed";
    case 1:
      return "regdir";
    case 2:
      return "regind";
    case 3:
      return "regind pom";
    case 4:
      return "memdir";
    case 5: 
      return "pcrel";
  }
  return "error";
}

int Emulator::getHigherReg(int byte) {
  int ret = byte / 16;
  return ret;
}

int Emulator::getLowerReg(int byte) {
  int ret = byte % 16;
  return ret;
}

void Emulator::getData(string** input, int size) {
  int cnt = 0;
  int byte;
  string line, temp;
  
  while(cnt < size) {
      line = *input[cnt];
      line = removeSpaces(line);
      if(line[0] != '%') {
        while(line.size() != 0) {
          temp = line.substr(0,2);
          byte = hexStringToInt(temp);
          line = line.substr(2,string::npos);
          memory[last] = byte;
          last++;
        }
      }
      cnt++;
    }
}

int Emulator::hexStringToInt(string str) {
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

string Emulator::removeSpaces(string str){
    int index;
    string ret = str;
    while ((index = ret.find_first_of(" \t")) != string::npos){
        ret = ret.substr(0,index).append(ret.substr(index+1, string::npos));
    }
    return ret;
}

void Emulator::printMemory() {
  cout << "MEMORIJA :\n";
  for(int i = 0; i < last; i++) {
    if(i % 8 == 0) {
      cout << "\n";
      if (i < 16)
        cout << "000" << hex<<i << " : ";
      else if (i < 256)
        cout << "00" << hex<<i << " : ";
      else 
        cout << "0" << hex<<i << " : ";
    }
    if(memory[i] < 16)
      cout << "0" << hex<<memory[i] << " ";
    else {
      cout << hex<<memory[i] << " ";
    }
  }
  cout << "\n";
  cout << "MEMORIJA ISPISANA\n";
}