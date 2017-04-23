//
//  code_storage.cpp
//  BP_multiplication
//
//  Created by Matyáš Hollmann on 21/04/2017.
//  Copyright © 2017 Matyáš Hollmann. All rights reserved.
//

//============================================ MATH DRIVER ===========================================//

 void MathDriver::inputMult()
 {
 mParser.setParserStream(&std::cin);
 mParser.pState = ST_LEFT;
 std::vector<Element> res;
 
 while ( mParser.pState != ST_EOF )
 {
 std::cout << "Input elements you want to multiply: " << std::endl;
 
 
 res.clear();
 mParser.pState = ST_LEFT;
 try {
 Element left = mParser.parseElement();
 
 mParser.pState = ST_COMMA;
 Element right = mParser.parseElement();
 
 res = mTable.multiply(left, right, &mKernel);
 
 std::cout << std::endl;
 left.printElement();
 std::cout << " * ";
 right.printElement();
 std::cout << " = \n\nresult: ";
 bool f = true;
 for ( auto & el : res )
 {
 if ( !f )
 std::cout << " + ";
 el.printElement();
 f = false;
 }
 std::cout << std::endl;
 }
 catch (std::runtime_error & e) {
 std::cout << "Exception catched: " << e.what() << std::endl;
 }
 }
 }
 
 //=================================================================================================================//
 
 void MathDriver::inputExpr()
 {
 mParser.setParserStream(&std::cin);
 std::vector<Element> res;
 
 while ( mParser.pState != ST_EOF )
 {
 std::cout << "Input expression to evaluate: " << std::endl;
 
 res.clear();
 try {
 res = mParser.parseExpression(&mTable);
 std::cout << "Result: ";
 bool f = true;
 for ( auto & t : res )
 {
 if ( f == false )
 std::cout << " + ";
 f = false;
 t.printElement();
 }
 
 std::cout << std::endl << std::endl;
 }
 catch (std::runtime_error e) {
 std::cout << "Exception catched: " << e.what() << std::endl;
 }
 }
 }

//===================================================== MAIN ========================================================//

driver.inputExpr();


driver.solveIdentity();
driver.inputMult();
driver.printMultTable();


 Element a = Element(SECOND, "1", "0", "3", "1");
 Element b = Element(SECOND, "1", "0", "3", "0");
 
 std::cout << "A: ";
 a.printElement();
 std::cout << "\nB: ";
 b.printElement();
 bool q = (a < b);
 bool qq = (b < a);
 std::cout << "\ncmp: " << q << " qq: " << qq << std::endl;
 std::string as = "0";
 std::cout << "\n" << as.compare("1") << endl;
 
 KernelConnection con(argv[0]);
 
 con.evalString("FactorInteger[19203]"); // /. k -> 2 /. n -> 8");
 con.evalString("k=2;"); // /. k -> 2 /. n -> 8");
 con.evalString("25 * k^2");
 con.clearKernel();
 con.evalString("25 * k^2");
 

//=================================================================================================================//

// eval mult from left to right
//=================================================================================================================//
 
 std::vector<Element> Parser::evalStackLeftToRight(std::stack<StackEl> & stackCopy)
 {
 std::vector<Element> res;
 std::vector<Element> left;
 std::vector<StackEl> tmpArray;
 
 int tmpSize = (int)stackCopy.size();
 std::cout << "stack size: " << tmpSize << std::endl << std::endl;
 
 tmpArray.reserve(tmpSize);
 for ( int i = tmpSize - 1; i >= 0; i-- )
 {
 tmpArray[i] = stackCopy.top();
 stackCopy.pop();
 }
 
 bool mult = false;
 for ( int i = 0; i < tmpSize; i++ )
 {
 
 switch (tmpArray[i].mStackType) {
 case STACK_ELEMENT:
 if ( mult == false )
 {
 if ( left.empty() == 0 )
 {
 res.insert(res.end(), std::make_move_iterator(left.begin()), std::make_move_iterator(left.end()));
 left.clear();
 }
 left = tmpArray[i].mData;
 }
 else if ( left.empty() == 0 )
 {
 left = mTable->multiplyExpr(left, tmpArray[i].mData, mKernel);
 mult = false;
 }
 else
 parsingError("Error - right operand for multiplication is missing.", false);
 break;
 case STACK_MULT:
 if ( mult == false && left.empty() == 0 )
 {
 mult = true;
 }
 else
 parsingError("Error in parse Expression - stack eval . mult", false);
 break;
 default:
 parsingError("Error in parse - unexpected thing on stack" + std::to_string(tmpArray[i].mStackType), false);
 break;
 }
 }
 
 if ( left.empty() == 0 )
 {
 res.insert(res.end(), std::make_move_iterator(left.begin()), std::make_move_iterator(left.end()));
 left.clear();
 }
 
 mTable->simplifyExpression(res, mKernel);
 return res;
 }
 
//=================================================================================================================//
// eval mult from left to right, end of the function


//=================================================================================================================//

 const char * KernelConnection::evalString(const char * toEval) const // , bool releaseInput = true
 {
 //WSPutFunction(link, "EvaluatePacket", 1);
 //WSPutFunction(link, "Plus", 3);
 // WSPutFunction(link, "ToExpression", 1);
 //WSPutString(link, "k=12;");
 // WSEndPacket(link);
 // WSFlush(link);
 
 int packet = 42;
 while ((packet = WSNextPacket(link)) && packet != RETURNPKT)
 {
 std::cout << "11rec packet: " << packet << std::endl;
 WSNewPacket(link);
 }
 
 const char * aaa;
 //WSGetSymbol(link, &aaa);
 
 //WSPutFunction(link, "ToExpression", 1);
 //WSPutString(link, "k=.;");
 //WSEndPacket(link);
 //WSFlush(link);
 
 
 while ((packet = WSNextPacket(link)) && packet != RETURNPKT)
 {
 std::cout << "11rec packet: " << packet << std::endl;
 WSNewPacket(link);
 }
 WSGetSymbol(link, &aaa);
 
 
 WSPutFunction(link, "ToString", 2);
 WSPutFunction(link, "FullSimplify", 1);
 WSPutFunction(link, "ToExpression", 1);
 WSPutString(link, toEval);
 WSPutSymbol(link, "InputForm");
 
 // WSPutFunction(link, "ToExpression", 1);
 // WSPutFunction(link, "Simplify", 1);
 // WSPutString(link, "27 - 9*9 + 3^2");
 // WSPutFunction(link, "ToExpression", 1);
 // WSPutString(link, "17^2");
 WSEndPacket(link);
 WSFlush(link);
 
 //int packet = 42;
 while ((packet = WSNextPacket(link)) && packet != RETURNPKT)
 {
 WSNewPacket(link);
 }
 if (WSError(link) != WSEOK)
 {
 std::cerr << "error occured during computation: " << WSError(link) << std::endl;
 //return "";
 }
 
 const char * tmp = nullptr;
 double c;
 wsint64 a;
 int b;
 switch(WSGetType(link))
 {
 case WSTKINT:
 WSGetInteger64(link, &a);
 std::cout << "integer ret: " << a << std::endl;
 break;
 case WSTKSTR:
 WSGetString(link, &tmp);
 std::cout << "string ret: " << tmp << std::endl;
 break;
 case WSTKSYM:
 WSGetString(link, &tmp);
 std::cout << "symbol ret: " << tmp << std::endl;
 break;
 case WSTKFUNC:
 WSGetFunction(link, &tmp, &b);
 std::cout << "function ret: name: " << tmp << "number of params: " << b << std::endl;
 break;
 case WSTKREAL:
 WSGetReal(link, &c);
 std::cout << "real ret: " << c << std::endl;
 break;
 default:
 std::cout << "unexpected return type: " << WSGetType(link) << std::endl;
 break;
 }
 return tmp;
 
 }
 
//=================================================================================================================//

 void Element::printElement() const
 {
 if ( mType == ZERO )
 {
 std::cout << "0";
 return;
 }
 
 if ( mCoeff != "1" )
 std::cout << "(" << mCoeff << ") * ";
 
 if ( mTExp == "1" )
 std::cout << "t";
 else if ( mTExp != "0" )
 std::cout << "t^(" << mTExp << ")";
 
 switch (mType) {
 case SECOND:
 std::cout << "(x^[" << mSquareBrackExp << "]";
 break;
 case THIRD:
 std::cout << "u^[" << mSquareBrackExp << "]";
 break;
 case FOURTH:
 std::cout << "z^[" << mSquareBrackExp << "]";
 break;
 default:
 break;
 }
 if ( mSigma == "1" )
 std::cout << "x";
 else if ( mSigma != "0" )
 std::cout << "x^(" << mSigma << ")";
 
 if ( mType == SECOND )
 std::cout << ")";
 }
 

//=================================================================================================================//


 void MathDriver::substituteIdentity(const Element & u, const Element & v, const Element & w)
 {
 std::string stringU = u.toString();
 std::string stringV = v.toString();
 std::string stringW = w.toString();
 
 std::string parU = u.getElementParity(&mKernel);
 std::string parV = v.getElementParity(&mKernel);
 std::string parW = w.getElementParity(&mKernel);
 
 std::string ident = "";
 
 ident += "(" + stringU + "**" + stringV + ")**" + stringW + " + (-1)^((" + parU + ")((" + parV + ")+(" + parW + ")))*(" + stringV + "**" + stringW + ")**" + stringU;
 ident += " + (-1)^((" + parW + ")((" + parU + ")+(" + parV + ")))*(" + stringW + "**" + stringU + ")**" + stringV + " + (-1)^((" + parV + ")(" + parW + "))*(" + stringU + "**" + stringW + ")**" + stringV;
 ident += " + (-1)^((" + parU + ")(" + parV + "))*(" + stringV + "**" + stringU + ")**" + stringW + " + (-1)^((" + parU + ")(" + parV + ")+(" + parU + ")(" + parW + ")+(" + parV + ")(" + parW + "))*(" + stringW + "**" + stringV + ")**" + stringU + ",";
 
 if ( verbose == true )
 *output << "\n\nSubstitued identity: " << ident << std::endl << std::endl;
 
 std::istringstream inp(ident.c_str());
 mParser.setParserStream(&inp);
 
 std::vector<Element> res;
 
 try {
 res = mParser.parseExpression(&mTable);
 }
 catch (std::runtime_error e) {
 std::cerr << "Substitued identity parsing error, please make sure the identity is syntactically correct. Error from parser:\n" << e.what() << std::endl;
 std::cerr << "\nSubstitued identity: " << ident << std::endl << std::endl;
 return;
 }
 
 print("\nResult: ");
 printResult(res, true, &mKernel);
 print("\n");
 }

Substitued identity: (t**t)**x^[k]+(-1)^((0)((0)+(k)))*(t**x^[k])**t+(-1)^((k)((0)+(0)))*(x^[k]**t)**t+(-1)^((0)(k))*(t**x^[k])**t+(-1)^((0)(0))*(t**t)**x^[k]+(-1)^((0)(0)+(0)(k)+(0)+(k))*(x^[k]**t)**t,



