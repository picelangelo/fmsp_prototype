#ifndef LEXER_H_
#define LEXER_H_

#include <vector>
#include "opcodes.h"
#include "main.h"

class Lexer {
    private:
        int state;
        int pc;
        std::vector<OPCode> bc_parsed;

    public:
        Lexer() {
            state = 0;
            pc = 0;
        }
        void next(char c0, char c1) {
            if (state == 0) {
                OPCode op((char_to_digit(c0) << 4) + char_to_digit(c1), pc);
                bc_parsed.push_back(op);
                state = op.push_arg_count();
            } else {
                bc_parsed.back().next_pushval_byte((char_to_digit(c0) << 4) + char_to_digit(c1));
                state--;
            }
            pc++;
        }

        std::vector<OPCode>& get_bytecode() {
            return bc_parsed;
        }
};

#endif // LEXER_H_