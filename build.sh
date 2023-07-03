g++ -o asembler ./src/main.cpp ./src/asembler.cpp ./src/parser.cpp ./src/relokacioniZapis.cpp ./src/sekcija.cpp ./src/simbol.cpp ./src/tabelaRelZapisa.cpp ./src/tabelaObracanjaUnapred.cpp ./src/zapis.cpp
./asembler -o main.o main.s
./asembler -o math.o math.s
./asembler -o ivt.o ivt.s
./asembler -o isr_reset.o isr_reset.s
./asembler -o isr_terminal.o isr_terminal.s
./asembler -o isr_timer.o isr_timer.s
./asembler -o isr_user0.o isr_user0.s
g++ -o linker ./src/linkerMain.cpp ./inc/linker.hpp ./inc/parser.hpp ./inc/relokacioniZapis.hpp ./inc/sekcija.hpp ./inc/simbol.hpp ./inc/tabelaRelZapisa.hpp ./src/linker.cpp ./src/parser.cpp ./src/relokacioniZapis.cpp ./src/sekcija.cpp ./src/simbol.cpp ./src/tabelaRelZapisa.cpp
./linker -o program.hex ivt.o math.o main.o isr_reset.o isr_terminal.o isr_timer.o isr_user0.o
g++ -o emulator ./src/emulatorMain.cpp ./src/emulator.cpp ./src/parser.cpp
./emulator program.hex
