#include "../inc/parser.hpp"
#include <algorithm>

Parser::Parser(string fileName) {

    char line[100];
    int i = 0;
 
    ifstream inputFile(fileName, ios::in);

    if(!inputFile.is_open())
        cout << "Greska pri otvaranju fajla\n";

    int numOfLines = count(istreambuf_iterator<char>(inputFile), istreambuf_iterator<char>(), '\n') + 1;
    
    lines = new string*[numOfLines];
    size = numOfLines;
    inputFile.seekg(0);

    while (inputFile) {
        inputFile.getline(line, 80);
        lines[i++] = new string(line);
        //cout << *lines[i-1] << "\n";
    }
    inputFile.close();

}

Parser::~Parser() {

    for (int i = 0; i < size; i++){
        if (lines[i] != NULL)
            delete lines[i];
    }
	delete[] lines;

}

string** Parser::getParsed() {

    
    return lines;
}

int Parser::getSize(){
    return size;
}