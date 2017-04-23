//
//  element.hpp
//  BP_multiplication
//
//  Created by Matyáš Hollmann on 25/03/2017.
//  Copyright © 2017 Matyáš Hollmann. All rights reserved.
//

#ifndef element_hpp
#define element_hpp

#include "constants.h"
#include <regex>
#include "KernelConnection.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

//=================================================================================================================//

class Element{
public:
   Element(const ElementType & tp = EL_ZERO, const std::string & cf = "0", const std::string & tExp = "0", const std::string & sExp = "0", const std::string & sgm = "0");
   ~Element();
   void simplify(KernelConnection * kernel);
   void setZero();
   void replace();
   const bool operator <( const Element & a) const;
   
   void setCoeff( const std::string & in);
   
   const std::string & getCoeff() const;
   ElementType getType() const;
   const std::string & getTExp() const;
   const std::string & getSquareBrExp() const;
   const std::string & getSigma() const;
   
   const std::string getElementParity(KernelConnection * kernel) const;
   std::string toString() const;
   std::string toLatex(KernelConnection * kernel) const;
   
private:
   ElementType mType;      // type of the element, more info in the constants.h header file
   std::string mCoeff; // symbolic coefficient of the element as a string
   std::string mTExp; // exponent of t ('m' in the type definition)
   std::string mSquareBrackExp;  // square brackets exponent
   std::string mSigma; // whether there is the last 'x'
};

//=================================================================================================================//

void printResult( const std::vector<Element> & vect, const bool dualStream = true, KernelConnection * kernel = nullptr);
void print( const std::string & str, const bool dualStream = true );

//=================================================================================================================//

#endif /* element_hpp */
