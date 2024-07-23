#ifndef BBANALYZER_H_ 
#define BBANALYZER_H_ 

#include <vector>
#include <unordered_map>
#include "opcodes.h"

class BBAnalyzer {

    private:

        std::unordered_map<std::string,int[2]> pc_block_map;

        void insert(std::vector<OPCode>& block) {
            int n = 0;
            int m = 0;
            for (OPCode op : block) {
                int args_from_block = (op.stack_arg_count() <= m) ? op.stack_arg_count() : m;
                int additional_args = op.stack_arg_count() - args_from_block;
                n += additional_args;
                m -= args_from_block;
                m += op.stack_ret_count();
            }
            pc_block_map[block[0].get_pc().str()][0] = n;
            pc_block_map[block[0].get_pc().str()][1] = m;
        }

        void extract_basic_blocks(std::vector<OPCode>& code) {
            std::vector<OPCode> bb;
            for (OPCode op : code) {
                switch (op.get_op()) {
                    case JUMP:
                    case JUMPI: 
                        bb.push_back(op);
                        insert(bb);
                        bb = {};
                        break;
                    case JUMPDEST:
                        if (!bb.empty()) {
                            insert(bb);
                            bb = {};
                        }
                    default:
                        bb.push_back(op);
                        break;
                }
            }
            if (!bb.empty()) {
                insert(bb);
            }
        }

    public:
        BBAnalyzer(std::vector<OPCode>& code) {
            extract_basic_blocks(code);
        }

        int get_arg_cnt(std::string pc) {
            return pc_block_map[pc][0];
        }

        int get_ret_cnt(std::string pc) {
            return pc_block_map[pc][1];
        }
};

#endif // BBANALYZER_H_ 