#include "shell.h"

#include <iostream>
#include <regex>

#ifndef SHELL_WITH_WILDCARDS_H
#define SHELL_WITH_WILDCARDS_H

class ShellWithWildcards: public Shell
{
protected:
	virtual void parse_line(); // will be called from parent's mainloop
	virtual void substitute_wildcards();
};

void ShellWithWildcards:: parse_line()
{
	Shell:: parse_line();
	substitute_wildcards();
}

void ShellWithWildcards:: substitute_wildcards()
{
	// note that not all args are file names; options and option params are also args
	vector<string> new_args;
	for (auto iarg: args)
	{
		if (iarg.find("*") != string::npos or
			iarg.find("?") != string::npos)
		{
			// Converting wildcard pattern to regex pattern
			// 1. Escape any periods
			// 2. Change ? to .
			// 3. Change * to .*

			// POSSIBLE BUG: This might break if file name contains 
			// other characters that can be interpreted as regex operations.

			string new_iarg;
			for(auto ch: iarg)
			{
				if (ch == '.')
				{
					new_iarg += "\\."; // escaping the period
				}
				else if (ch == '?')
				{
					new_iarg += ".";
				}
				else if (ch == '*')
				{
					new_iarg += ".*";
				}
				else
				{
					new_iarg += ch;
				}
			}
			// cout << new_iarg << endl;//
			regex pattern(new_iarg);

			// Adding to new_args every file name that matches the regular expression
			auto search_list = list_files_and_folders(current_working_dir);
			for (auto fname: search_list)
			{
				if (fname != "." and fname != "..") // . and .. should be ignored for wildcard matching
				if (regex_match(fname, pattern))
				{
					new_args.push_back(fname);
				}
			}
		}
		else
		{
			new_args.push_back(iarg);
		}
	}

	// If no matches were found, retain original args
	if (new_args.size()) args = new_args;
}
#endif