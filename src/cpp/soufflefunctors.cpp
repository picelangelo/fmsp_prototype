#include <string>
#include <cassert>
#include "evmint.h"
#include "souffle/SymbolTable.h"
#include "souffle/RecordTable.h" 

extern "C" {
   souffle::RamDomain add(souffle::SymbolTable* symbolTable, souffle::RecordTable* recordTable,
        souffle::RamDomain arg1, souffle::RamDomain arg2) {
      assert(symbolTable && "NULL symbol table");
      assert(recordTable && "NULL record table");
      const std::string& sarg1 = symbolTable->decode(arg1);
      const std::string& sarg2 = symbolTable->decode(arg2);
      EVMInt x(sarg1);
      EVMInt y(sarg2);
      EVMInt r(x.add(y));
      std::string res = r.str();
      return symbolTable->encode(res); 
   }

   int32_t eq(char *arg1, char *arg2) {
      EVMInt x(arg1);
      EVMInt y(arg2);
      return x.eq(y);
   }

}