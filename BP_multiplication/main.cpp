//
//  main.cpp
//  BP_multiplication
//
//  Created by Matyáš Hollmann on 20/03/2017.
//  Copyright © 2017 Matyáš Hollmann. All rights reserved.
//

#include <iostream>
#include "MathDriver.hpp"
//#include "element.hpp"
//#include "KernelConnection.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
   
   // to debug multiplication table parsing
   if ( argc > 1 && strcmp(argv[1], "-v") == 0 )
      verbose = true;
   
   MathDriver driver(argv[0]);
   
   driver.parseMultTable(SpecialMultTableFile, true); // it can fail
   
   if ( driver.parseMultTable(MultTableFile) == true )
   {
      driver.parseIdentity(IdentityFileNil3); // load nil3 ident file
      
      
//      std::ifstream ifs ("input/aa.txt", std::ifstream::in);
      
  //    cin.rdbuf( ifs.rdbuf() ); // redirect input
      
      driver.showMenu();
   }

   return 0;
}

