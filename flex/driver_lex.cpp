#include "tokentypes.h"
#include <FlexLexer.h>
#include <fstream>      // std::filebuf


using namespace std;

extern Token * gCurrentToken;
int main()
{
	std::filebuf fb;
	fb.open ("a.txt",std::ios::in);
	std::istream is(&fb);

	yyFlexLexer Lexer(&is);
for ( int i = 0; i < 2; i++)
{
	while(Lexer.yylex() != 0)
 	{
    	cout << gCurrentToken->print() << endl;
    	delete gCurrentToken;
 	}
 	fb.close();
 	fb.open("b.txt",std::ios::in);
	std::istream ib(&fb);
 	Lexer = yyFlexLexer(&ib);
 	printf("next File\n");
}
fb.close();
}