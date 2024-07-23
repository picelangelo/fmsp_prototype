#include <iostream>
#include <ostream>
#include <cstring>
#include <vector>
#include "opcodes.h"
#include "lexer.h"
#include "evmint.h"
#include "bbanalyzer.h"
#include "souffleinterface.h"
#include "souffle/SouffleInterface.h"

int main(int argc, char *argv[]) {
    char *contract_bc;

    if (argc == 2) {
        contract_bc = argv[1];
    } else {
        std::cout << "Synopsis: \n";
        std::cout << "    " << argv[0] << " <Contract Bytecode>" << std::endl;
        exit(1);
    }

    //remove 0x prefix
    if (contract_bc[0] == '0' && contract_bc[1] == 'x') {
        contract_bc = contract_bc + 2;
    }

    //parse code
    Lexer lexer;
    int contract_size = strlen(contract_bc) ;
    if (contract_size%2) {
        std::cerr << "Contract size is not even\n";
        exit(1);
    }
    for (int i(0); i < contract_size; i+=2) {
        char c0 = *(contract_bc + i);
        char c1 = *(contract_bc + i + 1);
        lexer.next(c0,c1);
    }

    std::vector<OPCode>& parsed_bc = lexer.get_bytecode();

    //in souffleinterface.h
    analyze(parsed_bc);

    return 0;
}
