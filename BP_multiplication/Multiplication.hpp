//
//  multiplication_table.hpp
//  BP_multiplication
//
//  Created by Matyáš Hollmann on 25/03/2017.
//  Copyright © 2017 Matyáš Hollmann. All rights reserved.
//

#ifndef MultiplicationTable_hpp
#define MultiplicationTable_hpp

#include "Element.hpp"
#include <vector>
#include <iostream>

//=================================================================================================================//

class MultiplicationTable {
public:
   MultiplicationTable();
   ~MultiplicationTable();
   void addRule( const Element & l, const Element & r, const std::vector<Element> & res, bool exact );
   std::vector<Element> multiplyExpr( std::vector<Element> & l, std::vector<Element> & r, KernelConnection * kernel ) const;
   void simplifyExpression( std::vector<Element> & vect, KernelConnection * kernel ) const;
   
   bool isFixed(const std::string & str) const;
   void printTable() const;
   void printTableLatex(KernelConnection * kernel) const;
private:
   std::vector<Element> multiply(Element & l, Element & r, KernelConnection * kernel) const;
   
   struct Rule{
      Rule( const Element & l, const Element & r, const std::vector<Element> & res, bool exact );
      ~Rule();
      
      std::vector<Element> applyRule(const Element & l, const Element & r, KernelConnection * kernel) const;
      
      Element mLeft;
      Element mRight;
      std::vector<Element> mResult;
      bool mExactMatch;
   };
   
   std::vector<Rule> mTable;
};

//=================================================================================================================//

#endif /* MultiplicationTable_hpp */
