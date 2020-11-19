#include <iostream>
#include <regex>
#include <cassert>

using namespace std;

int main(int argc, char const *argv[])
{
	string a = "arun kumar";
	regex b ("arun.*");
	assert(regex_match(a, b));
	return 0;
}