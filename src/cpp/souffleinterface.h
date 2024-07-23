#ifndef SOUFFLEFACTGENERATOR_H_ 
#define SOUFFLEFACTGENERATOR_H_

#include "opcodes.h"
#include "bbanalyzer.h"
#include "souffle/SouffleInterface.h"

#include <vector>
#include <optional>


void generate_instrrels(souffle::SouffleProgram *prog, std::vector<OPCode>& code) {
    for (OPCode op : code)  {
        try {
            if (souffle::Relation *rel = prog->getRelation(op.souffle_rel())) {
                souffle::tuple instrrel(rel); 
                op.souffle_populate(instrrel);
                rel->insert(instrrel);
            } else {
                std::cerr << "Failed to get input relation " << op.souffle_rel() << std::endl;
                exit(1);
            }
        } catch (int op) {
            std::cerr << std::format("Instruction 0x{:0>2x} not implemented\n", op);
            exit(1);
        }
    }
}

void generate_mstaterel(souffle::SouffleProgram *prog, std::string pc, int iteration, souffle::RamDomain stackptr) {
    if (souffle::Relation *rel = prog->getRelation("MState")) {
        souffle::tuple mstate(rel); 
        mstate << pc << iteration << stackptr;
        rel->insert(mstate);
    } else {
        std::cerr << "Failed to get input relation MState" << std::endl;
        exit(1);
    }
}

void generate_entryrel(souffle::SouffleProgram *prog, std::string pc, int iteration) {
    if (souffle::Relation *rel = prog->getRelation("entry")) {
        souffle::tuple entry(rel); 
        entry << pc << iteration;
        rel->insert(entry);
    } else {
        std::cerr << "Failed to get input relation entry" << std::endl;
        exit(1);
    }
}

std::vector<std::optional<std::string>> decode_stack(souffle::SouffleProgram *prog, souffle::RamDomain stackptr) {
    const int V = 1;
    
    std::vector<std::optional<std::string>> stack;
    souffle::RecordTable& recordTable = prog->getRecordTable();
    souffle::SymbolTable& symbolTable = prog->getSymbolTable();

    while (stackptr) {

        //contains list element on position 0 and the remaining stack at position 1
        const souffle::RamDomain* myTuple0 = recordTable.unpack(stackptr, 2); 

        //contains branch of ADT on position 0 and value on position 1, see https://github.com/souffle-lang/souffle/issues/2181#issuecomment-1025128550
        const souffle::RamDomain* myTuple1 = recordTable.unpack(myTuple0[0], 2);  
        std::optional<std::string> stackElement;
        if (myTuple1[0] == V) {
            stackElement = symbolTable.decode(myTuple1[1]);
        } else {
            stackElement = std::nullopt;
        }
        stack.push_back(stackElement);

        stackptr = myTuple0[1];
    }
    return stack;
}

std::string stack_to_string(souffle::SouffleProgram *prog, souffle::RamDomain stackptr) {
    const int V = 1;
    
    std::string stack;
    souffle::RecordTable& recordTable = prog->getRecordTable();
    souffle::SymbolTable& symbolTable = prog->getSymbolTable();

    while (stackptr) {
        
        //contains list element on position 0 and the remaining stack at position 1
        const souffle::RamDomain* myTuple0 = recordTable.unpack(stackptr, 2); 
        
        //contains branch of ADT on position 0 and value on position 1, see https://github.com/souffle-lang/souffle/issues/2181#issuecomment-1025128550
        const souffle::RamDomain* myTuple1 = recordTable.unpack(myTuple0[0], 2);  

        stack += (myTuple1[0] == V) ? (symbolTable.decode(myTuple1[1])) : "T";
        stack += " ";

        stackptr = myTuple0[1];
    }
    return stack;
}


std::vector<souffle::RamDomain> split_stack(souffle::SouffleProgram *prog, souffle::RamDomain stackptr, int split_idx) {
    souffle::RecordTable& recordTable = prog->getRecordTable();

    std::vector<souffle::RamDomain> topStack;
    for (int i = 0; i < split_idx && stackptr; ++i) {
        //contains list element on position 0 and the remaining stack at position 1
        const souffle::RamDomain* myTuple0 = recordTable.unpack(stackptr, 2); 
        topStack.push_back(myTuple0[0]);
        stackptr = myTuple0[1];
    }

    souffle::RamDomain part2 = stackptr;

    stackptr = 0;
    for (auto it = topStack.rbegin(); it != topStack.rend(); ++it ) {
        souffle::RamDomain myTuple[2] = {*it, stackptr}; 
        stackptr = recordTable.pack(myTuple, 2);
    }

    return {stackptr, part2};
}

std::string to_analyzed_blocks_key(souffle::SouffleProgram *prog, std::string pc, souffle::RamDomain stackptr) {
    return pc + "; " + stack_to_string(prog, stackptr);
}

souffle::RamDomain merge_stack(souffle::SouffleProgram *prog, souffle::RamDomain top, souffle::RamDomain bot) {
    souffle::RecordTable& recordTable = prog->getRecordTable();

    std::vector<souffle::RamDomain> topStack;
    while (top) {
        //contains list element on position 0 and the remaining stack at position 1
        const souffle::RamDomain* myTuple0 = recordTable.unpack(top, 2); 
        topStack.push_back(myTuple0[0]);
        top = myTuple0[1];
    }

    for (auto it = topStack.rbegin(); it != topStack.rend(); ++it ) {
        souffle::RamDomain myTuple[2] = {*it, bot}; 
        bot = recordTable.pack(myTuple, 2);
    }

    return bot;
}


void run_block_analysis(souffle::SouffleProgram *prog, BBAnalyzer& bbanalyzer, std::unordered_map<std::string, bool>& analyzed_blocks, std::string pc, int *i, souffle::RamDomain stackptr) {
    int arg_cnt = bbanalyzer.get_arg_cnt(pc);
    auto stack_parts = split_stack(prog, stackptr, arg_cnt);
    if (analyzed_blocks[to_analyzed_blocks_key(prog, pc, stack_parts[0])]) {
        //this has already been called
        return;
    }
    analyzed_blocks[to_analyzed_blocks_key(prog, pc, stack_parts[0])] = true;
    generate_mstaterel(prog, pc, ++(*i), stack_parts[0]);
    generate_entryrel(prog, pc, *i);
    prog->run();
    
    if (souffle::Relation *rel = prog->getRelation("Goto")) {
        souffle::RamDomain newstackptr;
        int iteration;
        std::string jpc;
        for (auto &output : *rel) {       
            output >> jpc >> iteration >> newstackptr; 
            if (iteration == *i) {
                newstackptr = merge_stack(prog, newstackptr, stack_parts[1]);
                run_block_analysis(prog, bbanalyzer, analyzed_blocks, jpc, i, newstackptr);
            }
        }
    } else {
        std::cerr << "Failed to get output relation\n";
        exit(1);
    }
}

void analyze(std::vector<OPCode> code) {

    if (souffle::SouffleProgram *prog = souffle::ProgramFactory::newInstance("semantics")) {

        BBAnalyzer blocks(code);
        std::unordered_map<std::string,bool> analyzed_blocks;
        int iteration;

        generate_instrrels(prog, code);

        //start at pc 0 with empty stack
        run_block_analysis(prog, blocks, analyzed_blocks, "0", &iteration, 0);
        
        prog->printAll();
        
        // Clean up
        delete prog;
    } else {
        std::cerr << "Failed to create instance\n";
        exit(1);
    }
}

#endif // SOUFFLEFACTGENERATOR_H_