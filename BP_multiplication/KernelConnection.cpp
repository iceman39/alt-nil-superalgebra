//
//  mathematica_KernelConnection.cpp
//  BP_multiplication
//
//  Created by Matyáš Hollmann on 26/03/2017.
//  Copyright © 2017 Matyáš Hollmann. All rights reserved.
//

#include "KernelConnection.hpp"

//=================================================================================================================//

KernelConnection::KernelConnection(const char * path)
{
   env = WSInitialize(0);
   KernelParams[0] = (char *)path;
   KernelParams[1] = (char *)Param1;
   KernelParams[2] = (char *)Param2;
   KernelParams[3] = (char *)Param3;
   KernelParams[4] = (char *)KernelPath;
   
   link = WSOpenArgcArgv(env, NumberOfKernelParams, KernelParams, &errorNumber);
   if ( link == (WSLINK)0 || errorNumber != WSEOK || WSActivate(link) == 0)
   {
      throw std::runtime_error("Error: link to Mathematica kernel could not be established: Error " + std::to_string(errorNumber));
   }
}

//=================================================================================================================//

KernelConnection::~KernelConnection()
{
   WSClose(link);
   WSDeinitialize(env);
}

//=================================================================================================================//

bool KernelConnection::validateString(std::string & str)
{
   //clearKernel();
   
   WSPutFunction(link, "ToString", 2);
   WSPutFunction(link, "FullSimplify", 2);
   WSPutFunction(link, "ToExpression", 1);
   WSPutString(link, str.c_str());
   WSPutFunction(link, "ToExpression", 1);
   WSPutString(link, VariableAssumptions);
   WSPutSymbol(link, "InputForm");
   
   while ((packet = WSNextPacket(link)) && packet != RETURNPKT)
   {
      WSNewPacket(link);
   }
   if (WSError(link) != WSEOK)
   {
      errorNumber = WSError(link);
      throw std::runtime_error("Error occured during validation of: '" + str + "' by Wolfram kernel. Error number: " + std::to_string(errorNumber));
   }
   
   WSGetString(link, &tmpResult);
   
   if ( strcmp(tmpResult, MathError) == 0 )
   {
      if ( verbose == true )
         *output << "String '" + str + "' is not valid according to the Wolfram kernel. Returning false from function KernelConnection::validateString()." << std::endl;
      
      return false;
   }
   // update result string
   str.assign(tmpResult);
   WSReleaseString(link, tmpResult);
   
   return true;
}

//=================================================================================================================//

void KernelConnection::clearKernel()
{
   WSPutFunction(link, "EvaluatePacket", 1);
   WSPutFunction(link, "ToExpression", 1);
   WSPutString(link, ResetVariables);
   WSEndPacket(link);
   WSFlush(link);
   while ((packet = WSNextPacket(link)) && packet != RETURNPKT)
   {
      WSNewPacket(link);
   }
   
   WSGetSymbol(link, &tmpResult);
   WSReleaseString(link, tmpResult);
}

//=================================================================================================================//

bool KernelConnection::testCondition(const std::string & leftSide, const std::string & cond)
{
 //  clearKernel();
   
   std::string input = "(" + leftSide + ")" + cond;
   bool res = false;
   
   WSPutFunction(link, "ToString", 2);
   WSPutFunction(link, "FullSimplify", 2);
   WSPutFunction(link, "ToExpression", 1);
   WSPutString(link, input.c_str());
   WSPutFunction(link, "ToExpression", 1);
   WSPutString(link, VariableAssumptions);
   WSPutSymbol(link, "InputForm");

   while ((packet = WSNextPacket(link)) && packet != RETURNPKT)
   {
      WSNewPacket(link);
   }
   if (WSError(link) != WSEOK)
   {
      errorNumber = WSError(link);
      throw std::runtime_error("Error occured during testing condition: '" + leftSide + cond + "' by Wolfram kernel. Error number: " + std::to_string(errorNumber));
   }

   WSGetString(link, &tmpResult);
   if ( strcmp(tmpResult, MathTrue) == 0 )
      res = true;
   
   WSReleaseString(link, tmpResult);
   return res;
}

//=================================================================================================================//

std::string KernelConnection::toLatex(const std::string & input)
{
   WSPutFunction(link, "ToString", 2);
   WSPutFunction(link, "TeXForm", 1);
   WSPutFunction(link, "ToExpression", 1);
   WSPutString(link, input.c_str());
   WSPutSymbol(link, "InputForm");
   
   while ((packet = WSNextPacket(link)) && packet != RETURNPKT)
   {
      WSNewPacket(link);
   }
   if (WSError(link) != WSEOK)
   {
      errorNumber = WSError(link);
      throw std::runtime_error("Error occured during converting string: '" + input + "' to its LaTeX form. Error number: " + std::to_string(errorNumber));
   }
   
   WSGetString(link, &tmpResult);
   
   std::string res(tmpResult);
   WSReleaseString(link, tmpResult);
   
   return res;
}

//=================================================================================================================//



