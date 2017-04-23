//
//  parser.cpp
//  BP_multiplication
//
//  Created by Matyáš Hollmann on 20/03/2017.
//  Copyright © 2017 Matyáš Hollmann. All rights reserved.
//

#include "Parser.hpp"


// TEST: t**(t^(5)x + 2*t^(2)),  == (2) * t^(3) + t^(6)x

// (t^(2)x -1*tx^[3])**x,   == (1/2) * t^(3) + (-1) * t(x^[3]x) + (2/3) * t(x^[4])

// x**(t^(2)x -1*tx^[3]),   == (1/2) * t^(3) + (-1) * t(x^[3]x) + (1/3) * t(x^[4]) + (-1/6) * z^[4]

// t tx t^(2)x x^[3] x^[4] x^[4]x x^[3]x,  ==  t + tx + t^(2)x + (x^[3]) + (x^[3]x) + (x^[4]) + (x^[4]x)

// (t^(2)x^[k] + 1*tz^[k])**x,  == t^(2)(x^[k]x) + (2/3) * tz^[1 + k] + tz^[k]x

// x**(t^(2)x^[k] + 1*tz^[k]),  == ((-1)^(1 + k)) * t^(2)(x^[1 + k]) + ((-1)^k) * t^(2)(x^[k]x) + ((-1)^(1 + k)) * tu^[k] + (-(-1)^k/3) * tz^[1 + k] + ((-1)^k) * tz^[k]x

// (6*t(x^[k]x) + (-3)*tx^[k+1] + (1)/(2)*z^[k+1] + (-3)/(2)*u^[k] + 3*z^[k]x)**x,  == (-3) * t(x^[1 + k]x) + (2) * t(x^[2 + k]) + (3) * t^(2)(x^[k]) + u^[1 + k] + (-3/2) * u^[k]x + (-3/2) * z^[1 + k]x + (11/2) * tz^[k]

// 2*t^(2)**x^[k] + (2*tx^[k])**t + 2*(x^[k]**t)**t,  == (6) * t^(2)(x^[k]) + (6) * tz^[k]

// x**x**x**x**x**x**x,  == (1/8) * t^(3)x + (1/18) * (x^[6]x) + (-1/18) * (x^[7]) + (-1/4) * t(x^[4]x) + (1/4) * t(x^[5]) + (-3/8) * t^(2)(x^[3]) + (-1/4) * u^[3]x + (1/8) * u^[4] + (-1/9) * z^[4]x + (1/24) * z^[5]

// (2*x**t)**t + 2*t^(2)x + (2*tx)**t,  == (6) * t^(2)x + (-6) * t(x^[3])

// (t^(2)x - tx^[3])**x,  == (1/2) * t^(3) + (-1) * t(x^[3]x) + (2/3) * t(x^[4])

// 2*t^(2)x^[k]+(2*tx^[k])**t + (2*x^[k]**t)**t,  == (6) * t^(2)(x^[k]) + (6) * tz^[k]

// (2) * t^(2)(x^[k]x) + (2*t(x^[k]x))**t + ((2*x^[k]x)**t)**t,  == (6) * t^(2)(x^[k]x) + (-3) * tu^[k] + (3) * tz^[1 + k] + (6) * tz^[k]x

// (x**t + tx)**x^[k] + (x**x^[k])**t + ((-1)^(k)*x^[k]x)**t + ((-1)^(k)*tx^[k])**x + ((-1)^(k)*x^[k]**t)**x,


// (x**t+tx)**x^[k] + (x**x^[k])**t + ((-1)^(k)*x^[k]x)**t + (-1)^(k)*(tx^[k])**x + (x^[k]**t)**x,  (−1)^(k)*(tx^[k])**x +(x^[k]**t)**x


// (x**t + tx)**(x^[k]x)+(x**(x^[k]x))**t + (-1)^(k+1)*((x^[k]x)**x)**t + (-1)^(k+1)*(tx^[k]x)**x + (-1)^(k+1)*((x^[k]x)**t)**x,

// (6t(x^[k]x) -3*tx^[k+1] + (1)/(2)*z^[k+1] -(3)/(2)*u^[k] + 3*z^[k]x)**x,

// x**(6t(x^[k]x) -3*tx^[k+1] + (1)/(2)*z^[k+1] -(3)/(2)*u^[k] + 3*z^[k]x),

/*
 Gramatika symbol: (white-spacy nejsou podstatne)
 
 S -> EXPR * A | EXPR * ( A ) | A | ( A ) #expr je koeficient v tomto pripade
 A -> tB | t^(EXPR_1)B | C
 B -> END | ( SECOND ) | THIRD | FOURTH   #ostatni typy se pisou bez zavorek
 C -> x | SECOND | THIRD | FOURTH
 END -> x | x^(BIN_VAR) | eps
 BIN_VAR -> \epsilon | \sigma
 SECOND -> x^[EXPR_2] END  #expr by mela bejt ve tvaru k + 2, k > 0
 THIRD -> u^[EXPR_3] END
 FOURTH -> z^[EXPR_3] END
 EXPR -> libovolny vyraz s promennymi {ijkmn} a funkcí \delta( EXPR ), který je akceptován kernelem Mathematicy
 EXPR_1 -> EXPR pro kterou navíc platí >= 0
 EXPR_2 -> EXPR ve tvaru: k + 2, pro k celé > 0
 EXPR_3 -> EXPR ve tvaru: 4k nebo 4k + 1, pro k celé > 0
 
 
 Pravidlo z tabulky nasobeni:
 
 S -> SYMB SYMB = A
 A -> SYMB D | SYMB B
 B -> SYMB B | SYMB D
 D -> , #čárka ukoncuje pravidlo
 */

//=================================================================================================================//

Parser::Parser(KernelConnection * k, std::istream * in, MultiplicationTable * table ) : mKernel(k), mLexer(in), mTable(table)
{
   mGetNext = true;
   mBracketCount= 0;
   mBracketsAround = false;
   pState = ST_LEFT;
}

//=================================================================================================================//

Parser::~Parser()
{
   
}

//=================================================================================================================//

Element Parser::parseElement()
{
   bool coreBracket = false;
   std::string coeff = parseCoeff();
   
   if ( parserProgress == true )
      *output << std::endl << "Coeff parsed: " << coeff << std::endl;
   
   // reached the end of the expression
   if ( pState == ST_EXPR && mID == DELIM_COMMA )
   {
      pState = ST_COMMA;
      return Element();
   }

   std::string tExp = "0";
   std::string bracketExp = "0";
   std::string boolX = "0";
   ElementType tp = EL_FIRST;
   int state = INITIAL;
   bool end = false;
   
   while ( true )
   {
      nextToken();
      
      if ( parserProgress == true )
         *output << "Current token: " << gCurrentToken->print() << " state: " << state << " parserState: " << pState <<std::endl;
      
      switch (state) {
         case INITIAL:
            if ( mID == SYM_T )
               state = PARSE_T;
            else if ( pState == ST_EXPR && mID == SYM_LBRACKET )
            {
               mExprStack.push_back(StackEl(STACK_LEFT_BR));
               
               gActualExpress += "(";
            }
            else if ( mID == SYM_X || mID == SYM_U || mID == SYM_Z || mID == SYM_LBRACKET )
            {
               state = PARSE_CORE;
               mGetNext = false;
            }
            else if ( mID == SYM_X_END )
            {
               boolX = gCurrentToken->getData();
               state = PARSE_END1;
            }
            else
               parsingError("Parsing element - unexpected symbol occurred in state INITIAL: ");
            break;
            
            
         case PARSE_T:
            tExp = "1";
            if ( mID == OP_POW )
            {
               tExp = parseExp();
               state = PARSE_CORE;
            }
            else if ( mID == SYM_X || mID == SYM_U || mID == SYM_Z || mID == SYM_LBRACKET )
            {
               state = PARSE_CORE;
               mGetNext = false;
            }
            else if ( mID == SYM_X_END )
            {
               boolX = gCurrentToken->getData();
               state = PARSE_END1;
            }
            else if ( mID == SYM_EOF || mID == SYM_T || mID == EXPR_PART || mID == FUNC_C || mID == FUNC_DELTA || mID == OP_EQUAL || mID == DELIM_COMMA || mID == DELIM_ELEMENT_MUL || mID == SYM_RBRACKET )
            {
               mGetNext = false;
               state = PARSE_END1;
            }
            else
               parsingError("Parsing element - unexpected symbol occurred in state PARSE_T: ");
            break;
            
            
         case PARSE_CORE:
            state = PARSE_CORE1;
            if ( mID == SYM_X )
            {
               tp = EL_SECOND;
            }
            else if ( mID == SYM_U )
            {
               tp = EL_THIRD;
            }
            else if ( mID == SYM_Z )
            {
               tp = EL_FOURTH;
            }
            else if ( mID == SYM_LBRACKET && coreBracket == false )
            {
               coreBracket = true;
               mBracketCount++;
               state = PARSE_CORE;
            }
            else if ( mID == SYM_RBRACKET || mID == SYM_LBRACKET || mID == SYM_T || mID == EXPR_PART || mID == FUNC_C || mID == FUNC_DELTA || mID == OP_EQUAL || mID == DELIM_COMMA || mID == SYM_X_END || mID == DELIM_ELEMENT_MUL )
            {
               mGetNext = false;
            }
            else if ( mID == SYM_EOF )
            {
               state = PARSE_END1;
            }
            else
               parsingError("Parsing element - unexpected symbol occurred in state PARSE_CORE: ");
            break;
            
            
         case PARSE_CORE1:
            if ( mID == OP_POW )
               state = PARSE_CORE_POW;
            else if ( mID == SYM_RBRACKET || mID == SYM_LBRACKET || mID == SYM_T || mID == SYM_X || mID == SYM_U || mID == SYM_Z || mID == EXPR_PART || mID == FUNC_C || mID == FUNC_DELTA || mID == OP_EQUAL || mID == DELIM_COMMA || mID == DELIM_ELEMENT_MUL )
            {
               mGetNext = false;
               state = PARSE_END1;
            }
            else if ( mID == SYM_X_END )
            {
               state = PARSE_END1;
               boolX = gCurrentToken->getData();
            }
            else if ( mID == SYM_EOF )
            {
               state = PARSE_END1;
            }
            else
               parsingError("Parsing element - unexpected symbol occurred in state PARSE_CORE_1: ");
            break;
            
            
         case PARSE_CORE_POW:
            if ( mID == FUNC_SQRBRAC )
            {
               bracketExp = gCurrentToken->getData();
               state = PARSE_END;
            }
            else
               parsingError("Parsing element - unexpected symbol occurred in state PARSE_CORE_POW, expected FUNC_SQRBRAC but received: ");
            break;
            
            
         case PARSE_END:
            state = PARSE_END1;
            if ( mID == SYM_X_END )
            {
               boolX = gCurrentToken->getData();
            }
            else
               mGetNext = false;
            break;
            
            
         case PARSE_END1:
            if ( mID == SYM_LBRACKET || mID == SYM_T || mID == SYM_X || mID == SYM_X_END || mID == SYM_U || mID == SYM_Z || mID == EXPR_PART || mID == FUNC_C || mID == FUNC_DELTA || mID == OP_EQUAL || mID == DELIM_COMMA || mID == SYM_EOF || end == true || mID == DELIM_ELEMENT_MUL )
            {
               mBracketsAround = true;
               if ( mBracketCount == 0 || ( mBracketCount == 1 && tp == EL_FIRST && coreBracket == true )) // 1 openening bracket belongs to the next element
               {
                  if ( mBracketCount == 0 )
                     mBracketsAround = false;
                  
                  mGetNext = false;
                  if ( pState == ST_RIGHT && mID == OP_EQUAL )
                  {
                     mGetNext = true;
                     pState = ST_EQUAL_SIGN;
                  }
                  else if ( pState == ST_RES && mID == DELIM_COMMA )
                  {
                     mGetNext = true;
                     nextToken();
                     if ( mID == SYM_EOF )
                        pState = ST_EOF;
                     else
                        pState = ST_LEFT;
                     mGetNext = false;
                  }
                  else if ( mID == DELIM_COMMA && pState == ST_COMMA  )
                     mGetNext = true;
                  else if ( mID == DELIM_COMMA && pState == ST_EXPR )
                  {
                     pState = ST_COMMA;
                  }
                  else if ( mID == OP_EQUAL || mID == DELIM_COMMA )
                  {
                     parsingError("Parsing element - unexpected symbol (= or ,) occurred. Parser state: " + std::to_string(pState) + ". ");
                  }
                  else if ( mID == SYM_EOF )
                     pState = ST_EOF;
                  if ( mKernel->validateString(coeff) && mKernel->validateString(tExp) && mKernel->validateString(bracketExp) && mKernel->validateString(boolX) )
                  {
                     return Element(tp, coeff, tExp, bracketExp, boolX);
                  }
                  else
                     parsingError("Parsing element - Mathematica kernel validation error occurred.", false);
               }
               else if ( pState == ST_EXPR && mBracketCount == 1 && mBracketsAround == true )
               {
                  mGetNext = false;
                  mBracketCount = 0;
                  mBracketsAround = false;
                  mExprStack.push_back(StackEl(STACK_LEFT_BR));
                  
                  gActualExpress += "(";
               }
               else
                  parsingError("Parsing element - bracket mismatch in state PARSE_END1: bracket count: " + std::to_string(mBracketCount));
            }
            else if ( mID == SYM_RBRACKET )
            {
               if ( mBracketsAround == true )
               {
                  mBracketCount--;
                  mBracketsAround = false;
               }
               else if ( coreBracket == true )
               {
                  mBracketCount--;
                  coreBracket = false;
               }
               else if ( pState == ST_EXPR && end == false )
               {
                  mGetNext = false;
                  end = true;
               }
               else
                  parsingError("Parsing element - bracket mismatch in state PARSE_END1. Parser state: " + std::to_string(pState), false);
            }
            else
               parsingError("Parsing element - unexpected symbol occurred in state PARSE_END_1.");
            break;
      }
   }
}

//=================================================================================================================//

std::string Parser::parseExp()
{
   std::string res = "(";
   int brackets = 0;
   nextToken();
   if ( mID == SYM_LBRACKET )
      brackets++;
   else
      parsingError("Argument of the exponent has to be in () brackets.");
   
   while( true )
   {
      nextToken();
      if ( mID == SYM_RBRACKET )
      {
         res += ")";
         brackets--;
      }
      else if ( mID == SYM_LBRACKET )
      {
         res += "(";
         brackets++;
      }
      else if ( mID == EXPR_PART )
         res += gCurrentToken->getData();
      else
         parsingError("Parsing exponent - unexpected symbol occurred.");
      
      if ( brackets == 0 )
         break;
   }
   
   return res;
}

//=================================================================================================================//

std::string Parser::parseCoeff()
{
   std::string res = "";
   int state = 0;
   if ( parserProgress == true )
      *output << std::endl;
   
   while ( true )
   {
      nextToken();
      if ( parserProgress == true )
         *output << "ParseCoeff: current token: " << gCurrentToken->print() << " parseCoeffstate: " << state << " parserState: " << pState <<std::endl;
      
      switch (state)
      {
         case 0:
            state = 1;
            if ( mID == SYM_LBRACKET )
            {
               mBracketsAround = true;
               mBracketCount++;
               res += "(";
            }
            else if ( mID == SYM_T || mID == SYM_X || mID == SYM_U || mID == SYM_Z || mID == SYM_X_END )
            {
               mGetNext = false;
               return "1";
            }
            else if ( mID == FUNC_C || mID == EXPR_PART || mID == FUNC_DELTA )
            {
               mGetNext = false;
            }
            else if ( mID == SYM_RBRACKET && pState == ST_EXPR )
            {
               mExprStack.push_back(StackEl(STACK_RIGHT_BR));
               stackMatchBrackets();
               
               gActualExpress += ")";
            }
            else if ( mID == DELIM_ELEMENT_MUL && pState == ST_EXPR )
            {
               mExprStack.push_back(StackEl(STACK_MULT));
               
               if ( latexForm )
                  gActualExpress += "*";
               else
                  gActualExpress += "**";
            }
            else if (mID == DELIM_COMMA && pState == ST_EXPR )
            {
               return "0";
            }
            else
               parsingError("Parsing coefficient - case 0 unexpected symbol occurred.");
            break;
            
         case 1:
            if (mID == SYM_LBRACKET )
            {
               mBracketCount++;
               res += "(";
            }
            else if ( mID == SYM_RBRACKET )
            {
               if ( mBracketCount > 0 )
               {
                  mBracketCount--;
                  res += ")";
               }
               else if ( pState == ST_EXPR && res.length() == 0 )
               {
                  mExprStack.push_back(StackEl(STACK_RIGHT_BR));
                  stackMatchBrackets();
                  
                  gActualExpress += ")";
               }
               else
                  parsingError("Parsing coefficient - unmatched closing bracket found.", false);
               
            }
            else if ( (mID == SYM_T || mID == SYM_X || mID == SYM_U || mID == SYM_Z || mID == SYM_X_END ) && ( res.empty() || ( res.length() == 1 && (res[0] == '(' || res[0] == '+' || res[0] == '-' ) ) ) )
            {
               mGetNext = false;
               if ( res.length() == 1 && res[0] == '-' )
                  return "-1";
               
               return "1";
            }
            else if ( mID == FUNC_C ) // c(j) = j*(j - 1)/2
            {
               res += "(" + gCurrentToken->getData() + ")*(" + gCurrentToken->getData() + " -1)/2";
            }
            else if ( mID == FUNC_DELTA ) // delta(j) = 1 + (-1)^j
            {
               res += "(1 + (-1)^(" + gCurrentToken->getData() + "))";
            }
            else if ( mID == EXPR_PART )
               res += gCurrentToken->getData();
            else if ( mID == OP_POW )
            {
               res += "^";
            }
            else if ( mID == DELIM_MUL || ( pState == ST_EXPR && res.length() && (res[0] == '+' || res[0] == '-' || res[0] == '(') && (mID == SYM_T || mID == SYM_X || mID == SYM_U || mID == SYM_Z || mID == SYM_X_END ) ) )
            {
               if ( mID == SYM_T || mID == SYM_X || mID == SYM_U || mID == SYM_Z || mID == SYM_X_END )
                  mGetNext = false;
               
               if ( res.empty() )
                  parsingError("Parsing coefficient - delimiter symbol '*' used without any preceding coefficient.", false);
               else
               {
                  if ( mBracketsAround && pState != ST_EXPR )
                  {
                     if ( mBracketCount > 0 )
                        res = res.substr(1);
                     else
                        mBracketsAround = false;
                  }
                  else if (pState == ST_EXPR )
                  {
                     if ( res.length() != 0 && res[0] == '+' )
                        res = res.substr(1);
                     mBracketsAround = false;
                     while ( res.length() != 0 && res[0] == '(' && mBracketCount > 0) // fix this shit
                     {
                        mBracketCount--;
                        mExprStack.push_back(StackEl(STACK_LEFT_BR));
                        res = res.substr(1);
                        
                        gActualExpress += "(";
                     }
                  }
                  
                  if ( res.length() == 0 )
                     res = "1";
                  
                  return res;
               }
            }
            else if ( mID == DELIM_ELEMENT_MUL && pState == ST_EXPR )
            {
               mExprStack.push_back(StackEl(STACK_MULT));
               
               if ( latexForm )
                  gActualExpress += "*";
               else
                  gActualExpress += "**";
            }
            else if (mID == DELIM_COMMA && pState == ST_EXPR && res.length() == 0 )
            {
               return "0";
            }
            else
               parsingError("Parsing coefficient - expected *, received: ");
            break;
      }
   }
}

//=================================================================================================================//

void Parser::nextToken()
{
   if ( pState == ST_EOF )
   {
      if ( verbose == true )
         *output << "Error: Tokenizer is out of tokens." << std::endl;
      return;
   }
   if ( mGetNext )
   {
      delete gCurrentToken;
      gCurrentToken = nullptr;
      
      if ( mLexer.yylex() == 0 )
      {
         if ( verbose == true )
            *output << "Tokenizer is out of tokens. Changing state to ST_EOF." << std::endl;
         
         pState = ST_EOF;
      }
      mID = gCurrentToken->getID();
   }
   mGetNext = true;
}

//=================================================================================================================//

void Parser::parsingError(const std::string & a, bool printSymbol)
{
   std::string symbol = "\tActual token: " + gCurrentToken->print() + " at line: " + std::to_string(line_num);
   
   if ( printSymbol == true )
      throw std::runtime_error("Error: " + a + symbol);
   else
      throw std::runtime_error("Error: " + a);
}

//=================================================================================================================//

void Parser::setParserStream(std::istream * in)
{
   mGetNext = true;
   mBracketCount= 0;
   mBracketsAround = false;
   pState = ST_LEFT;
   
   line_num = 1;
   
   mLexer.switch_streams(in);
}

//=================================================================================================================//

Parser::StackEl::StackEl(StackType st, const std::vector<Element> & v) : mStackType(st), mData(v)
{
   
}

//=================================================================================================================//

Parser::StackEl::~StackEl()
{
   
}

//=================================================================================================================//

Parser::StackEl::StackEl(StackType st, const Element & e ) : mStackType(st)
{
   mData.push_back(e);
}
//=================================================================================================================//

std::vector<Element> Parser::parseExpression(MultiplicationTable * tbl)
{
   mTable = tbl;
   pState = ST_EXPR;
   gActualExpress = "";
   
   if ( parserProgress == true )
      *output << "Parse Expression called:" << std::endl;
   
   mExprStack.clear(); // clear list
   
   while ( pState != ST_COMMA )
   {
      Element tmp = parseElement();
      
      if ( parserProgress == true )
      {
         *output << "\n Element parsed: " << tmp.toString() << std::endl << std::endl;
      }
      
      if ( tmp.getType() != EL_ZERO )
      {
         if ( !mExprStack.empty() && mExprStack.back().mStackType == STACK_ELEMENT && !gActualExpress.empty() && gActualExpress.back() != ')' )
            gActualExpress += " + ";
         
         mExprStack.push_back(StackEl(STACK_ELEMENT,tmp));
         
         if ( latexForm )
            gActualExpress += tmp.toLatex(mKernel);
         else
            gActualExpress += tmp.toString();
      }
   }
   mGetNext = true;
   
   return evalExpr();
}

//=================================================================================================================//
                                          // eval in stack order - mult from right to left
//=================================================================================================================//


std::vector<Element> Parser::evalExpr()
{
   std::vector<Element> res;
   std::vector<Element> left;
   
   if ( parserProgress == true )
      *output << "Eval Expression called. " <<  "Stack size: " << mExprStack.size() << std::endl;
   
   bool mult = false;
   while ( mExprStack.empty() == 0 )
   {
      StackEl tmp = mExprStack.front();
      mExprStack.pop_front();
      
      switch (tmp.mStackType) {
         case STACK_ELEMENT:
            if ( mult == false )
            {
               if ( left.empty() == 0 )
               {
                  res.insert(res.end(), std::make_move_iterator(left.begin()), std::make_move_iterator(left.end()));
                  left.clear();
               }
               left = tmp.mData;
            }
            else if ( tmp.mData.empty() == 0 )
            {
               left = mTable->multiplyExpr(left, tmp.mData, mKernel);
               mult = false;
            }
            else
               parsingError("Error - right operand for the multiplication is missing.", false);
            break;
         
         
         case STACK_MULT:
            if ( mult == false && left.empty() == 0 )
            {
               mult = true;
            }
            else
               parsingError("Error in evaluating expression STACK_MULT - left operand for multiplication is missing.", false);
            break;
            
            
         default:
            parsingError("Error in parse - unexpected symbol on the stack" + std::to_string(tmp.mStackType), false);
            break;
      }
   }
   if ( mult == true )
      parsingError("Stack evaluation error - right operand for the multiplication is missing.", false);
   
   if ( left.empty() == 0 )
   {
      res.insert(res.end(), std::make_move_iterator(left.begin()), std::make_move_iterator(left.end()));
      left.clear();
   }
   
   mTable->simplifyExpression(res, mKernel);
   return res;
}

//=================================================================================================================//

void Parser::stackMatchBrackets() // closing bracket came
{
   if ( parserProgress == true )
      *output << "Match brackets called." << std::endl;
   
   std::vector<Element> res;
   StackEl right;
   bool finished = false;
   bool inside = false;
   bool mult = false;
   
   while( !mExprStack.empty() )
   {
      if ( finished )
         break;
      
      StackEl tmp = mExprStack.back();
      mExprStack.pop_back();
      
      switch (tmp.mStackType) {
         case STACK_LEFT_BR:
            if ( mult == false && inside == true )
            {
               finished = true;
               
               if ( right.mStackType == STACK_ELEMENT  )
               {
                  res.insert(res.end(), std::make_move_iterator(right.mData.begin()), std::make_move_iterator(right.mData.end()));
                  right.mData.clear();
               }
               
               mTable->simplifyExpression(res, mKernel);
               
               if ( res.empty() == true ) // push ZERO
                  res.push_back(Element());
               
               mExprStack.push_back(StackEl(STACK_ELEMENT, res));
            }
            else
               parsingError("Error matching brackets in expression evaluation failed.", false);
            break;
            
         case STACK_ELEMENT:
            if ( mult == false )
            {
               if ( right.mStackType == STACK_ELEMENT )
               {
                  res.insert(res.end(), std::make_move_iterator(right.mData.begin()), std::make_move_iterator(right.mData.end()));
                  right.mData.clear();
               }
               right = tmp;
            }
            else
            {
               right.mData = mTable->multiplyExpr(tmp.mData, right.mData, mKernel);
               mult = false;
            }
            break;
            
         case STACK_MULT:
            if ( mult == false && right.mStackType == STACK_ELEMENT )
               mult = true;
            else
               parsingError("Error: parsing brackets - symbol MULTIPLY used without two operands around itself.", false);
            break;
            
         case STACK_RIGHT_BR:
            if ( inside == false )
               inside = true;
            else
               parsingError("Error in matching brackets in expression. Multiple closing brackets on the stack.", false);
            break;
         
         default:
            parsingError("Error in matching brackets. Unexpected symbol on the stack" + std::to_string(tmp.mStackType), false);
            break;
      }
      
   }
   
   if ( finished == false )
      parsingError("Error in matching brackets: left bracket was not found.", false);
}

//=================================================================================================================//

std::string Parser::parseIdentity( std::vector<char> * vars )
{
   int state = 0;
   int brackets = 0;
   std::string res;
   
   while ( true )
   {
      nextToken();
      switch (state) {
         case 0:
            state = 1;
            if ( mID != SYM_IDENTITY )
               parsingError("Parsing identity error: expected identity symbol (W), received: ", true);
            break;
         case 1:
            state = 2;
            if ( mID != SYM_LBRACKET )
               parsingError("Parsing identity error: expected opening bracket, received: ", true);
            break;
         case 2:
            state = 3;
            if ( mID != IDENT_VAR )
               parsingError("Parsing identity error: expected 'a' or 'b' or 'c' or 'd', received: ", true);
            else
            {
               vars->push_back(gCurrentToken->getData()[0]);
            }
            break;
         case 3:
            if ( mID == SYM_RBRACKET )
               state = 4;
            else if ( mID == DELIM_COMMA )
               state = 2;
            else
               parsingError("Parsing identity error: expected comma separator or a closing bracket, received: ", true);
            break;
         case 4:
            state = 5;
            if ( mID != OP_EQUAL )
               parsingError("Parsing identity error: expected equal sign, received: ", true);
            break;
         case 5:
            if ( mID == SYM_LBRACKET )
            {
               res += "(";
               brackets++;
            }
            else if ( mID == SYM_RBRACKET )
            {
               if ( brackets > 0 )
               {
                  res += ")";
                  brackets--;
               }
               else
                  parsingError("Parsing identity error: bracket mismatch, more closing brackets than opening.", false);
            }
            else if ( mID == FUNC_SGN )
            {
               if ( std::find(vars->begin(), vars->end(), gCurrentToken->getData()[0]) == vars->end() )
                  parsingError("Parsing identity error: sgn function with element that is not from this identity appered. Element: " + gCurrentToken->getData(), false);
               res += "sgn(";
               res += gCurrentToken->getData()[0];
               res += ")";
            }
            else if ( mID == IDENT_VAR )
            {
               if ( std::find(vars->begin(), vars->end(), gCurrentToken->getData()[0]) == vars->end() )
                  parsingError("Parsing identity error: element that is not from this identity appered. Element: " + gCurrentToken->getData(), false);
               res += gCurrentToken->getData()[0];
            }
            else if ( mID == DELIM_COMMA )
            {
               res += ",";
               if ( brackets == 0 )
               {
                  return res;
               }
               else
                  parsingError("Parsing identity error: unmatched bracket found. Identity general string: " + res, false);
            }
            else if ( mID == DELIM_ELEMENT_MUL )
            {
               res +="**";
            }
            else if ( mID == EXPR_PART )
            {
               res += gCurrentToken->getData();
            }
            else if ( mID == OP_POW )
            {
               res += "^";
            }
            else if ( mID == DELIM_MUL )
            {
               res += "*";
            }
            else
               parsingError("Parsing identity error: (state 5) unexpected symbol received, ", true);
            break;
         default:
            break;
      }
   }
}

//=================================================================================================================//


