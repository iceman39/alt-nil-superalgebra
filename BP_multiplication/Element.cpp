//
//  element.cpp
//  BP_multiplication
//
//  Created by Matyáš Hollmann on 25/03/2017.
//  Copyright © 2017 Matyáš Hollmann. All rights reserved.
//

#include "Element.hpp"

//=================================================================================================================//

Element::Element(const ElementType & tp, const std::string & cf, const std::string & tExp, const std::string & sExp, const std::string & sgm) : mType(tp), mCoeff(cf), mTExp(tExp), mSquareBrackExp(sExp), mSigma(sgm)
{

}

//=================================================================================================================//

Element::~Element()
{
   
}

//=================================================================================================================//

ElementType Element::getType() const
{
   return mType;
}

//=================================================================================================================//

void Element::setZero()
{
   mType = EL_ZERO;
   mCoeff = mTExp = mSquareBrackExp = mSigma = "0";
}

//=================================================================================================================//

void Element::simplify(KernelConnection * kernel)
{
   if ( kernel->testCondition(mTExp, "<0") || kernel->testCondition(mCoeff, " == 0") || (mTExp == "0" && mSquareBrackExp == "0" && mSigma == "0"))
   {
      setZero();
      return;
   }
   if ( mType == EL_FOURTH && kernel->testCondition("Mod[" + mSquareBrackExp + ", 4]", "> 1") )
   {
      setZero();
      return;
   }
   else if ( mType == EL_THIRD )
   {
      if ( kernel->testCondition(mSquareBrackExp, " == 2") || kernel->testCondition("Mod[" + mSquareBrackExp + ", 4]", " == 3") )
      {
         setZero();
         return;
      }
      else if ( kernel->testCondition("Mod[" + mSquareBrackExp + ", 4]", " == 2") )
      {
         mType = EL_FOURTH;
         mCoeff = "(-1)*(" + mCoeff + ")";
         if ( kernel->validateString(mCoeff) == 0 )
            throw std::runtime_error("Validation error: string '" + mCoeff + "' is not valid according to the Wolfram kernel. In function Element::simplify()");
         
         mSquareBrackExp = mSquareBrackExp + "-1";
         if ( kernel->validateString(mSquareBrackExp) == 0 )
            throw std::runtime_error("Validation error: string '" + mSquareBrackExp + "' is not valid according to the Wolfram kernel. In function Element::simplify()");
         
         mTExp = mTExp + "+1";
         if ( kernel->validateString(mTExp) == 0 )
            throw std::runtime_error("Validation error: string '" + mTExp + "' is not valid according to the Wolfram kernel. In function Element::simplify().");
      }
   }
}

//=================================================================================================================//

const std::string & Element::getSigma() const
{
   return mSigma;
}

//=================================================================================================================//

const std::string & Element::getCoeff() const
{
   return mCoeff;
}

//=================================================================================================================//

const std::string & Element::getTExp() const
{
   return mTExp;
}

//=================================================================================================================//

const std::string & Element::getSquareBrExp() const
{
   return mSquareBrackExp;
}

//=================================================================================================================//

void Element::replace()
{
   std::replace( mCoeff.begin(), mCoeff.end(), 'm', 'a');
   std::replace( mCoeff.begin(), mCoeff.end(), 'n', 'b');
   std::replace( mCoeff.begin(), mCoeff.end(), 'i', 'c');
   std::replace( mCoeff.begin(), mCoeff.end(), 'j', 'd');
   std::replace( mCoeff.begin(), mCoeff.end(), 'k', 'f');
   
   std::replace( mTExp.begin(), mTExp.end(), 'm', 'a');
   std::replace( mTExp.begin(), mTExp.end(), 'n', 'b');
   std::replace( mTExp.begin(), mTExp.end(), 'i', 'c');
   std::replace( mTExp.begin(), mTExp.end(), 'j', 'd');
   std::replace( mTExp.begin(), mTExp.end(), 'k', 'f');
   
   std::replace( mSquareBrackExp.begin(), mSquareBrackExp.end(), 'm', 'a');
   std::replace( mSquareBrackExp.begin(), mSquareBrackExp.end(), 'n', 'b');
   std::replace( mSquareBrackExp.begin(), mSquareBrackExp.end(), 'i', 'c');
   std::replace( mSquareBrackExp.begin(), mSquareBrackExp.end(), 'j', 'd');
   std::replace( mSquareBrackExp.begin(), mSquareBrackExp.end(), 'k', 'f');
}

//=================================================================================================================//

const bool Element::operator <( const Element & a) const
{
   if ( mType == a.mType )
   {
      if ( mTExp.compare(a.mTExp) == 0 )
      {
         if ( mSquareBrackExp.compare(a.mSquareBrackExp) == 0 )
         {
            return mSigma.compare(a.mSigma) < 0;
         }
         else
         {
            return mSquareBrackExp.compare(a.mSquareBrackExp) < 0;
         }
      }
      else
      {
         return mTExp.compare(a.mTExp) < 0;
      }
   }
   else
   {
      return mType < a.mType;
   }  
}

//=================================================================================================================//

void Element::setCoeff( const std::string & in)
{
   mCoeff = in;
}

//=================================================================================================================//

const std::string Element::getElementParity(KernelConnection * kernel) const
{
   std::string res;
   if ( mType == EL_ZERO )
      res = "0";
   else if ( mType == EL_FIRST )
      return mSigma;
   else
   {
      res = mSquareBrackExp + "+" + mSigma;
      if ( mType == EL_THIRD )
         res += "+1";
   
      if ( kernel->validateString(res) == 0 )
      {
         throw std::runtime_error("Validation error: string '" + res + "' is not valid according to the Wolfram kernel. In function Element::getElementParity()");
      }
   
      if ( kernel->testCondition("Mod[" + res + ", 2]", " == 0") )
         res = "0";
      else if ( kernel->testCondition("Mod[" + res + ", 2]", " == 1") )
         res = "1";
   }
   
   if ( parserProgress == true )
      *output << "Parity of the element (" << toString() << ") is: '" << res << "'" << std::endl;
   
   return res;
}

//=================================================================================================================//

std::string Element::toString() const
{
   std::string res = "";
   
   if ( mType == EL_ZERO )
   {
      res = "0";
   }
   else
   {
      if ( mCoeff == "-1" )
         res += "-";
      else if ( mCoeff != "1" )
         res += "(" + mCoeff + ") * ";
      
      if ( mTExp == "1" )
         res += "t";
      else if ( mTExp != "0" )
         res += "t^(" + mTExp + ")";
      
      switch (mType) {
         case EL_SECOND:
            if ( mSigma != "0" )
               res += "(";
            res += "x^[" + mSquareBrackExp + "]";
            break;
         case EL_THIRD:
            res += "u^[" + mSquareBrackExp + "]";
            break;
         case EL_FOURTH:
            res += "z^[" + mSquareBrackExp + "]";
            break;
         default:
            break;
      }
      
      if ( mSigma == "1" )
         res += "x";
      else if ( mSigma != "0" )
         res += "x^(" + mSigma + ")";
      if ( mSigma != "0" && mType == EL_SECOND )
         res += ")";
   }
   
   return res;
}

//=================================================================================================================//

std::string Element::toLatex(KernelConnection * kernel) const
{
   std::string cf = mCoeff, t = mTExp, sq = mSquareBrackExp;
   
   std::replace( cf.begin(), cf.end(), 'a', 'm');
   std::replace( cf.begin(), cf.end(), 'b', 'n');
   std::replace( cf.begin(), cf.end(), 'c', 'i');
   std::replace( cf.begin(), cf.end(), 'd', 'j');
   std::replace( cf.begin(), cf.end(), 'f', 'k');
   
   std::replace( t.begin(), t.end(), 'a', 'm');
   std::replace( t.begin(), t.end(), 'b', 'n');
   std::replace( t.begin(), t.end(), 'c', 'i');
   std::replace( t.begin(), t.end(), 'd', 'j');
   std::replace( t.begin(), t.end(), 'f', 'k');
   
   std::replace( sq.begin(), sq.end(), 'a', 'm');
   std::replace( sq.begin(), sq.end(), 'b', 'n');
   std::replace( sq.begin(), sq.end(), 'c', 'i');
   std::replace( sq.begin(), sq.end(), 'd', 'j');
   std::replace( sq.begin(), sq.end(), 'f', 'k');
   
   std::string res = "";
   if ( mType == EL_ZERO )
   {
      res = "0";
   }
   else
   {
      if ( cf == "-1" )
         res += "-";
      else if ( cf != "1" )
         res += kernel->toLatex(cf);
      
      if ( t == "1" )
         res += "t";
      else if ( t != "0" )
         res += "t^{" + kernel->toLatex(t) + "}";
      
      switch (mType) {
         case EL_SECOND:
            if ( mSigma != "0" )
               res += "(";
            res += "x^{[" + kernel->toLatex(sq) + "]}";
            break;
         case EL_THIRD:
            res += "u^{[" + kernel->toLatex(sq) + "]}";
            break;
         case EL_FOURTH:
            res += "z^{[" + kernel->toLatex(sq) + "]}";
            break;
         default:
            break;
      }
      
      if ( mSigma == "1" )
         res += "x";
      else if ( mSigma == "eps" )
         res += "x^{\\epsilon}";
      else if ( mSigma == "sigma" )
         res += "x^{\\sigma}";
      
      if ( mSigma != "0" && mType == EL_SECOND )
         res += ")";
   }
   
   std::size_t found = 0;
   while ( (found = res.find("\\\\", found) ) != std::string::npos )
   {
      res.erase(found, 1);
   }
   return res;
}

//=================================================================================================================//

void printResult( const std::vector<Element> & vect, const bool dualStream, KernelConnection * kernel )
{
   if ( vect.empty() == true )
      print("0", dualStream);
   
   bool f = true;
   for ( auto & t : vect )
   {
      if ( f == false )
         print(" + ", dualStream);

      if ( latexForm && kernel != nullptr )
         print( "(" + t.toLatex(kernel) + ")", dualStream);
      else
         print(t.toString(), dualStream);
      f = false;
   }
}

//=================================================================================================================//

void print( const std::string & str, const bool dualStream )
{
   *output << str;
   if (output != &std::cout && dualStream == true )
   {
      std::cout << str;
   }
}

//=================================================================================================================//


