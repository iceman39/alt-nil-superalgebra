//
//  multiplication_table.cpp
//  BP_multiplication
//
//  Created by Matyáš Hollmann on 25/03/2017.
//  Copyright © 2017 Matyáš Hollmann. All rights reserved.
//

#include "Multiplication.hpp"

//=================================================================================================================//

MultiplicationTable::Rule::Rule(const Element & l, const Element & r, const std::vector<Element> & res, bool exact ) : mLeft(l), mRight(r), mResult(res), mExactMatch(exact)
{
   
}

//=================================================================================================================//

MultiplicationTable::Rule::~Rule()
{
   mResult.clear();
}

//=================================================================================================================//

MultiplicationTable::MultiplicationTable()
{

}

//=================================================================================================================//

MultiplicationTable::~MultiplicationTable()
{
   mTable.clear();
}

//=================================================================================================================//

void MultiplicationTable::addRule(const Element & l, const Element & r, const std::vector<Element> & res, bool exact)
{
   mTable.push_back(Rule(l, r, res, exact));
}

//=================================================================================================================//

std::vector<Element> MultiplicationTable::multiply(Element & l, Element & r, KernelConnection * kernel) const
{
   std::vector<Element> res;
   
   l.simplify(kernel);
   r.simplify(kernel);
   
   ElementType leftType = l.getType();
   ElementType rightType = r.getType();
   
   
   if ( ((leftType == EL_THIRD || leftType == EL_FOURTH) && rightType != EL_FIRST) || ((rightType == EL_THIRD || rightType == EL_FOURTH) && leftType != EL_FIRST) || leftType == EL_ZERO || rightType == EL_ZERO )
   {
      res.push_back(Element()); // result of the multiplication will be 0
   }
   else
   {
      std::string notLeftX, notRightX;
      if ( kernel->testCondition(l.getSigma(), " == 0"))
      {
         notLeftX = "1";
      }
      else if ( kernel->testCondition(l.getSigma(), " == 1"))
      {
         notLeftX = "0";
      }
      else
      {
         throw std::runtime_error("Multiplication error: Multiplied elements have to have fixed last x.\n");
      }
      
      if ( kernel->testCondition(r.getSigma(), " == 0"))
      {
         notRightX = "1";
      }
      else if ( kernel->testCondition(r.getSigma(), " == 1"))
      {
         notRightX = "0";
      }
      else
      {
         throw std::runtime_error("Multiplication error: Multiplied elements have to have fixed last x.\n");
      }
      
      int counter = 0;
      for ( auto & rule : mTable )
      {
         counter++;
         if ( rule.mLeft.getType() == leftType && rule.mRight.getType() == rightType )
         {
            if ( rule.mLeft.getSigma() != notLeftX && rule.mRight.getSigma() != notRightX )
            {
               if ( rule.mExactMatch == true )
               {
                  if ( (isFixed(rule.mLeft.getSquareBrExp()) == false || rule.mLeft.getSquareBrExp() == l.getSquareBrExp()) ) // nothing else is fixed
                  {
                     //  full condition
                     //  if ( (isFixed(rule.mLeft.getTExp()) == false || rule.mLeft.getTExp() == l.getTExp())
                     //  && (isFixed(rule.mRight.getTExp()) == false || rule.mRight.getTExp() == r.getTExp())
                     //  && (isFixed(rule.mLeft.getSquareBrExp()) == false || rule.mLeft.getSquareBrExp() == l.getSquareBrExp())
                     //  && (isFixed(rule.mRight.getSquareBrExp()) == false || rule.mRight.getSquareBrExp() == r.getSquareBrExp()) )
                     
                     if ( verbose == true )
                        *output << "\nApplying rule #" << counter << " : " << l.toString() << " ** " << r.toString() << std::endl;
                           
                        return rule.applyRule(l, r, kernel);
                  }
               }
               else
               {
                  if ( verbose == true )
                     *output << "\nApplying rule #" << counter << " : " << l.toString() << " ** " << r.toString() << std::endl;
               
                  return rule.applyRule(l, r, kernel);
               }
            }
         }
      }
      throw std::runtime_error("Multiplication error: suitable rule was not found. (" + l.toString() + ") ** (" + r.toString() + ").");
   }
   
   return res;
}

//=================================================================================================================//

void MultiplicationTable::printTable() const
{
   std::string tmp;
   int counter = 0;
   for ( auto & rule : mTable )
   {
      counter++;
      std::cout << "Rule #" << counter << ": ( exact match = " << rule.mExactMatch << " )" <<std::endl;
      
      tmp = rule.mLeft.toString();
      std::replace( tmp.begin(), tmp.end(), 'a', 'm');
      std::replace( tmp.begin(), tmp.end(), 'b', 'n');
      std::replace( tmp.begin(), tmp.end(), 'c', 'i');
      std::replace( tmp.begin(), tmp.end(), 'd', 'j');
      std::replace( tmp.begin(), tmp.end(), 'f', 'k');
      
      std::cout << tmp;
      
      tmp = rule.mRight.toString();
      std::replace( tmp.begin(), tmp.end(), 'a', 'm');
      std::replace( tmp.begin(), tmp.end(), 'b', 'n');
      std::replace( tmp.begin(), tmp.end(), 'c', 'i');
      std::replace( tmp.begin(), tmp.end(), 'd', 'j');
      std::replace( tmp.begin(), tmp.end(), 'f', 'k');
      
      std::cout << " ** " << tmp << " = ";
      bool f = true;
      for ( auto & el : rule.mResult )
      {
         tmp = el.toString();
         std::replace( tmp.begin(), tmp.end(), 'a', 'm');
         std::replace( tmp.begin(), tmp.end(), 'b', 'n');
         std::replace( tmp.begin(), tmp.end(), 'c', 'i');
         std::replace( tmp.begin(), tmp.end(), 'd', 'j');
         std::replace( tmp.begin(), tmp.end(), 'f', 'k');
         
         if ( f == false )
            std::cout << " + ";
         std::cout << tmp;
         f = false;
      }
   
      std::cout << std::endl << std::endl;
   }
   
}

//=================================================================================================================//

std::vector<Element> MultiplicationTable::Rule::applyRule(const Element & l, const Element & r, KernelConnection * kernel) const
{
   std::string substitution, coeff, tExp, sqr, sigma;
   std::vector<Element> res;
   Element tmp;
   
   substitution += " /. " + mLeft.getTExp() + " -> (" + l.getTExp() + ") /. " + mLeft.getSquareBrExp() + " -> (" +  l.getSquareBrExp() + ")";
   
   if ( kernel->testCondition(mLeft.getSigma(), " < 2") == false ) // last X is not fixed
   {
      substitution += " /. " + mLeft.getSigma() + " -> (" +  l.getSigma() + ")";
   }
   
   substitution += " /. " + mRight.getTExp() + " -> (" +  r.getTExp() + ") /. " + mRight.getSquareBrExp() + " -> (" +  r.getSquareBrExp() + ")";
   
   if ( kernel->testCondition(mRight.getSigma(), " < 2") == false ) // last X is not fixed
   {
      substitution += " /. " + mRight.getSigma() + " -> (" +  r.getSigma() + ")";
   }
   
   for ( auto & el : mResult )
   {
      coeff = "(" + l.getCoeff() + ")*(" + r.getCoeff() + ")*(" + el.getCoeff() + ")" + substitution;
      kernel->validateString(coeff);
      
      tExp = el.getTExp() + substitution;
      kernel->validateString(tExp);
      
      sqr = el.getSquareBrExp() + substitution;
      kernel->validateString(sqr);
      
      sigma = el.getSigma() + substitution;
      kernel->validateString(sigma);
      
      tmp = Element(el.getType(), coeff, tExp, sqr, sigma);
      
      
      // tmp.simplify(kernel);  //, I think this simplify call can be ommitted
      
      if ( tmp.getType() != EL_ZERO )
         res.push_back(tmp);
   }
   
   return res;
}

//=================================================================================================================//

void MultiplicationTable::simplifyExpression(std::vector<Element> & vect, KernelConnection * kernel) const
{
   // sort vector and join coeffs the same type
   
   Element a,b;
   std::string cond;
   
   sort(vect.begin(), vect.end());
   
   bool move = true;
   int len = (int)vect.size();
   for ( int i = 0; i < len; i++ )
   {
      move = true;
      vect[i].simplify(kernel);
      
      if ( vect[i].getType() == EL_ZERO )
      {
         vect.erase(vect.begin() + i);
         i--;
         len--;
         continue;
      }
      if ( a.getType() == EL_ZERO )
      {
         a = vect[i];
         continue;
      }
      else
      {
         b = vect[i];
      }
      if ( a.getType() == b.getType() && a.getType() != EL_ZERO )
      {
         cond = " == (";
         cond += b.getTExp();
         cond += ")";
         if ( kernel->testCondition(a.getTExp(), cond) )
         {
            cond = " == (";
            cond += b.getSquareBrExp();
            cond += ")";
            if ( kernel->testCondition(a.getSquareBrExp(), cond) )
            {
               cond = " == (";
               cond += b.getSigma();
               cond += ")";
               if ( kernel->testCondition(a.getSigma(), cond) )
               {
                  if ( verbose == true )
                  {
                     *output << "These two elements can be merged: " << a.toString() << " and " << b.toString() << std::endl;
                  }
                  
                  cond = "(";
                  cond += a.getCoeff();
                  cond += ") + (";
                  cond += b.getCoeff();
                  cond += ")";
                  if ( kernel->validateString(cond) )
                  {
                     vect.erase(vect.begin() + i);
                     move = false;
                     
                     if ( kernel->testCondition(cond, " == 0") )
                     {
                        vect.erase(vect.begin() + i - 1);
                        
                        i--;
                        len--;
                        a.setZero();
                     }
                     else
                     {
                        a.setCoeff(cond);
                        vect[i - 1].setCoeff(cond);
                        
                        if ( verbose == true )
                           *output << "Aggregated coefficient of these two elements is: " << cond << std::endl;
                     }

                     i--;
                     len--;
                  }
                  else
                  {
                     throw std::runtime_error("Error in simplify expression. Aggregated coefficient is not valid according to the Wolfram kernel. Coefficient: " + cond);
                  }
               }
            }
         }
         
      }
      
      if ( move == true )
      {
         a = b;
      }
   }
}

//=================================================================================================================//

std::vector<Element> MultiplicationTable::multiplyExpr(std::vector<Element> & l, std::vector<Element> & r, KernelConnection * kernel) const
{
   std::vector<Element> res, tmp;
   for ( auto & left : l )
   {
      for ( auto & right : r )
      {
         tmp = multiply(left, right, kernel);
         res.reserve(res.size() + tmp.size());
         res.insert(res.end(), std::make_move_iterator(tmp.begin()), std::make_move_iterator(tmp.end()));
         
         tmp.clear();
      }
   }
   
   simplifyExpression(res, kernel);
   
   if ( verbose == true )
   {
      *output << "\nMultiplying two expressions: (";
      
      printResult(l, false);
      *output << ") ** (";
      
      printResult(r, false);
      *output << ")\nResult of this multiplication: ";
      
      printResult(res, false);
      *output << std::endl << std::endl;
   }
   
   return res;
}

//=================================================================================================================//

bool MultiplicationTable::isFixed(const std::string & str) const
{
   return str.find_first_not_of( "0123456789" ) == std::string::npos;
}

//=================================================================================================================//

void MultiplicationTable::printTableLatex(KernelConnection * kernel) const
{
   *output << "\\documentclass[a4paper,12pt]{article}\n\\usepackage{latexsym}\n\\usepackage[utf8]{inputenc}\n\\usepackage{breqn}\n\\begin{document}\n\\section{Multiplication Table}\n";
   
   for ( auto & rule : mTable )
   {
      *output << "\\begin{dmath}" << std::endl;
      *output << "(" << rule.mLeft.toLatex(kernel) << ")*(" << rule.mRight.toLatex(kernel) << ") = ";
      bool f = true;
      for ( auto & el : rule.mResult )
      {
         if ( f == false )
            *output << " + ";
         *output << "(" << el.toLatex(kernel) << ")";
         f = false;
      }
      *output << ",\n\\end{dmath}" << std::endl;
   }
   *output << "\n\\end{document}" << std::endl;
}

//=================================================================================================================//
