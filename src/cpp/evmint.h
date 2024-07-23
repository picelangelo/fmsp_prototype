#ifndef EVMINT_H_
#define EVMINT_H_

#include <cstdint>
#include <string>
#include <format>
#include "main.h"
#include "souffle/SouffleInterface.h"

class EVMInt {
    private:
        uint64_t i0;
        uint64_t i1;
        uint64_t i2;
        uint64_t i3;
        void shl8() {
            uint64_t overflow3 = (i3 >> (64 - 8));
            uint64_t overflow2 = (i2 >> (64 - 8));
            uint64_t overflow1 = (i1 >> (64 - 8));
            i3 <<= 8;
            i2 = (i2 << 8) | overflow3;
            i1 = (i1 << 8) | overflow2;
            i0 = (i0 << 8) | overflow1;
        }
    public:
        EVMInt() {
            i0 = 0, i1 = 0, i2 = 0, i3 = 0;
        }
        
        EVMInt(uint64_t _i0, uint64_t _i1, uint64_t _i2, uint64_t _i3) {
            i0 = _i0, i1 = _i1, i2 = _i2, i3 = _i3;
        }

        EVMInt(int i) {
            i0 = 0, i1 = 0, i2 = 0, i3 = i;
        }

        EVMInt(std::string str) {
            i0 = 0, i1 = 0, i2 = 0, i3 = 0;
            if (str.length() % 2 == 1) {
                str = "0" + str;
            }
            uint64_t len = str.length();
            const char *cstr = str.c_str();
            for (int i(0); i < len - 1; i += 2) {
                char c0 = *(cstr + i);
                char c1 = *(cstr + i + 1);
                addByte((char_to_digit(c0) << 4) + char_to_digit(c1));
            }
        }

        void addByte(uint8_t byte) {
            shl8();
            i3 += byte;
        }

        bool eq(EVMInt& i) {
            return i0 == i.i3 && i1 == i.i1 && i2 == i.i2 && i3 == i.i3;
        }

        EVMInt add(EVMInt& i) {
            uint64_t i3_add = i3 + i.i3;
            bool overflow3 = (i3_add < i3);

            uint64_t i2_add_wo_of = i2 + i.i2;
            bool overflow2 = (i2_add_wo_of < i2);
            uint64_t i2_add = i2_add_wo_of + overflow3;
            overflow2 = (overflow2 || i2_add < i2_add_wo_of);

            uint64_t i1_add_wo_of = i1 + i.i1;
            bool overflow1 = (i1_add_wo_of < i1);
            uint64_t i1_add = i1_add_wo_of + overflow2;
            overflow1 = (overflow1 || i1_add < i1_add_wo_of);

            uint64_t i0_add = i0 + i.i0 + overflow1;

            return EVMInt(i0_add, i1_add, i2_add, i3_add);
        }

        std::string str() {
            if (i0 == 0 && i1 == 0 && i2 == 0) {
                return std::format("{:x}", i3);
            } else if (i0 == 0 && i1 == 0) {
                return std::format("{:x}{:0>16x}", i2, i3);
            } else if (i0 == 0) {
                return std::format("{:x}{:0>16x}{:0>16x}", i1, i2, i3);
            } else {
                return std::format("{:x}{:0>16x}{:0>16x}{:0>16x}", i0, i1, i2, i3);
            }
        }
        
};

#endif // EVMINT_H_
