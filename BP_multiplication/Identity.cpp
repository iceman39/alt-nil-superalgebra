//
//  Identity.cpp
//  BP_multiplication
//
//  Created by Matyáš Hollmann on 22/04/2017.
//  Copyright © 2017 Matyáš Hollmann. All rights reserved.
//

#include "Identity.hpp"

//=================================================================================================================//

Identity::Identity(const int num, const std::vector<char> & order, const std::string & gen) : mNumberOfElements(num), mVariableOrder(order), mGeneralForm(gen)
{
   
}

//=================================================================================================================//

Identity::~Identity()
{
   
}

//=================================================================================================================//

std::string Identity::substituteIdentity(const std::vector<Element> & vars, KernelConnection * kernel) const
{
   if ( vars.size() != mNumberOfElements )
   {
      throw std::runtime_error("Identity substitution error - number of elements doesn't match.");
   }
   
   std::string res = mGeneralForm;
   
   std::string sgn, tmp;
   std::size_t found;
   
   for ( int i = 0; i < mNumberOfElements; i++ )
   {
      found = 0;
      sgn = "(";
      sgn += vars[i].getElementParity(kernel);
      sgn += ")";
      
      tmp = "sgn(";
      tmp += mVariableOrder[i];
      tmp += ")";
      while ( (found = res.find(tmp, found) ) != std::string::npos )
      {
         res = res.replace(found, 6, sgn);
      }
      
      found = 0;
      tmp = vars[i].toString();
      
      while ( (found = res.find(mVariableOrder[i], found) ) != std::string::npos )
      {
         res = res.replace(found, 1, tmp);
      }
   }
   
   return res;
}

//=================================================================================================================//

int Identity::getNumElems() const
{
   return mNumberOfElements;
}

//=================================================================================================================//

void Identity::printIdent(const bool dual) const
{
   print("\nW(", dual);
   for ( int i = 0; i < mNumberOfElements; i++ )
   {
      print(std::string(1, mVariableOrder[i]), dual);
      if ( i != mNumberOfElements - 1 )
         print(", ", dual);
      else
         print(") = ", dual);
   }
   print(mGeneralForm, dual);
   print("\n\n", dual);
}

//=================================================================================================================//



