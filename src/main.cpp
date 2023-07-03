#include "../inc/asembler.hpp"
#include "../inc/parser.hpp"


int main(int argc, char *argv[]) {

    string inputFile = "main.s";
    string outputFile = "output.o";

    if (argc == 4 && (strcmp(argv[1],"-o")==0)) {
        outputFile = argv[2];
        inputFile= argv[3];    
    } else {
        cout << "Argumenti nisu korektni\n";
        return 0;
    }

    Parser* parser = new Parser(inputFile);

    string** asemblerInput = parser->getParsed();
    int size = parser->getSize();

    Asembler::setOutput(outputFile);
    Asembler::assemble(asemblerInput,size);

    //delete parser;
    cout << "Asembliranje fajla " << inputFile << " uspesno zavrseno\n";
    return 0;
}