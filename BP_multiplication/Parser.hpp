//
//  parser.hpp
//  BP_multiplication
//
//  Created by Matyáš Hollmann on 20/03/2017.
//  Copyright © 2017 Matyáš Hollmann. All rights reserved.
//

#ifndef parser_hpp
#define parser_hpp
/*
 Gramatika symbol: (white-spacy nejsou podstatne)
 
 S -> COEFF * A | ( COEFF * A )
 COEFF -> exprt_part COEFF | func_delta COEFF | func_c COEFF | ( COEFF ) COEFF | ^( COEFF ) COEFF | eps
 A -> tB | t^( EXP ) B | C
 B -> ( D ) | D
 C -> x | SECOND | THIRD | FOURTH
 D -> END | SECOND | THIRD | FOURTH
 SECOND -> x^[ func_sqrbrac ] END
 THIRD -> u^[ func_sqrbrac ] END
 FOURTH -> z^[ func_sqrbrac ] END
 END -> x | x^(\e) | x^(\s) | eps
 EXP -> exprt_part EXP | ( EXP ) EXP | eps
 
 Pravidlo z tabulky nasobeni:
 
 S -> SYMB SYMB = A
 A -> SYMB D | SYMB B
 B -> SYMB B | SYMB D
 D -> , #čárka ukoncuje pravidlo
 
 */



#include "Element.hpp"
#include "Multiplication.hpp"
#include <vector>
#include <iostream>
#include "../flex/tokentypes.h"
#include <FlexLexer.h>
#include <string>
#include <sstream>
#include <list>

//=================================================================================================================//

extern Token * gCurrentToken;
extern int line_num;

//=================================================================================================================//

class Parser {
public:
   Parser(KernelConnection * k, std::istream * in = &std::cin, MultiplicationTable * table = nullptr );
   ~Parser();
   void setParserStream(std::istream * in);
   
   std::vector<Element> parseExpression(MultiplicationTable * tbl);
   Element parseElement();
   void parsingError(const std::string & a, bool printSymbol = true);
   
   std::string parseIdentity( std::vector<char> * vars );
   
   RuleParserState pState;
   std::string gActualExpress;
   
private:
   struct StackEl
   {
      StackEl(StackType st = STACK_ZERO, const std::vector<Element> & v = std::vector<Element>() );
      StackEl(StackType st, const Element & e );
      ~StackEl();
      
      
      StackType mStackType;
      std::vector<Element> mData;
   };
   
   void nextToken();
   std::string parseCoeff();
   std::string parseExp();
   void stackMatchBrackets();
   std::vector<Element> evalExpr();
   
   KernelConnection * mKernel;
   yyFlexLexer mLexer;
   MultiplicationTable * mTable;
   std::list<StackEl> mExprStack;
   TokenID mID;
   
   int mBracketCount;
   bool mBracketsAround;
   bool mGetNext;
};

//=================================================================================================================//

#endif /* parser_hpp */
