CC = g++
CFLAGS_MY = -c -std=c++20
CFLAGS_SF = -c -D__EMBEDDED_SOUFFLE__=1
SOUFFLE = souffle
NAME = bin/protot

.PHONY: all prog runs clean dirs

all: prog 

prog: $(NAME) 

$(NAME): bin/main.o bin/libfunctors.so bin/semantics.o | dirs
	$(CC) $^ -o $@

bin/main.o: src/cpp/main.cpp $(shell ls src/cpp/*.h) | dirs
	$(CC) $(CFLAGS_MY) $< -o $@

gen/semantics.cpp: src/souffle/semantics.dl | dirs
	$(SOUFFLE) -D ./out -g $@ $< 

bin/semantics.o: gen/semantics.cpp | dirs
	$(CC) $(CFLAGS_SF) $< -o $@

bin/functors.o: src/cpp/soufflefunctors.cpp | dirs
	$(CC) $(CFLAGS_MY) $< -fPIC -o $@
	
bin/libfunctors.so: bin/functors.o | dirs
	$(CC) -shared -o $@ $<

dirs:
	mkdir -p bin out gen
	
clean:
	rm -rf bin/* out/* gen/*
