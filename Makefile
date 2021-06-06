all: compiler

clean: 
	rm src/scanner/scanner.cpp src/parser/parser.hpp src/parser/parser.cpp compiler

src/parser/parser.cpp: src/parser/parser.y
	bison -d -o $@ $^

src/parser/parser.hpp: src/parser/parser.cpp

src/scanner/scanner.cpp: src/scanner/scanner.l src/parser/parser.hpp
	lex -o $@ $^

compiler: src/main.cpp src/parser/parser.cpp src/ast/ast.cpp src/visitor/visitor.cpp src/scanner/scanner.cpp
	g++ -o $@ src/main.cpp  src/scanner/scanner.cpp src/parser/parser.cpp src/ast/ast.cpp src/visitor/visitor.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core mcjit native` -w -DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING=1 