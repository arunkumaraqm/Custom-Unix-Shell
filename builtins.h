#include "myutils.h"

#include <iostream>
#include <functional>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <vector>


#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifndef BUILTINS_H
#define BUILTINS_H

using namespace std;
class Shell;

class Builtins
{
	Shell& shell;
public:
	Builtins(Shell& shell) : shell(shell) {
	}

	int cd(const vector<string>& args);

	int help(const vector<string>& args)
	{
		cout << "This is our custom shell. Have a nice day!\n";
	}

	int pwd(const vector<string>& args);

	int echo(const vector<string>& args)
	{
		for (auto arg: args)
		{
			cout << arg << ' ';
		}
		cout << endl;
	}
};

#endif