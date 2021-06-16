TARGET = compiler
SCANNER_SOURCE = src/scanner/scanner.l
PARSER_SOURCE = src/parser/parser.y
SCANNER = src/scanner/scanner.cpp
PARSER = src/parser/parser.cpp
PARSER_HEAD = src/parser/parser.hpp
SRC = src/main.cpp src/ast/ast.cpp src/visitor/visitor.cpp
CXXFLAGS = -o

all: $(TARGET)

clean: 
	rm $(SCANNER) $(PARSER) $(PARSER_HEAD) $(TARGET)

$(PARSER): $(PARSER_SOURCE)
	bison -d -o $@ $^

$(PARSER_HEAD): $(PARSER)

$(SCANNER): $(SCANNER_SOURCE) $(PARSER_HEAD)
	lex -o $@ $^

$(TARGET):  $(SCANNER) $(PARSER) $(SRC)
	$(CXX) $(CXXFLAGS) $@ $(SCANNER) $(PARSER) $(SRC) `llvm-config --cxxflags --ldflags --system-libs --libs core mcjit native` -w -DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING=1 