#include "../inc/emulator.hpp"
#include "../inc/parser.hpp"

int main(int argc, char *argv[]) {

    int numFiles = argc - 3;
    string inputFile;

    if (argc > 1) {
        inputFile = argv[1];   
    } else {
        cout << "Argumenti nisu korektni\n";
        return 0;
    }

    Parser* parser = new Parser(inputFile);

    string** emulatorInput = parser->getParsed();
    int size = parser->getSize();

    cout << "Emulate " << inputFile << "\n";
    Emulator::getData(emulatorInput,size);
    Emulator::Emulate();

    //delete parser;
 
    return 0;
}