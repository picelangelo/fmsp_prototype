#ifndef OPCODES_H_
#define OPCODES_H_

#include <string>
#include <format>
#include "evmint.h"

enum OPCodeEnum {
    ADD, JUMP, JUMPI, JUMPDEST, POP, PUSH, NOP
};

class OPCode {
    private:
        enum LLOPCodeEnum {
          ADD = 0x01,
          JUMP = 0x56,
          JUMPI = 0x57,
          JUMPDEST = 0x5B,
          POP = 0x50,
          PUSH0 = 0x5F,
          PUSH1, // remaining values get inferred
          PUSH2,
          PUSH3,
          PUSH4,
          PUSH5,
          PUSH6,
          PUSH7,
          PUSH8,
          PUSH9,
          PUSH10,
          PUSH11,
          PUSH12,
          PUSH13,
          PUSH14,
          PUSH15,
          PUSH16,
          PUSH17,
          PUSH18,
          PUSH19,
          PUSH20,
          PUSH21,
          PUSH22,
          PUSH23,
          PUSH24,
          PUSH25,
          PUSH26,
          PUSH27,
          PUSH28,
          PUSH29,
          PUSH30,
          PUSH31,
          PUSH32,
        };
        LLOPCodeEnum op;
        EVMInt pushval;
        EVMInt pc;
        
        bool is_push() {
            return (op >= PUSH0 && op <= PUSH32);
        }

    public:

        OPCode(int _op, int _pc) {
            op = static_cast<LLOPCodeEnum>(_op);
            pc = EVMInt(_pc);
        }

        /*
        ** If the instruction is of type PUSHi, return i,
        ** otherwise return 0.
             */
        int push_arg_count() {
            if (is_push()) {
                return op - PUSH0;
            }
            return 0;
        }

        int stack_arg_count() {
            switch(op) {
                case ADD:
                case JUMPI:
                    return 2;
                case POP:
                case JUMP:
                    return 1;
                case JUMPDEST:
                default: //PUSH0-32
                    return 0;
            }
        }

        int stack_ret_count() {
            switch(op) {
                case JUMP:
                case JUMPI:
                case JUMPDEST:
                case POP:
                    return 0;
                case ADD:
                default: //PUSH0-32
                    return 1;
            }
        }

        void next_pushval_byte(uint8_t byte) {
            pushval.addByte(byte);
        }

        OPCodeEnum get_op() {
            switch (op) {
                case ADD:
                    return OPCodeEnum::ADD;
                case JUMP:
                    return OPCodeEnum::JUMP;
                case JUMPI:
                    return OPCodeEnum::JUMPI;
                case JUMPDEST:
                    return OPCodeEnum::JUMPDEST;
                case POP:
                    return OPCodeEnum::POP;
                default:
                    if (is_push()) {
                        return OPCodeEnum::PUSH;
                    } else {
                        return OPCodeEnum::NOP;
                    }
            }
        }

        EVMInt get_pc() {
            return pc;
        }

        std::string str() {
            switch (op) {
                case ADD:
                    return "ADD";
                case JUMP:
                    return "JUMP";
                case JUMPI:
                    return "JUMPI";
                case JUMPDEST:
                    return "JUMPDEST";
                case POP:
                    return "POP";
                default:
                    if (is_push()) {
                        return std::format("PUSH {}", pushval.str());
                    } else {
                        //not implemented
                        return "NOP";
                    }
            }
        }

        std::string souffle_rel() {
            switch (op) {
                case ADD:
                    return "addinstr";
                case JUMP:
                    return "jumpinstr";
                case JUMPI:
                    return "jumpiinstr";
                case JUMPDEST:
                    return "jumpdestinstr";
                case POP:
                    return "popinstr";
                default:
                    if (is_push()) {
                        return "pushinstr";
                    } else {
                        //not implemented
                        throw static_cast<int>(op);
                    }
            }
        }

        void souffle_populate(souffle::tuple& tuple) {
            switch (op) {
                case ADD:
                case JUMP:
                case JUMPI:
                case JUMPDEST:
                case POP:
                    tuple << pc.str();
                    break;
                default:
                    if (is_push()) {
                        EVMInt os(op - PUSH0);
                        tuple << pc.str() << pushval.str() << os.str();
                        break;
                    } else {
                        //not implemented
                        throw static_cast<int>(op);
                    }
            }
        }
};

#endif // OPCODES_H_
