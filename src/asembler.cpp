#include "../inc/asembler.hpp"

ofstream *Asembler::file = NULL;
int Asembler::lc = 0;
bool Asembler::end = false;
int Asembler::currSecInd = 0;
Sekcija* Asembler::currSection = 0;
TabelaRelokacija* Asembler::currRelTable = 0;
TabelaObracanjaUnapred* Asembler::tabelaObracanja = 0;

list<Simbol*> Asembler::symTable = list<Simbol*>();
list<Sekcija*> Asembler::sectTable = list<Sekcija*>();
list<TabelaRelokacija*> Asembler::relTableList = list<TabelaRelokacija*>();

// ---------------- GLAVNA FUNKCIJA ASEMBLERA ---------------- //

void Asembler::assemble(string** input, int size) {

    symTable.push_back(new Simbol("UND", 0, 0, 0, 0, "###"));
    tabelaObracanja = new TabelaObracanjaUnapred();

    for (int i = 0; i < size; i++) {

        if(end)
            break;
        string line = *input[i];
        if (!line.empty()) {
            // brisanje space-ova ispred naredbe/instrukcije/labele
            int startIndex = line.find_first_not_of(" \t");
            line = line.substr(startIndex);
            
            // brisanje komentara
            line = removeComments(line);

            if(!line.empty()) {
                checkIfLabel(line);
                checkIfDirective(line);
                checkIfInstruction(line);
            }

            //cout << line << "\n";
        }
    }

    resolveSymbols();

    //printSections();
    //printSymbolTable();
    
    
    list<Simbol*>::iterator it = symTable.begin();
    list<Sekcija*>::iterator itSek = sectTable.begin();
    list<TabelaRelokacija*>::iterator itRel = relTableList.begin();

    // upis u objektni fajl

    for(int i = 0; i < sectTable.size(); i++) {
        Sekcija *sek = *itSek;
        *file << *sek;
        advance(itSek,1);
    }
    
    *file << "$TABELA SIMBOLA\n";
    for(int i = 0; i < symTable.size(); i++) {
        Simbol *sim = *it;
        *file << *sim;
        advance(it,1);
    }
    *file << "&RELOKACIONI ZAPISI\n";
    for(int i = 0; i < relTableList.size(); i++) {
        TabelaRelokacija *tab = *itRel;
        *file << *tab;
        advance(itRel,1);
    }
    *file << "###";

    // brisanje tabela simbola, sekcija i relokacionih zapisa, zatvaranje fajla
    it = symTable.begin();
    for (int i = 0; i < symTable.size();i++) {
        Simbol *tmp = *it;
        delete tmp;
        advance(it,1);
    }
    itSek = sectTable.begin();
    for (int i = 0; i < sectTable.size();i++) {
        Sekcija *tmp = *itSek;
        delete tmp;
        advance(itSek,1);
    }
    
    file->close();
    delete file;

}

void Asembler::checkIfLabel(string line) {
    int cnt;
    string label;
    if((cnt = line.find_first_of(":")) != string::npos) {
        label = line.substr(0,cnt);
        line = line.substr(cnt + 1, string::npos);
        if(!checkIfSymbolExists(label, 1))
            symTable.push_back(new Simbol(label, currSecInd, 1, lc, 0, currSection->getName()));
        else {
            if(!findSymbol(label)->isGlobal()) {
                cout << "DVOSTRUKA DEFINICIJA LABELE\n";
                exit(1);
            }   
            if(findSymbol(label)->getGlobal() == "extern") {
                cout << "DEFINICIJA LABELE IZ DRUGOG FAJLA\n";
                exit(2);
            }

            findSymbol(label)->setValue(lc);
            findSymbol(label)->setSection(currSecInd);
            findSymbol(label)->setSecName(currSection->getName());
        }

        if(line.size() > 0) {
            line = removeSpaces(line);
        checkIfDirective(line);
        checkIfInstruction(line);
        }
    }
}

void Asembler::checkIfDirective(string line) {
    
    if (line[0] != '.') return;
    int cnt;
    int help;
    Zapis* zapis;
    RelokacioniZapis* record;
    string directive;
    if((cnt = line.find_first_of(" ")) != string::npos) {
        directive = line.substr(0,cnt);
        line = line.substr(cnt + 1, string::npos);
        if(line[0] == ' ') {
            int cnt2 = line.find_first_not_of(" ");
            line = line.substr(cnt2, string::npos);
        }
    }
    else {
        directive = line;
        if ((cnt = line.find_first_of("\r")) != string::npos)
            directive = line.substr(0,cnt);
    }
    
    Simbol* tmpSim;
    int cntr;
    string tmpString;

    switch(hashDirective(directive)) {
        case GLOBAL:
            line = removeSpaces(line);
            line = line + ',';

            cntr = line.find_first_of(", \0");
            tmpString = line.substr(0,cntr);
            line = line.substr(cntr + 1, string::npos);
            if (checkIfSymbolExists(tmpString, 1)) {
                findSymbol(tmpString)->setGlobal("global");
            }
            else {
                Simbol* temp = new Simbol(tmpString, 0, 1, -1, 0,"###");
                symTable.push_back(temp);
                temp->setGlobal("global");
            }

            while((cntr = line.find_first_of(", \0")) != string::npos) {
                tmpString = line.substr(0, cntr);
                line = line.substr(cntr + 1, string::npos);

                if (checkIfSymbolExists(tmpString, 1)) {
                    findSymbol(tmpString)->setGlobal("global");
                }
                else {
                    Simbol* temp = new Simbol(tmpString, 0, 1, -1, 0, "###");
                    symTable.push_back(temp);
                    temp->setGlobal("global");
                }
            }

        break;
        case EXTERN:
            line = removeSpaces(line);
            line = line + ',';

            cntr = line.find_first_of(", \0");
            tmpString = line.substr(0,cntr);
            line = line.substr(cntr + 1, string::npos);
            if (checkIfSymbolExists(tmpString, 1)) {
                findSymbol(tmpString)->setGlobal("extern");
            }
            else {
                Simbol* temp = new Simbol(tmpString, currSecInd, 1, -1, 0, "###");
                symTable.push_back(temp);
                temp->setGlobal("extern");
            }

            while((cntr = line.find_first_of(", \0")) != string::npos) {
                tmpString = line.substr(0, cntr);
                line = line.substr(cntr + 1, string::npos);

                if (checkIfSymbolExists(tmpString, 1)) {
                    findSymbol(tmpString)->setGlobal("extern");
                }
                else {
                    Simbol* temp = new Simbol(tmpString, currSecInd, 1, -1, 0, "###");
                    symTable.push_back(temp);
                    temp->setGlobal("extern");
                }
            }
        break;
        case SECTION:
           line = removeSpaces(line);
           if(!checkIfSymbolExists(line,0)) {
               // ubacivanje nove sekcije u tabele
               Simbol* temp = new Simbol(line, 0, 0, 0, 0, line);
               symTable.push_back(temp);
               TabelaRelokacija* tmp = new TabelaRelokacija(line);
               relTableList.push_back(tmp);
               currRelTable = tmp;

               if(currSection) {
                    currSection->setCursor(lc);
                    findSymbol(currSection->getName())->setSize(lc);
               }

               currSection = new Sekcija(line);
               sectTable.push_back(currSection);
               
               lc = 0;
               currSecInd = temp->getIndex();
           }
           /*else {

           }*/
                
           
        break;
        case WORD:
            line = removeSpaces(line);
            line = line + ',';
            cntr = line.find_first_of(", \0");
            tmpString = line.substr(0,cntr);
            
            if((help = tmpString.find_first_not_of("0123456789ABCDEFx")) != string::npos) {
                // simbol
                if(checkIfSymbolExists(tmpString,1)) {
                    if(findSymbol(tmpString)->getValue() != -1) {
                        // simbol definisan
                        record = new RelokacioniZapis(lc, 0, findSymbol(tmpString)->getIndex(), tmpString);
                        currRelTable->addRecord(record); 
                        currSection->addWord(0); 
                    } else {
                        // deklarisan al nedefinisan
                        zapis = new Zapis(findSymbol(tmpString)->getName(), lc, currSecInd, 0);
                        tabelaObracanja->records->push_back(zapis);
                        currSection->addWord(0); 
                    }
                } else {
                    // nije ni deklarisan
                    zapis = new Zapis(findSymbol(tmpString)->getName(), lc, currSecInd, 0);
                    tabelaObracanja->records->push_back(zapis);
                    currSection->addWord(0); 
                }
            } else {
                // literal
                cntr = stoi(tmpString,nullptr,0);
                currSection->addWord(cntr); 
            }

            line = line.substr(cntr + 1, string::npos);
            lc = lc + 2;

            while((cntr = line.find_first_of(", \0")) != string::npos && line != ",") {
                tmpString = line.substr(0, cntr);
                
                line = line.substr(cntr + 1, string::npos);
                
                if((help = tmpString.find_first_of("0123456789")) != string::npos) {
                    cntr = stoi(tmpString,nullptr,0);
                    currSection->addWord(cntr);
                } else {
                    if(findSymbol(tmpString)->getGlobal() == "extern")
                        currSection->addWord(0);
                }
                
                lc = lc + 2;
                
            }
        break;

        case SKIP:
            cntr = stoi(line,nullptr,0);
            for(int j = 0; j < cntr; j++) {
                //currSection->addByte(0); 
                //lc = lc + 1;
            }
        break;

        case END:
            if(currSection) {
                currSection->setCursor(lc);
                findSymbol(currSection->getName())->setSize(lc);
            }
            end = true;
        break;

        case DIRECTIVE_NOT_FOUND:
            cout << "GRESKA, NEPOSTOJECA DIREKTIVA\n";
            exit(-1);
        break;
    }
}

void Asembler::writeOperandJump(string line, int adrMode) {
    int byte;
    int help;
    int ind, ind2;
    int cnt;
    string temp;
    Simbol* simb;
    RelokacioniZapis* record;
    Zapis* zapis;
    line = removeSpaces(line);

    if(adrMode == 0) {
        // immed literal
        byte = 240;
        currSection->addByte(byte);
        byte = 0;
        currSection->addByte(byte);
        if(line[0] == '0' && line[1] == 'x') {
            line = line.substr(2,string::npos);
            help = hexStringToInt(line);
        }
        else {
            help = stoi(line,nullptr,0);
        }
        currSection->addWord(help);
        lc = lc + 5;
    }
    else if (adrMode == 1) {
        // immed simbol
        byte = 240;
        currSection->addByte(byte);
        byte = 0;
        currSection->addByte(byte);

        if (checkIfSymbolExists(line,1)) {
            simb = findSymbol(line);
            if(simb->getValue() != -1) {
                // simbol postoji i definisan je
                record = new RelokacioniZapis(lc + 3, 0, simb->getIndex(), simb->getName());
                currRelTable->addRecord(record);
            }
            else {
                // deklarisan ali nije definisan
                zapis = new Zapis(line, lc + 3, currSecInd, 0);
                tabelaObracanja->records->push_back(zapis);
            }
        } else {
            // nije ni deklarisan
            zapis = new Zapis(line, lc + 3, currSecInd, 0);
            tabelaObracanja->records->push_back(zapis);
        }
        currSection->addWord(0);
        lc = lc + 5;
    }
    else if (adrMode == 2) {
        // pcrel
        byte = 247;
        currSection->addByte(byte);
        byte = 5;
        currSection->addByte(byte);

        line = line.substr(1,string::npos);

        if (checkIfSymbolExists(line,1)) {
            simb = findSymbol(line);
            if(simb->getValue() != -1) {
                // simbol postoji i definisan je
                if(simb->getSection() != currSecInd) {
                    // nije u istoj sekciji
                    record = new RelokacioniZapis(lc + 3, 1, simb->getIndex(), simb->getName());
                    currRelTable->addRecord(record);
                    currSection->addWord(0);
                } else {
                    byte = simb->getValue() - lc - 5;
                    currSection->addWord(byte);
                }
            } else {
                // simbol deklarisan ali nije definisan
                zapis = new Zapis(simb->getName(), lc + 3, currSecInd, 1);
                tabelaObracanja->records->push_back(zapis);
                currSection->addWord(0);
            }
        } else {
            // nije ni deklarisan
            zapis = new Zapis(line, lc + 3, currSecInd, 1);
            tabelaObracanja->records->push_back(zapis);
            currSection->addWord(0);
        }
        lc = lc + 5;
    } 
    else if (adrMode == 3) {
        // memdir literal
        byte = 240;
        currSection->addByte(byte);
        byte = 4;
        currSection->addByte(byte);

        line = line.substr(1,string::npos);

        if(line[0] == '0' && line[1] == 'x') {
            line = line.substr(2,string::npos);
            help = hexStringToInt(line);
        }
        else {
            help = stoi(line,nullptr,0);
        }
        currSection->addWord(help);
        lc = lc + 5;
    }
    else if (adrMode == 4) {
        // memdir simbol
        byte = 240;
        currSection->addByte(byte);
        byte = 4;
        currSection->addByte(byte);

        line = line.substr(1,string::npos);

        if(checkIfSymbolExists(line, 1)) {
            simb = findSymbol(line);
            if(simb->getValue() != -1) {
                // simbol definisan
                record = new RelokacioniZapis(lc + 3, 0, simb->getIndex(), simb->getName());
                currRelTable->addRecord(record);
        }
        else {
            // simbol deklarisan ali nije definisan
            zapis = new Zapis(simb->getName(), lc + 3, currSecInd, 0);
            tabelaObracanja->records->push_back(zapis);
            }
        }
        else {
            // simbol nije ni deklarisan
            zapis = new Zapis(line, lc + 3, currSecInd, 0);
            tabelaObracanja->records->push_back(zapis);
        }
        currSection->addWord(0);
        lc = lc + 5;
    }
    else if(adrMode == 5) {
        // regdir
        line = line.substr(1,string::npos);
        ind = getRegisterIndex(line);
        byte = 240 + ind;
        currSection->addByte(byte);
        byte = 1;
        currSection->addByte(byte);
        lc = lc + 3;
    }
    else if(adrMode == 6) {
        // regind
        line = line.substr(1,string::npos);
        line = line.substr(1,2);
        ind = getRegisterIndex(line);
        byte = 240 + ind;
        currSection->addByte(byte);
        byte = 2;
        currSection->addByte(byte);
        lc = lc + 3;
    }
    else if(adrMode == 7) {
        // regind + literal
        line = line.substr(1,string::npos);
        line.pop_back();
        cnt = line.find_first_of("+");
        temp = line.substr(0,cnt);
        line = line.substr(cnt + 1, string::npos);

        ind = getRegisterIndex(temp);
        byte = 240 + ind;
        currSection->addByte(byte);
        byte = 3;
        currSection->addByte(byte);

        line = line.substr(1,string::npos);
        if(line[0] == '0' && line[1] == 'x') {
            line = line.substr(2,string::npos);
            help = hexStringToInt(line);
        }
        else {
            help = stoi(line,nullptr,0);
        }
        currSection->addWord(help);

        lc = lc + 5;
    }
    else if(adrMode == 8) {
        // regind + simbol
        
        line = line.substr(1,string::npos);
        line.pop_back();
        cnt = line.find_first_of("+");
        temp = line.substr(0,cnt);
        line = line.substr(cnt + 1, string::npos);

        ind = getRegisterIndex(temp);
        byte = 240 + ind;
        currSection->addByte(byte);
        byte = 3;
        currSection->addByte(byte);

        if(checkIfSymbolExists(line, 1)) {
            simb = findSymbol(line);
            if(simb->getValue() != -1) {
                // simbol definisan
                record = new RelokacioniZapis(lc + 3, 0, simb->getIndex(), simb->getName());
                currRelTable->addRecord(record);             
            }
            else {
                // simbol deklarisan ali nije definisan
                zapis = new Zapis(simb->getName(), lc + 3, currSecInd, 0);
                tabelaObracanja->records->push_back(zapis);
            }
        }
        else {
            // simbol nije ni deklarisan
            zapis = new Zapis(line, lc + 3, currSecInd, 0);
            tabelaObracanja->records->push_back(zapis);
        }
        currSection->addWord(0);
        lc = lc + 5;
    }
}

void Asembler::checkIfInstruction(string line) {
    if (line[0] == '.') {return;}
    int cnt;
    if((cnt = line.find_first_of(":")) != string::npos) {return;}

    string instruction;

    if((cnt = line.find_first_of(" ")) != string::npos) {
        instruction = line.substr(0,cnt);
        line = line.substr(cnt + 1, string::npos);
        if(line[0] == ' ') {
            int cnt2 = line.find_first_not_of(" ");
            line = line.substr(cnt2, string::npos);
        }
    }
    else {
        instruction = line;
        if ((cnt = line.find_first_of("\r")) != string::npos)
            instruction = line.substr(0,cnt);
    }
    
    int ind, byte;
    int ind2;
    int adr;
    int help;
    string temp;
    Simbol* simb;
    RelokacioniZapis* record;
    Zapis* zapis;

    switch(hashInstruction(instruction)) {
        case HALT:      // 1B // OK
            currSection->addByte(0);
            lc++;
            if(currSection) {
                currSection->setCursor(lc);
                findSymbol(currSection->getName())->setSize(lc);
            }
        break;
            
        case INT:       // 2B // OK
            currSection->addByte(16);
            ind = getRegisterIndex(line);
            byte = ind * 16 + 15;
            currSection->addByte(byte);
            lc = lc + 2;
        break;
            
        case IRET:      // 1B // OK
            currSection->addByte(32);
            lc++;
        break;

        case CALL:      // 3B ili 5B
            currSection->addByte(48);

            adr = checkAdressingModeJump(line);
      
            writeOperandJump(line,adr);
        break;

        case RET:       // 1B // OK
            currSection->addByte(64);
            lc++;
        break;

        case JMP:       // 3B ili 5B
            currSection->addByte(80);

            adr = checkAdressingModeJump(line);

            writeOperandJump(line,adr);
        break;

        case JEQ:       // 3B ili 5B
            currSection->addByte(81);

            adr = checkAdressingModeJump(line);

            writeOperandJump(line,adr);
        break;

        case JNE:       // 3B ili 5B
            currSection->addByte(82);

            adr = checkAdressingModeJump(line);

            writeOperandJump(line,adr);
        break;

        case JGT:       // 3B ili 5B
            currSection->addByte(83);

            adr = checkAdressingModeJump(line);

            writeOperandJump(line,adr);
        break;

        case PUSH:
            currSection->addByte(176);

            ind = getRegisterIndex(line);
            byte = 16 * 6 + ind;
            currSection->addByte(byte);
            
            currSection->addByte(18);
            lc = lc + 3;
        break;

        case POP:
            currSection->addByte(160);

            ind = getRegisterIndex(line);
            byte = 16 * ind + 6;
            currSection->addByte(byte);

            currSection->addByte(66);
            lc = lc + 3;
        break;

        case XCHG:
            currSection->addByte(96);

            line = removeSpaces(line);
            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            line = line.substr(cnt + 1, string::npos);
            ind = getRegisterIndex(temp);

            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            ind2 = getRegisterIndex(temp);

            byte = 16*ind + ind2;
                
            currSection->addByte(byte);

            lc = lc + 2;
        break;

        case ADD:
            currSection->addByte(112);

            line = removeSpaces(line);
            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            line = line.substr(cnt + 1, string::npos);
            ind = getRegisterIndex(temp);

            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            ind2 = getRegisterIndex(temp);

            byte = 16*ind + ind2;

            currSection->addByte(byte);
            
            lc = lc + 2;
        break;

        case SUB:
            currSection->addByte(113);

            line = removeSpaces(line);
            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            line = line.substr(cnt + 1, string::npos);
            ind = getRegisterIndex(temp);

            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            ind2 = getRegisterIndex(temp);

            byte = 16*ind + ind2;
                
            currSection->addByte(byte);

            lc = lc + 2;
        break;

        case MUL:
            currSection->addByte(114);

            line = removeSpaces(line);
            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            line = line.substr(cnt + 1, string::npos);
            ind = getRegisterIndex(temp);

            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            ind2 = getRegisterIndex(temp);

            byte = 16*ind + ind2;
                
            currSection->addByte(byte);

            lc = lc + 2;
        break;

        case DIV:
            currSection->addByte(115);

            line = removeSpaces(line);
            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            line = line.substr(cnt + 1, string::npos);
            ind = getRegisterIndex(temp);

            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            ind2 = getRegisterIndex(temp);

            byte = 16*ind + ind2;
                
            currSection->addByte(byte);

            lc = lc + 2;
        break;

        case CMP:
            currSection->addByte(116);

            line = removeSpaces(line);
            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            line = line.substr(cnt + 1, string::npos);
            ind = getRegisterIndex(temp);

            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            ind2 = getRegisterIndex(temp);

            byte = 16*ind + ind2;
                
            currSection->addByte(byte);

            lc = lc + 2;
        break;

        case NOT: // 2B // OK
            currSection->addByte(128);

            ind = getRegisterIndex(line);
            byte = ind * 16;
            currSection->addByte(byte);

            lc = lc + 2;
        break;

        case AND: // 2B // OK
            currSection->addByte(129);

            line = removeSpaces(line);
            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            line = line.substr(cnt + 1, string::npos);
            ind = getRegisterIndex(temp);

            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            ind2 = getRegisterIndex(temp);

            byte = 16*ind + ind2;
                
            currSection->addByte(byte);
            lc = lc + 2;
        break;

        case OR: // 2B // OK
            currSection->addByte(130);

            line = removeSpaces(line);
            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            line = line.substr(cnt + 1, string::npos);
            ind = getRegisterIndex(temp);

            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            ind2 = getRegisterIndex(temp);

            byte = 16*ind + ind2;
                
            currSection->addByte(byte);
            lc = lc + 2;
        break;

        case XOR: // 2B // OK
            currSection->addByte(131);

            line = removeSpaces(line);
            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            line = line.substr(cnt + 1, string::npos);
            ind = getRegisterIndex(temp);

            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            ind2 = getRegisterIndex(temp);

            byte = 16*ind + ind2;
                
            currSection->addByte(byte);
            lc = lc + 2;
        break;

        case TEST: // 2B // OK
            currSection->addByte(132);

            line = removeSpaces(line);
            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            line = line.substr(cnt + 1, string::npos);
            ind = getRegisterIndex(temp);

            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            ind2 = getRegisterIndex(temp);

            byte = 16*ind + ind2;
                
            currSection->addByte(byte);
            lc = lc + 2;
        break;

        case SHL: // 2B // OK
            currSection->addByte(148);

            line = removeSpaces(line);
            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            line = line.substr(cnt + 1, string::npos);
            ind = getRegisterIndex(temp);

            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            ind2 = getRegisterIndex(temp);

            byte = 16*ind + ind2;
            
            currSection->addByte(byte);
            lc = lc + 2;
        break;

        case SHR: // 2B // OK
            currSection->addByte(149);

            line = removeSpaces(line);
            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            line = line.substr(cnt + 1, string::npos);
            ind = getRegisterIndex(temp);

            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            ind2 = getRegisterIndex(temp);

            byte = 16*ind + ind2;
                
            currSection->addByte(byte);
            lc = lc + 2;
        break;

        case LDR:       // 3B ili 5B
            currSection->addByte(164);

            line = removeSpaces(line);
            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            line = line.substr(cnt + 1, string::npos);
            ind = getRegisterIndex(temp);

            adr = checkAdressingMode(line);

            if(adr == 0) {
                // immed literal
                byte = ind * 16;
                currSection->addByte(byte);
                byte = 0;
                currSection->addByte(byte);
                if(line[1] == '0' && line[2] == 'x') {
                    line = line.substr(3,string::npos);
                    help = hexStringToInt(line);
                }
                else {
                    line = line.substr(1, string::npos);
                    help = stoi(line,nullptr,0);
                }
                currSection->addWord(help);
                lc = lc + 5;
            }
            else if(adr == 1) {
                // immed simbol
                byte = ind * 16;
                currSection->addByte(byte);
                byte = 0;
                currSection->addByte(byte);
                line = line.substr(1, string::npos);

                if(checkIfSymbolExists(line,1)) {
                    simb = findSymbol(line);
                    if(simb->getValue() != -1) {
                        // simbol definisan
                        record = new RelokacioniZapis(lc + 3, 0, simb->getIndex(), simb->getName());
                        currRelTable->addRecord(record);
                    }
                    else {
                        // simbol deklarisan ali nije definisan
                        zapis = new Zapis(simb->getName(), lc + 3, currSecInd, 0);
                        tabelaObracanja->records->push_back(zapis);
                    }
                }
                else {
                    // simbol nije ni deklarisan ni definisan
                    zapis = new Zapis(line, lc + 3, currSecInd, 0);
                    tabelaObracanja->records->push_back(zapis);
                }
                currSection->addWord(0);
                lc = lc + 5;
            }
            else if(adr == 2) {
                // memdir literal
                byte = ind * 16;
                currSection->addByte(byte);
                byte = 4;
                currSection->addByte(byte);
                if(line[0] == '0' && line[1] == 'x') {
                    line = line.substr(2,string::npos);
                    help = hexStringToInt(line);
                }
                else {
                    help = stoi(line,nullptr,0);
                }
                currSection->addWord(help);
                lc = lc + 5;
            }
            else if(adr == 3) {
                // memdir simbol
                byte = ind * 16;
                currSection->addByte(byte);
                byte = 4;
                currSection->addByte(byte);

                if(checkIfSymbolExists(line, 1)) {
                    simb = findSymbol(line);
                    if(simb->getValue() != -1) {
                        // simbol definisan
                        record = new RelokacioniZapis(lc + 3, 0, simb->getIndex(), simb->getName());
                        currRelTable->addRecord(record);
                    }
                    else {
                        // simbol deklarisan ali nije definisan
                        zapis = new Zapis(simb->getName(), lc + 3, currSecInd, 0);
                        tabelaObracanja->records->push_back(zapis);
                    }
                }
                else {
                    // simbol nije ni deklarisan
                    zapis = new Zapis(line, lc + 3, currSecInd, 0);
                    tabelaObracanja->records->push_back(zapis);
                }
                currSection->addWord(0);
                lc = lc + 5;
            }
            else if(adr == 5) {
                // regdir
                ind2 = getRegisterIndex(line);
                byte = ind * 16 + ind2;
                currSection->addByte(byte);
                byte = 1;
                currSection->addByte(byte);
                lc = lc + 3;
            }
            else if(adr == 4) {
                // pcrel
                byte = ind * 16 + 7;
                currSection->addByte(byte);
                byte = 3;
                currSection->addByte(byte);

                line = line.substr(1,string::npos);
                
                if(checkIfSymbolExists(line, 1)) {
                    simb = findSymbol(line);
                    if(simb->getValue() != -1) {
                        // simbol definisan
                        if(simb->getSection() != currSecInd) {
                            record = new RelokacioniZapis(lc + 3, 1, simb->getIndex(), simb->getName());
                            currRelTable->addRecord(record);
                            currSection->addWord(0);
                        }
                        else {
                            byte = simb->getValue() - lc - 5;
                            currSection->addWord(byte);
                        }
                    }
                    else {
                        // simbol deklarisan ali nije definisan
                        zapis = new Zapis(simb->getName(), lc + 3, currSecInd, 1);
                        tabelaObracanja->records->push_back(zapis);
                        currSection->addWord(0);
                    }
                }
                else {
                    // simbol nije ni deklarisan
                    zapis = new Zapis(line, lc + 3, currSecInd, 1);
                    tabelaObracanja->records->push_back(zapis);
                    currSection->addWord(0);
                }
                
                lc = lc + 5;
            }
            else if(adr == 6) {
                // regind
                line = line.substr(1,2);
                ind2 = getRegisterIndex(line);
                byte = ind * 16 + ind2;
                currSection->addByte(byte);
                byte = 2;
                currSection->addByte(byte);
                lc = lc + 3;
            }
            else if(adr == 7) {
                // regind + literal
                line = line.substr(1,string::npos);
                line.pop_back();
                cnt = line.find_first_of("+");
                temp = line.substr(0,cnt);
                line = line.substr(cnt + 1, string::npos);

                ind2 = getRegisterIndex(temp);
                byte = ind * 16 + ind2;
                currSection->addByte(byte);
                byte = 3;
                currSection->addByte(byte);

                if(line[0] == '0' && line[1] == 'x') {
                    line = line.substr(3,string::npos);
                    help = hexStringToInt(line);
                }
                else {
                    help = stoi(line,nullptr,0);
                }
                help = hexStringToInt(line);
                currSection->addWord(help);

                lc = lc + 5;
            }
            else if(adr == 8) {
                // regind + simbol
                line = line.substr(1,string::npos);
                line.pop_back();
                cnt = line.find_first_of("+");
                temp = line.substr(0,cnt);
                line = line.substr(cnt + 1, string::npos);

                ind2 = getRegisterIndex(temp);
                byte = ind * 16 + ind2;
                currSection->addByte(byte);
                byte = 3;
                currSection->addByte(byte);

                if(checkIfSymbolExists(line, 1)) {
                    simb = findSymbol(line);
                    if(simb->getValue() != -1) {
                        // simbol definisan
                        record = new RelokacioniZapis(lc + 3, 0, simb->getIndex(), simb->getName());
                        currRelTable->addRecord(record);
                        
                    }
                    else {
                        // simbol deklarisan ali nije definisan
                        zapis = new Zapis(simb->getName(), lc + 3, currSecInd, 0);
                        tabelaObracanja->records->push_back(zapis);
                    }
                }
                else {
                    // simbol nije ni deklarisan
                    zapis = new Zapis(line, lc + 3, currSecInd, 0);
                    tabelaObracanja->records->push_back(zapis);
                }
                currSection->addWord(0);
                lc = lc + 5;
            }

        break;

        case STR:       // 3B ili 5B
            currSection->addByte(180);
            line = removeSpaces(line);

            cnt = line.find_first_of("0123456789");
            cnt++;
            temp = line.substr(0,cnt);
            line = line.substr(cnt + 1, string::npos);
            ind = getRegisterIndex(temp);
            
            adr = checkAdressingMode(line);
            
            if(adr == 0) {
                // immed literal
                byte = ind * 16;
                currSection->addByte(byte);
                byte = 0;
                currSection->addByte(byte);
                if(line[1] == '0' && line[2] == 'x') {
                    line = line.substr(3,string::npos);
                    help = hexStringToInt(line);
                }
                else {
                    line = line.substr(1, string::npos);
                    help = stoi(line,nullptr,0);
                }
                currSection->addWord(help);
                lc = lc + 5;
            }
            else if(adr == 1) {
                // immed simbol
                byte = ind * 16;
                currSection->addByte(byte);
                byte = 0;
                currSection->addByte(byte);
                line = line.substr(1, string::npos);

                if(checkIfSymbolExists(line,1)) {
                    simb = findSymbol(line);
                    if(simb->getValue() != -1) {
                        // simbol definisan
                        record = new RelokacioniZapis(lc + 3, 0, simb->getIndex(), simb->getName());
                        currRelTable->addRecord(record);
                    }
                    else {
                        // simbol deklarisan ali nije definisan
                        zapis = new Zapis(simb->getName(), lc + 3, currSecInd, 0);
                        tabelaObracanja->records->push_back(zapis);
                    }
                }
                else {
                    // simbol nije ni deklarisan ni definisan
                    zapis = new Zapis(line, lc + 3, currSecInd, 0);
                    tabelaObracanja->records->push_back(zapis);
                }
                currSection->addWord(0);
                lc = lc + 5;
            }
            else if(adr == 2) {
                // memdir literal
                byte = ind * 16;
                currSection->addByte(byte);
                byte = 4;
                currSection->addByte(byte);
                if(line[0] == '0' && line[1] == 'x') {
                    line = line.substr(2,string::npos);
                    help = hexStringToInt(line);
                }
                else {
                    help = stoi(line,nullptr,0);
                }
                currSection->addWord(help);
                lc = lc + 5;
            }
            else if(adr == 3) {
                // memdir simbol
                byte = ind * 16;
                currSection->addByte(byte);
                byte = 4;
                currSection->addByte(byte);

                if(checkIfSymbolExists(line, 1)) {
                    simb = findSymbol(line);
                    if(simb->getValue() != -1) {
                        // simbol definisan
                        record = new RelokacioniZapis(lc + 3, 0, simb->getIndex(), simb->getName());
                        currRelTable->addRecord(record);
                    }
                    else {
                        // simbol deklarisan ali nije definisan
                        zapis = new Zapis(simb->getName(), lc + 3, currSecInd, 0);
                        tabelaObracanja->records->push_back(zapis);
                    }
                }
                else {
                    // simbol nije ni deklarisan
                    zapis = new Zapis(line, lc + 3, currSecInd, 0);
                    tabelaObracanja->records->push_back(zapis);
                }
                currSection->addWord(0);
                lc = lc + 5;
            }
            else if(adr == 5) {
                // regdir
                ind2 = getRegisterIndex(line);
                byte = ind * 16 + ind2;
                currSection->addByte(byte);
                byte = 1;
                currSection->addByte(byte);
                lc = lc + 3;
            }
            else if(adr == 4) {
                // pcrel
                byte = ind * 16 + 7;
                currSection->addByte(byte);
                byte = 3;
                currSection->addByte(byte);

                line = line.substr(1,string::npos);
                
                if(checkIfSymbolExists(line, 1)) {
                    simb = findSymbol(line);
                    if(simb->getValue() != -1) {
                        // simbol definisan
                        if(simb->getSection() != currSecInd) {
                            record = new RelokacioniZapis(lc + 3, 1, simb->getIndex(), simb->getName());
                            currRelTable->addRecord(record);
                            currSection->addWord(0);
                        }
                        else {
                            byte = simb->getValue() - lc - 5;
                            currSection->addWord(byte);
                        }
                    }
                    else {
                        // simbol deklarisan ali nije definisan
                        zapis = new Zapis(simb->getName(), lc + 3, currSecInd, 1);
                        tabelaObracanja->records->push_back(zapis);
                        currSection->addWord(0);
                    }
                }
                else {
                    // simbol nije ni deklarisan
                    zapis = new Zapis(line, lc + 3, currSecInd, 1);
                    tabelaObracanja->records->push_back(zapis);
                    currSection->addWord(0);
                }
                
                lc = lc + 5;
            }
            else if(adr == 6) {
                // regind
                line = line.substr(1,2);
                ind2 = getRegisterIndex(line);
                byte = ind * 16 + ind2;
                currSection->addByte(byte);
                byte = 2;
                currSection->addByte(byte);
                lc = lc + 3;
            }
            else if(adr == 7) {
                // regind + literal
                line = line.substr(1,string::npos);
                line.pop_back();
                cnt = line.find_first_of("+");
                temp = line.substr(0,cnt);
                line = line.substr(cnt + 1, string::npos);

                ind2 = getRegisterIndex(temp);
                byte = ind * 16 + ind2;
                currSection->addByte(byte);
                byte = 3;
                currSection->addByte(byte);

                if(line[0] == '0' && line[1] == 'x') {
                    line = line.substr(2,string::npos);
                    help = hexStringToInt(line);
                }
                else {
                    help = stoi(line,nullptr,0);
                }
                help = hexStringToInt(line);
                currSection->addWord(help);

                lc = lc + 5;
            }
            else if(adr == 8) {
                // regind + simbol
                line = line.substr(1,string::npos);
                line.pop_back();
                cnt = line.find_first_of("+");
                temp = line.substr(0,cnt);
                line = line.substr(cnt + 1, string::npos);

                ind2 = getRegisterIndex(temp);
                byte = ind * 16 + ind2;
                currSection->addByte(byte);
                byte = 3;
                currSection->addByte(byte);

                if(checkIfSymbolExists(line, 1)) {
                    simb = findSymbol(line);
                    if(simb->getValue() != -1) {
                        // simbol definisan
                        record = new RelokacioniZapis(lc + 3, 0, simb->getIndex(), simb->getName());
                        currRelTable->addRecord(record);
                        
                    }
                    else {
                        // simbol deklarisan ali nije definisan
                        zapis = new Zapis(simb->getName(), lc + 3, currSecInd, 0);
                        tabelaObracanja->records->push_back(zapis);
                    }
                }
                else {
                    // simbol nije ni deklarisan
                    zapis = new Zapis(line, lc + 3, currSecInd, 0);
                    tabelaObracanja->records->push_back(zapis);
                }
                currSection->addWord(0);
                lc = lc + 5;
            }
        break;

        case ILLEGAL_INSTRUCTION:
            cout << "GRESKA, ILEGALNA INSTRUKCIJA\n";
            exit(-2);
        break;

    }
}

int Asembler::getRegisterIndex(string str) {
    int cnt;
    cnt = str.find_first_of("0123456789");
    string tmp;
    tmp = str.substr(cnt,string::npos);
    int ret;
    ret = stoi(tmp,nullptr,0);
    return ret;
}

int Asembler::checkAdressingMode(string str) {
    int cnt;
    if(str[0] == '$') {
        // immed
        
        if((cnt = str.find_first_of("0123456789")) != string::npos) {
            if(cnt == 1) {
                // immed literal
                return 0;
            }
            else {
                // immed simbol
                return 1;
            }
        }
        else {
            // immed simbol
            return 1;
        }
    }
    else if (str[0] == '%') {
        // pcrel
        return 4;
    }
    else if (str[0] == '[') {
        if (str[3] == ']') {
            // regind
            return 6;
        }
        
        str = str.substr(4,string::npos);
        cnt = str.find_first_of("]");
        str = str.substr(0,cnt);
        if ((cnt = str.find_first_not_of("0123456789ABCDEFx")) != string::npos) {
            // regind + simbol
            return 8;
        } else {
            // regind + literal
            return 7;
        }    
    }
    else if (str[0] == 'r' && str.size() == 2) {
        // regdir
        return 5;
    }
    else {
        if ((cnt = str.find_first_not_of("0123456789ABCDEFx")) != string::npos) {
            // simbol
            return 3;
        } else {
            // literal
            return 2;
        }  
    }
}

int Asembler::checkAdressingModeJump(string str) {
    int cnt;
    str = removeSpaces(str);
    if(str[0] != '*' && str[0] != '%') {
        if ((cnt = str.find_first_not_of("0123456789ABCDEFx")) != string::npos)  {
            return 1; // vrednost <simbol> sa apsolutnim adresiranjem
        }
        else {
            return 0; // vrednost <literal>
        }
    }
    else if(str[0] == '%') {
        return 2; // vrednost <simbol> sa pc relativnim adresiranjem
    }
    else {
        str = str.substr(1,string::npos);
        if(str[0] != '[') {
            cnt = str.find_first_of("0123456789");
            if(str[0] == 'r' && cnt == 1) {
                return 5;
            }
            else {
                if ((cnt = str.find_first_not_of("0123456789ABCDEFx")) != string::npos) {
                    return 4; // vrednost iz memorije na adresi <literal>
                }
                else {
                    return 3; // vrednost iz memorije na adresi <simbol>
                }
            }
        }
        else {
            if (str[3] == ']') {
            // regind
            return 6;
            }
            str = str.substr(4,string::npos);
            cnt = str.find_first_of("]");
            str = str.substr(0,cnt);
            if ((cnt = str.find_first_not_of("0123456789ABCDEFx")) != string::npos) {
                // regind + simbol
                return 8;
            } else {
                // regind + literal
                return 7;
            } 
         }
    }
}

void Asembler::resolveSymbols() {
    list<Sekcija*>::iterator itSek = sectTable.begin();
    list<TabelaRelokacija*>::iterator itRel = relTableList.begin(); 
    for(int i = 0; i < sectTable.size(); i++) {
        Sekcija *sek = *itSek;
        list<int>::iterator it = sek->data->begin();
        int j;
        currRelTable = *itRel;
        for (j = 0; j < sek->data->size(); j++) {
            if((tabelaObracanja->records->size() > 0) && (tabelaObracanja->records->front()->getOffs() == j)) {
                Zapis* temp = tabelaObracanja->records->front();
                tabelaObracanja->records->pop_front();
                if(findSymbol(temp->getName())->getValue() != -1 || findSymbol(temp->getName())->getGlobal() == "extern") {
                    Simbol* simb = findSymbol(temp->getName());
                    RelokacioniZapis* record = new RelokacioniZapis(temp->getOffs(), temp->getRel(), simb->getIndex(), simb->getName());
                    currRelTable->addRecord(record);
                }
                else {
                    cout << "GRESKA : SIMBOL NIJE DEFINISAN\n";
                    exit(1);
                }  
            }
            advance(it, 1);
        }   
        advance(itSek,1);
        advance(itRel,1);
    }
}

int Asembler::hexStringToInt(string str) {
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
        ret = ret + tmp * pow(16,power);
        power++;
    }
    return ret;
}

void Asembler::setOutput(string outputName) {
    file = new ofstream(outputName);
}

string Asembler::removeSpaces(string str){
    int index;
    string ret = str;
    while ((index = ret.find_first_of(" \t")) != string::npos){
        ret = ret.substr(0,index).append(ret.substr(index+1, string::npos));
    }
    return ret;
}

string Asembler::removeComments(string str){
    int index;
    string ret = str;
    if((index = ret.find_first_of("#")) != string::npos)
        ret = ret.substr(0,index);
    return ret;
}


void Asembler::printSymbolTable() {
    cout << "\n---------------------------Tabela simbola---------------------------\n";
    cout << "   NAME      |   TYPE   | VALUE | SECTION | SCOPE | INDEX | SIZE | FLAGS\n";
    list<Simbol*>::iterator iter = symTable.begin();
    for (int i = 0; i < symTable.size(); i++) {
        Simbol *sim = *iter;
        cout << *sim;
        advance(iter, 1);
    }

    cout << *tabelaObracanja << "\n";
}

void Asembler::printSections() {
    cout << "--------------------------- SEKCIJE ---------------------------\n";
    list<Sekcija *>::iterator itSek = sectTable.begin();
    for (int i = 0; i < sectTable.size(); i++) {
        Sekcija *sek = *itSek;
        cout << *sek << "\n";
        advance(itSek,1);
    }
    list<TabelaRelokacija *>::iterator iter = relTableList.begin();
    for (int i = 0; i < relTableList.size(); i++) {
        TabelaRelokacija *tab = *iter;
        cout << *tab;
        advance(iter, 1);
    }
    
}

Instructions Asembler::hashInstruction(string str) {
    if(str == "halt") return HALT;
    else if(str == "int") return INT;
    else if(str == "iret") return IRET;
    else if(str == "call") return CALL;
    else if(str == "ret") return RET;
    else if(str == "jmp") return JMP;
    else if(str == "jeq") return JEQ;
    else if(str == "jne") return JNE;
    else if(str == "jgt") return JGT;
    else if(str == "push") return PUSH;
    else if(str == "pop") return POP;
    else if(str == "xchg") return XCHG;
    else if(str == "add") return ADD;
    else if(str == "sub") return SUB;
    else if(str == "mul") return MUL;
    else if(str == "div") return DIV;
    else if(str == "cmp") return CMP;
    else if(str == "not") return NOT;
    else if(str == "and") return AND;
    else if(str == "or") return OR;
    else if(str == "xor") return XOR;
    else if(str == "test") return TEST;
    else if(str == "shl") return SHL;
    else if(str == "shr") return SHR;
    else if(str == "ldr") return LDR;
    else if(str == "str") return STR;
    return ILLEGAL_INSTRUCTION;
}

Directives Asembler::hashDirective(string str) {
    if (str == ".global") return GLOBAL;
    else if (str == ".extern") return EXTERN;
    else if (str == ".section") return SECTION;
    else if (str == ".word")return WORD;
    else if (str == ".skip") return SKIP;
    else if (str == ".end") return END;
    return DIRECTIVE_NOT_FOUND;
}

bool Asembler::checkIfSymbolExists(string name, short type) {
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

Simbol* Asembler::findSymbol(string name) {
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

Simbol* Asembler::findSymbolByIndex(int index) {
    Simbol* tmp;
    list<Simbol*>::iterator it = symTable.begin();
    for(int i = 0; i < symTable.size(); i++) {
        tmp = *it;
        advance(it,1);
        if(tmp->getIndex() == index)
            return tmp;
    }
    return 0;
}