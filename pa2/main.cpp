#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cassert>
#include <fstream>
#include "Router.h"

using namespace std;

Router* router = new Router();

static void
usage()
{
   cout << "Usage: constraint_LEA [ -File < doFile > ]" << endl;
}

static void
myexit()
{
   usage();
   exit(-1);
}

static int
myStrNCmp(const string& s1, const string& s2, unsigned n)
{
   assert(n > 0);
   unsigned n2 = s2.size();
   if (n2 == 0) return -1;
   unsigned n1 = s1.size();
   assert(n1 >= n);
   for (unsigned i = 0; i < n1; ++i) {
      if (i == n2)
         return (i < n)? 1 : 0;
      char ch1 = (isupper(s1[i]))? tolower(s1[i]) : s1[i];
      char ch2 = (isupper(s2[i]))? tolower(s2[i]) : s2[i];
      if (ch1 != ch2)
         return (ch1 - ch2);
   }
   return (n1 - n2);
}

int main(int argc, char** argv)
{
    // -File <doFile>
    if (argc == 3) {
       if (!router->readNet(argv[1])) {
          cerr << "Error: cannot open file \"" << argv[1] << "\"!!\n";
          myexit();
       }
       /*router->printTrack();
       ofstream outfile(argv[2], ios::out);
       router->writeTrack(outfile);
       router->printChannelRouting();*/
       router->printNet();
       exit(EXIT_SUCCESS);
    }
    else myexit();
}
