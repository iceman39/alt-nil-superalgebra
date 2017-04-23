INCLUDEDIR=/Applications/Mathematica.app/Contents/SystemFiles/Links/WSTP/DeveloperKit/MacOSX-x86-64/CompilerAdditions/
LIBDIR=/Applications/Mathematica.app/Contents/SystemFiles/Links/WSTP/DeveloperKit/MacOSX-x86-64/CompilerAdditions/
MATHLINK_LIB = -lWSTPi4
EXTRA_LIBS = -lc++ -framework Foundation

all:
	g++ -std=c++11 -I${INCLUDEDIR} -L${LIBDIR} ${MATHLINK_LIB} ${EXTRA_LIBS} Main.cpp KernelConnection.cpp Parser.cpp Identity.cpp MathDriver.cpp Element.cpp Multiplication.cpp ../flex/Tokenizer.cpp ../flex/TokenTypes.cpp -w -o executable