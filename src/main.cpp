#include "io.h"
#include "parser.h"

int main(int argc, char *argv[]) {
        if (argc < 2) {
                std::cout << "Usage: pa2 assets/src1.txt assets/src2.txt assets/src3.txt\n";
                std::exit(EXIT_FAILURE);
        }
        
        for (size_t i = 1; i < argc; i++) {
                std::cout << argv[i] << ": ";
                std::string source = pa::io::readFile(argv[i]);
                pa::Parser parser(source);
                parser.parseProgram();
                std::cout << "Parsed Successfully!\n";
        }

	return 0;
}