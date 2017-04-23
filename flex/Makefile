all: lex

lex:
	lex -+ -o Tokenizer.cpp Tokenizer.lex
	g++ -std=c++11 -D DEBUG -o lexan.exe Tokenizer.cpp TokenTypes.cpp -w
driver:
	lex -+ -o Tokenizer.cpp Tokenizer.lex
	g++ -std=c++11 -o lexan.exe Tokenizer.cpp driver_lex.cpp TokenTypes.cpp -w
clear:
	rm lexan.exe Tokenizer.cpp