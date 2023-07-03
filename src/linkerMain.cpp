#include "../inc/parser.hpp"
#include "../inc/linker.hpp"
//#include <algorithm>

int main(int argc, char *argv[]) {

    int numFiles = argc - 3;
    string inputFile[numFiles];
    string outputFile;

    if (argc > 3 && (strcmp(argv[1],"-o")==0)) {
        outputFile = argv[2];
        for(int i = 0; i < numFiles; i++)
            inputFile[i] = argv[3 + i];    
    } else {
        cout << "Argumenti nisu korektni\n";
        return 0;
    }

    Linker::setOutput(outputFile);

    Parser** parseri = new Parser*[numFiles];
    for(int j = 0; j < numFiles; j++) {      
        parseri[j] = new Parser(inputFile[j]);
    }
    for(int j = 0; j < numFiles; j++) {
        string** linkerInput = parseri[j]->getParsed();
        int size = parseri[j]->getSize();
        Linker::getData(linkerInput,size);
    }

    
   /*
    Parser* parser = new Parser(line);
    cout << "Parser OK";
    string** linkerInput = parser->getParsed();
    int size = parser->getSize();
    Linker::getData(linkerInput,size);

    Parser* parser2 = new Parser(inputFile[1]);
    string** linkerInput2 = parser2->getParsed();
    int size2 = parser2->getSize();
    Linker::getData(linkerInput2,size2);

    Parser* parser3 = new Parser(inputFile[2]);
    string** linkerInput3 = parser3->getParsed();
    int size3 = parser3->getSize();
    Linker::getData(linkerInput3,size3);

    Parser* parser4 = new Parser(inputFile[3]);
    string** linkerInput4 = parser4->getParsed();
    int size4 = parser4->getSize();
    Linker::getData(linkerInput4,size4);

    Parser* parser5 = new Parser(inputFile[4]);
    string** linkerInput5 = parser5->getParsed();
    int size5 = parser5->getSize();
    Linker::getData(linkerInput5,size5);

    Parser* parser6 = new Parser(inputFile[5]);
    string** linkerInput6 = parser6->getParsed();
    int size6 = parser6->getSize();
    Linker::getData(linkerInput6,size6);

    Parser* parser7 = new Parser(inputFile[6]);
    string** linkerInput7 = parser7->getParsed();
    int size7 = parser7->getSize();
    Linker::getData(linkerInput7,size7);
    */
    Linker::link(numFiles);
    cout << "Linker uspesno zavrsio\n";
    //delete parser;
 
    return 0;
}

