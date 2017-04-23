//
//  MathDriver.cpp
//  BP_multiplication
//
//  Created by Matyáš Hollmann on 03/04/2017.
//  Copyright © 2017 Matyáš Hollmann. All rights reserved.
//

#include "MathDriver.hpp"

//========================================== INIT CONTROL VARIABLES ===============================================//

bool verbose = false;
bool parserProgress = false;
bool latexForm = false;
std::ostream * output = &std::cout;

//=================================================================================================================//

MathDriver::MathDriver(const char * path) : mKernel(path), mParser(&mKernel)
{
   outStream = new std::ofstream();
   mIdentity = nullptr;
}

//=================================================================================================================//

MathDriver::~MathDriver()
{
   if ( outStream != nullptr && outStream->is_open() )
      outStream->close();
   
   delete mIdentity;
   mIdentity = nullptr;
   
   delete outStream;
   outStream = nullptr;
}

//=================================================================================================================//

bool MathDriver::parseMultTable(const char * file, const bool specialCase )
{
   std::filebuf fb;
   Element tmp;
   int counter = 0;
   if (fb.open (file, std::ios::in) )
   {
      if ( parserProgress == true )
      {
         if ( specialCase == true )
            *output << "Parsing of the special cases part of the multiplication table begins:" << std::endl;
         else
            *output << "Parsing of the multiplication table begins:" << std::endl;
      }
   
      std::istream is(&fb);
      mParser.setParserStream(&is);
      std::vector<Element> res;
      mParser.pState = ST_LEFT;
      
      try {
         while ( mParser.pState != ST_EOF )
         {
            counter++;
            res.clear();
         
            Element left = mParser.parseElement();
            left.replace();
         
            mParser.pState = ST_RIGHT;
            Element right = mParser.parseElement();
            right.replace();
         
            if ( mParser.pState != ST_EQUAL_SIGN )
            {
               mParser.parsingError("Multiplication table parser error at rule #" + std::to_string(counter) + ", expected symbol was =, received: ", true);
            }
         
            mParser.pState = ST_RES;
            while ( mParser.pState == ST_RES )
            {
               tmp = mParser.parseElement();
               tmp.replace();
            
               res.push_back(tmp);
            }
         
            mTable.addRule(left, right, res, specialCase);
         
            if ( parserProgress )
               *output << "Rule #" << counter << " was successfully parsed. Is a special case: " << specialCase << std::endl;
         }
      }
      catch (const std::runtime_error & e)
      {
         fb.close();
         throw std::runtime_error(e.what());
      }
      
      fb.close();
      mParser.setParserStream(&std::cin);
      
      if ( parserProgress == true )
      {
         if ( specialCase == true )
            *output << "\nSpecial cases part of the multiplication table was successfully parsed." << std::endl;
         else
            *output << "\nMultiplication table was successfully parsed." << std::endl;
      }
      
      return true;
   }
   
   std::cerr << "Parsing multiplication table failed. File: '" << file << "' couldn't be opened." << std::endl;
   return false;
}

//=================================================================================================================//

void MathDriver::printMultTable() const
{
   mTable.printTable();
}

//=================================================================================================================//

void MathDriver::inputExpr()
{
   mParser.setParserStream(&std::cin);
   std::vector<Element> res;

   std::cout << "Input an expression you wish to evaluate: " << std::endl;
   try {
      res = mParser.parseExpression(&mTable);
   }
   catch (const std::runtime_error & e) {
      std::cerr << "\nExpression couldn't be parsed or evaluated.\n" << e.what() << std::endl;
      std::cerr << "Press ENTER to return to the menu." << std::endl;
      clearInputBuffer();
      return;
   }
   
   std::cout << std::endl;
   if ( latexForm )
      print("\\begin{dmath}\n", true);
   
   print(mParser.gActualExpress + " = ", true);
   printResult(res, true, &mKernel);
   
   if ( latexForm )
      print("\n\\end{dmath}\n", true);
   
   std::cout << std::endl;
}

//=================================================================================================================//

void MathDriver::evaluateIdentity()
{
   while ( mParser.pState != ST_EOF )
   {
   mParser.setParserStream(&std::cin);
   mParser.pState = ST_COMMA;
   std::vector<Element> elems;
   std::string ident;
   
   //std::cout << "Enter " << mIdentity->getNumElems() << " elements. Each element must be followed with a comma." << std::endl;
   
   try {
      for ( int i = 1; i <= mIdentity->getNumElems(); i++)
      {
         elems.push_back(mParser.parseElement());
         if ( parserProgress == true )
            *output << "Element #" << i << " parsed: " << elems.back().toString() << std::endl;
      }
   }
   catch (const std::runtime_error & e) {
      std::cerr << "\nElement parsing error:\n" << e.what() << std::endl;
      std::cerr << "Press ENTER to return to the menu." << std::endl;
      clearInputBuffer();
      return;
   }
   
   ident = mIdentity->substituteIdentity(elems, &mKernel);
   
   if ( verbose == true )
   {
      *output << "\n\n\n\nSubstitued identity: " << ident << "\n\n\n\n";
   }
   
   //std::cout << "\nEvaluating the identity. Please be patient it may take a while..." << std::endl;
   std::istringstream inp(ident.c_str());
   mParser.setParserStream(&inp);
   
   std::vector<Element> res;
   
   try {
      res = mParser.parseExpression(&mTable);
   }
   catch (const std::runtime_error & e) {
      std::cerr << "Substitued identity parsing error, please make sure the identity is syntactically correct. Error from parser:\n" << e.what() << std::endl;
      std::cerr << "\nSubstitued identity: " << ident << std::endl << std::endl;
      return;
   }
   
   ident = "W(";
   for ( int i = 0; i < mIdentity->getNumElems(); i++ )
   {
      if ( latexForm )
         ident += elems[i].toLatex(&mKernel);
      else
         ident += elems[i].toString();
      
      if ( i != mIdentity->getNumElems() - 1 )
         ident += ", ";
      else
         ident += ") = ";
   }
   
   std::cout << std::endl;
   
   if ( latexForm )
      print("\\begin{dmath}\n", true);
   
   print(ident, true);
   printResult(res, true, &mKernel);
   
   if ( latexForm )
      print("\n\\end{dmath}\n", true);
   
   std::cout << std::endl;
   
   
   }
}

//=================================================================================================================//

void MathDriver::changeOutputSettings()
{
   int choice = 0;
   while ( choice != 5 )
   {
      std::cout << "\n-----------------------------------------------------------------------------------------------------------------" << std::endl;
      std::cout << "\nOutput settings menu:" << std::endl;
      if ( verbose == false )
         std::cout << "1 - Enable detailed evaluation output." << std::endl;
      else
         std::cout << "1 - Disable detailed evaluation output." << std::endl;
      
      if ( parserProgress == false )
         std::cout << "2 - Show parser progress." << std::endl;
      else
         std::cout << "2 - Hide parser progress." << std::endl;
      
      if ( outStream->is_open() == true )
         std::cout << "3 - Restore standard output to the terminal." << std::endl;
      else
         std::cout << "3 - Redirect standard output to a log file." << std::endl;
      
      if ( latexForm == true )
         std::cout << "4 - Show results in the Wolfram Mathematica form." << std::endl;
      else
         std::cout << "4 - Show results in the LaTeX form." << std::endl;
      
      std::cout << "5 - Back to the main menu.\n" << std::endl;
      std::cout << "-----------------------------------------------------------------------------------------------------------------" << std::endl;
      std::cout << "Choose your option and press ENTER." << std::endl;
      
      std::cin >> choice;
      if ( std::cin.fail() )
      {
         choice = 0;
         clearInputBuffer();
      }
      
      if ( std::cin.eof() || std::cin.bad() )
      {
         choice = 5;
      }
      
      switch(choice)
      {
         case 1:
            verbose = !verbose;
            break;
         case 2:
            parserProgress = !parserProgress;
            break;
         case 3:
            changeStandOutput();
            break;
         case 4:
            latexForm = !latexForm;
            break;
         case 5:
            break;
         default:
            std::cout << "Invalid input. Please try again." << std::endl;
            break;
      }
   }
}

//=================================================================================================================//

void MathDriver::changeStandOutput()
{
   if ( outStream->is_open() == true )
   {
      outStream->close();
      output = &std::cout;
   }
   else
   {
      std::cout << "Enter the name (path) of the log file: ";
      std::string a;
      std::cin >> a;
      
      outStream->open(a.c_str(), std::ofstream::out);
      if ( outStream->is_open() == false || outStream->fail() == true )
      {
         std::cerr << "Error: The file '" + a + "' couldn't be opened for write." << std::endl;
         outStream->close();
         output = &std::cout;
      }
      else
      {
         output = outStream;
      }
   }
}

//=================================================================================================================//
   
void MathDriver::showMenu()
{
   int choice = 0;
   while ( choice != 5 )
   {
      std::cout << "\n-----------------------------------------------------------------------------------------------------------------" << std::endl;
      std::cout << "\nMain menu for alt-nil superalgebra A:" << std::endl;
      std::cout << "1 - Evaluate any expression in A." << std::endl;
      std::cout << "2 - Evaluate an identity W of A." << std::endl;
      std::cout << "3 - Change output settings." << std::endl;
     
      if ( latexForm )
      std::cout << "4 - Print the multiplication table of A as a LaTeX document." << std::endl;
      else
         std::cout << "4 - Print the multiplication table of A in the Wolfram Mathematica form." << std::endl;
      
      std::cout << "5 - Exit program.\n" << std::endl;
      std::cout << "-----------------------------------------------------------------------------------------------------------------" << std::endl;
      std::cout << "Choose your option and press ENTER." << std::endl;
   
      std::cin >> choice;
      if ( std::cin.fail() )
      {
         choice = 0;
         clearInputBuffer();
      }
      
      if ( std::cin.eof() || std::cin.bad() )
      {
         choice = 5;
      }
   
      switch(choice)
      {
         case 1:
            inputExpr();
            break;
         case 2:
            identSubmenu();
            break;
         case 3:
            changeOutputSettings();
            break;
         case 4:
            if ( latexForm )
               mTable.printTableLatex(&mKernel);
            else
               mTable.printTable();
            break;
         case 5:
            break;
         default:
            std::cout << "Invalid input. Please try again." << std::endl;
            break;
      }
   }
}

//=================================================================================================================//

void MathDriver::clearInputBuffer()
{
   std::cin.clear();
   std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

//=================================================================================================================//

bool MathDriver::parseIdentity(const char * file)
{
   std::filebuf fb;
   std::vector<char> elems;
   std::string res;
   int len;
   if (fb.open (file, std::ios::in) )
   {
      std::istream is(&fb);
      mParser.setParserStream(&is);
      
      try {
         res = mParser.parseIdentity(&elems);
      }
      catch (const std::runtime_error & e)
      {
         fb.close();
         throw std::runtime_error(e.what());
      }
      
      len = (int)elems.size();
      fb.close();
      mParser.setParserStream(&std::cin);
      
      if ( len > MaxIdentityParams )
      {
         throw std::runtime_error("Error: parsed identity has more parameters than the limit (" + std::to_string(MaxIdentityParams) + ". This program can't work with such identity. Number of parameters: " + std::to_string(len));
      }
      
      delete mIdentity;
      mIdentity = nullptr;
      mIdentity = new Identity(len, elems, res);
      
      return true;
   }
   
   std::cerr << "\nError: Identity file: '" << file << "' couldn't be opened." << std::endl;
   return false;
}

//=================================================================================================================//

void MathDriver::identSubmenu()
{
   int choice = 0;
   std::string file;
   while ( choice != 4 )
   {
      std::cout << "\n-----------------------------------------------------------------------------------------------------------------" << std::endl;
      std::cout << "\nIdentity submenu for alt-nil superalgebra A:" << std::endl;
      std::cout << "1 - Evaluate the actual identity W of A." << std::endl;
      std::cout << "2 - Print the actual identity W of A." << std::endl;
      std::cout << "3 - Change the actual identity W of A." << std::endl;
      std::cout << "4 - Back to the main menu.\n" << std::endl;
      std::cout << "-----------------------------------------------------------------------------------------------------------------" << std::endl;
      std::cout << "Choose your option and press ENTER." << std::endl;
      
      std::cin >> choice;
      if ( std::cin.fail() )
      {
         choice = 0;
         clearInputBuffer();
      }
      
      if ( std::cin.eof() || std::cin.bad() )
      {
         choice = 4;
      }
      
      switch(choice)
      {
         case 1:
            if ( mIdentity != nullptr )
               evaluateIdentity();
            else
            {
               std::cerr << "\nIdentity W of A is not loaded at the moment. Please select option 3 to load an identity." << std::endl << std::endl;
            }
            break;
         case 2:
            if ( mIdentity != nullptr )
               mIdentity->printIdent(true);
            else
            {
               std::cerr << "\nIdentity W of A is not loaded at the moment. Please select option 3 to load an identity." << std::endl << std::endl;
            }
            break;
         case 3:
            std::cout << "Enter the name of the identity file you want to load: " << std::endl;
            std::cin >> file;
            try {
               if ( parseIdentity(file.c_str()) == true )
                  std::cout << "\n\nIdentity was successfully loaded." << std::endl;
            }
            catch (const std::runtime_error & e) {
               std::cerr << "\n\nIdentity couldn't be loaded.\nError from the parser: " << e.what() << std::endl << std::endl;
            }
            break;
         case 4:
            break;
         default:
            std::cout << "Invalid input. Please try again." << std::endl;
            break;
      }
   }
}

//=================================================================================================================//

/*
void MathDriver::nonInterractive(const char * input, const char * output, bool ident, const char * identFile)
{
   
   if ( ident && parseIdentity(identFile))
   {
      mParser.setParserStream(&std::cin);
      mParser.pState = ST_COMMA;
      std::vector<Element> elems;
      std::string ident;
      
      std::cout << "Enter " << mIdentity->getNumElems() << " elements. Each element must be followed with a comma." << std::endl;
      
      try {
         for ( int i = 1; i <= mIdentity->getNumElems(); i++)
         {
            elems.push_back(mParser.parseElement());
            if ( parserProgress == true )
               *output << "Element #" << i << " parsed: " << elems.back().toString() << std::endl;
         }
      }
      catch (const std::runtime_error & e) {
         std::cerr << "\nElement parsing error:\n" << e.what() << std::endl;
         std::cerr << "Press ENTER to return to the menu." << std::endl;
         clearInputBuffer();
         return;
      }
      
      ident = mIdentity->substituteIdentity(elems, &mKernel);
      
      if ( verbose == true )
      {
         *output << "\n\n\n\nSubstitued identity: " << ident << "\n\n\n\n";
      }
      
      std::cout << "\nEvaluating the identity. Please be patient it may take a while..." << std::endl;
      std::istringstream inp(ident.c_str());
      mParser.setParserStream(&inp);
      
      std::vector<Element> res;
      
      try {
         res = mParser.parseExpression(&mTable);
      }
      catch (const std::runtime_error & e) {
         std::cerr << "Substitued identity parsing error, please make sure the identity is syntactically correct. Error from parser:\n" << e.what() << std::endl;
         std::cerr << "\nSubstitued identity: " << ident << std::endl << std::endl;
         return;
      }
      
      ident = "W(";
      for ( int i = 0; i < mIdentity->getNumElems(); i++ )
      {
         if ( latexForm )
            ident += elems[i].toLatex(&mKernel);
         else
            ident += elems[i].toString();
         
         if ( i != mIdentity->getNumElems() - 1 )
            ident += ", ";
         else
            ident += ") = ";
      }
      
      std::cout << std::endl;
      
      if ( latexForm )
         print("\\begin{dmath}\n", true);
      
      print(ident, true);
      printResult(res, true, &mKernel);
      
      if ( latexForm )
         print("\n\\end{dmath}\n", true);
      
      std::cout << std::endl;
      
      
   }
 
   
}   */

//=================================================================================================================//



