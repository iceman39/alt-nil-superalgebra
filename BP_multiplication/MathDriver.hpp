//
//  MathDriver.hpp
//  BP_multiplication
//
//  Created by Matyáš Hollmann on 03/04/2017.
//  Copyright © 2017 Matyáš Hollmann. All rights reserved.
//

#ifndef MathDriver_hpp
#define MathDriver_hpp

#include "KernelConnection.hpp"
#include "Parser.hpp"
#include "Multiplication.hpp"
#include "Identity.hpp"
#include <fstream> 
#include <limits>
#include <iostream>

//=================================================================================================================//

class MathDriver
{
public:
   MathDriver(const char * path);
   ~MathDriver();
   bool parseMultTable(const char * file, const bool specialCase = false );
   void showMenu();
   void changeOutputSettings();
   bool parseIdentity(const char * file);
   
   // void nonInterractive(const char * input, const char * output, bool ident = false, const char * identFile = nullptr  );
   
   void changeStandOutput();
   void identSubmenu();
   void inputExpr();
   
   void evaluateIdentity();
   void substituteIdentity(const Element & u, const Element & v, const Element & w);
   
   void printMultTable() const;
   
private:
   void clearInputBuffer();
   std::ofstream * outStream;
   std::ofstream * outErrorStream;
   KernelConnection mKernel;
   Parser mParser;
   MultiplicationTable mTable;
   Identity * mIdentity;
};

//=================================================================================================================//

#endif /* MathDriver_hpp */
