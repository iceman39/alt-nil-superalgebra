//
//  Identity.hpp
//  BP_multiplication
//
//  Created by Matyáš Hollmann on 22/04/2017.
//  Copyright © 2017 Matyáš Hollmann. All rights reserved.
//

#ifndef Identity_hpp
#define Identity_hpp

#include <stdio.h>
#include "Element.hpp"
#include <vector>

//=================================================================================================================//

class Identity{
public:
   Identity(const int num, const std::vector<char> & order, const std::string & gen);
   ~Identity();
   std::string substituteIdentity(const std::vector<Element> & vars, KernelConnection * kernel) const;
   int getNumElems() const;
   void printIdent(const bool dual) const;
   
private:
   int mNumberOfElements;
   std::vector<char> mVariableOrder;
   std::string mGeneralForm;
};

//=================================================================================================================//

#endif /* Identity_hpp */
