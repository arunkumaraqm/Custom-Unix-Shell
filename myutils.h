#include <iostream>

#ifndef MYUTILS_H
#define MYUTILS_H

using namespace std;

// Some functions and system calls only take char*, not string as params
char* string_to_charptr(string s)
{
	return const_cast<char*>(s.c_str());
}

template <typename T>
void print_container(T & v) {
	for (auto ele: v) cout << ele << endl;
}

// custom execv to take in a vector of strings, not overloaded to avoid confusion
// as second param instead of char* const*
int myexecv(const string& path, const vector<string>& args)
{
	vector<char*> av;
	// first arg has to be the path of executable
	av.push_back(string_to_charptr(path));

    for (const string& a: args) 
        av.push_back(string_to_charptr(a));

    av.push_back(nullptr); // list of args must be null terminated
    return execv(path.c_str(), &av[0]);
}
#endif
