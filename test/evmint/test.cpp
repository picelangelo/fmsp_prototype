#include <iostream>
#include <ostream>
#include <cstring>
#include <vector>
#include "../../src/cpp/evmint.h"

void test_printer(std::string res, std::string expected) {
    if (!res.compare(expected)) {
        std::cout << "Test passed" << std::endl;
        return;
    }
    std::cout << res << " result \n";
    std::cout << expected << " expected" << std::endl;
}


void test_evmint_add() {
    EVMInt a;
    EVMInt b;

    a = EVMInt("0");
    b = EVMInt("0");
    test_printer(a.add(b).str(), "0");

    a = EVMInt("0");
    b = EVMInt("1");
    test_printer(a.add(b).str(), "1");

    a = EVMInt("0");
    b = EVMInt("10000000000000000");
    test_printer(a.add(b).str(), "10000000000000000");
    
    a = EVMInt("1");
    b = EVMInt("ffffffffffffffff");
    test_printer(a.add(b).str(), "10000000000000000");
    
    a = EVMInt("1");
    b = EVMInt("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    test_printer(a.add(b).str(), "0");
    
    a = EVMInt("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    b = EVMInt("1");
    test_printer(a.add(b).str(), "0");
    
    a = EVMInt("ffffffffffffffff");
    b = EVMInt("ffffffffffffffff");
    test_printer(a.add(b).str(), "1fffffffffffffffe");
    
    b = EVMInt("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    a = EVMInt("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    test_printer(a.add(b).str(), "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe");

    a = EVMInt("1");
    b = EVMInt("FFFFFFFFFFFFFFFFFFFFFFFFFFF");
    test_printer(a.add(b).str(), "1000000000000000000000000000");

    a = EVMInt("1111111111111111");
    b = EVMInt("1111111111111111");
    test_printer(a.add(b).str(), "2222222222222222");
}

auto main() -> int {
    test_evmint_add();
    return 0;
}