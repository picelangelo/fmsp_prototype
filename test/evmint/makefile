NAME = test
CPPV = c++20
SRCD = ../../src/cpp

.PHONY: all clean

all: $(NAME) 

$(NAME): $(SRCD)/main.h $(SRCD)/evmint.h test.cpp 
	g++ -std=$(CPPV) $^ -o $(NAME)

clean:
	rm $(NAME)
