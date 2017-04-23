//
//  constants.h
//  BP_multiplication
//
//  Created by Matyáš Hollmann on 25/03/2017.
//  Copyright © 2017 Matyáš Hollmann. All rights reserved.
//

#ifndef constants_h
#define constants_h

#include <iostream>

//=================================================================================================================//

/*
 element of the first type is:  t^m * x^o, m + o >= 1
 element of the second type is: t^m * (x^[k + 2] * x^o)
 element of the third type is:  t^m * (u^[4*k + e] * x^o)
 element of the fourth type is: t^m * (z^[4*k + e] * x^o)
 k is an integer > 0, m is an integer >= 0, e,o are binary values(booleans)
 */

//==================================== Config ======================================================================//

static const char * MultTableFile = "config/MultiplicationTable.txt";
static const char * SpecialMultTableFile = "config/SpecialCasesMultiplicationTable.txt";

static const char * IdentityFileNil2 = "config/nil2.txt";
static const char * IdentityFileNil3 = "config/nil3.txt";
static const char * IdentityFileNil4 = "config/nil4.txt";

static const char * KernelPath = "/Applications/Mathematica.app/Contents/MacOS/MathKernel";

//============================ Program control variables (init in MathDriver) ======================================//

extern bool verbose;
extern bool parserProgress;
extern bool latexForm;
extern std::ostream * output;

//======================================= Kernel connection paramameter ============================================//

static const int NumberOfKernelParams = 5;
static char * KernelParams[NumberOfKernelParams];

static const char * Param1 = "-linkmode";
static const char * Param2 = "launch";
static const char * Param3 = "-linkname";

//=================================================================================================================//

static const char * MathError = "$Failed";
static const char * MathTrue = "True";
static const char * ResetVariables = "m=.; n=.; i=.; j=.; k=.; eps=.; sigma=.;";
static const char * VariableAssumptions = "{Element[{m, n, i, j, k}, Integers], m >= 0, n >= 0, i >= 0, j >= 0, k >= 0, Element[{eps, sigma}, Booleans]}";

static const int MaxIdentityParams = 4;

enum ElementType { EL_ZERO, EL_FIRST, EL_SECOND, EL_THIRD, EL_FOURTH };
enum ParseStates { INITIAL, PARSE_T, PARSE_CORE, PARSE_CORE1, PARSE_CORE_POW, PARSE_END, PARSE_END1 };
enum RuleParserState { ST_LEFT, ST_RIGHT, ST_EQUAL_SIGN, ST_RES, ST_COMMA, ST_EXPR, ST_EOF };
enum StackType { STACK_LEFT_BR, STACK_RIGHT_BR, STACK_ELEMENT, STACK_MULT, STACK_ZERO };

//=================================================================================================================//


#endif /* constants_h */
