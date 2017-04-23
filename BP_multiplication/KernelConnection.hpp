//
//  mathematica_connection.hpp
//  BP_multiplication
//
//  Created by Matyáš Hollmann on 26/03/2017.
//  Copyright © 2017 Matyáš Hollmann. All rights reserved.
//

#ifndef mathematica_connection_hpp
#define mathematica_connection_hpp

#include "wstp.h"
#include "Constants.h"
#include <iostream>
#include <string>

//=================================================================================================================//

class KernelConnection{
public:
   KernelConnection(const char * path);
   ~KernelConnection();
   bool validateString(std::string & str);
   bool testCondition(const std::string & leftSide, const std::string & cond);
   std::string toLatex(const std::string & input);
   void clearKernel();

private:
   WSENV env;
   WSLINK link;
   int errorNumber;
   
   int packet;
   const char * tmpResult;
};

//=================================================================================================================//

#endif /* mathematica_connection_hpp */
