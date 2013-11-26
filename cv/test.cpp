#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <curl/curl.h>

using namespace std;
int main()

{
    stringstream ss;
    ss << "coucou" << 5 ;
    cout << ss.str() <<endl;
}
