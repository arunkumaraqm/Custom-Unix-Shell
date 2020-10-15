#include "builtins.h"
#include "myutils.h"

#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <cstdlib>
#include <climits>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;


class Shell
{
	string line;
	string command;
	vector<string> args;
	string current_working_dir;
	string hostname;
	Builtins builtins;
	//vector<pid_t> bg_jobs;
public:
	Shell() : builtins(*this) {}
	void main_loop();
	void show_prompt();
	void read_line();
	void parse_line();
	void execute_command();
	string get_current_working_dir() {return current_working_dir;}

	// (cd and pwd need to be able to modify/view shell's private vars)
	friend int Builtins:: pwd(const vector<string>&);
	friend int Builtins:: cd(const vector<string>&);
	
};


/*------------------------------------------------*/

// Defined here where both classes are not "improperly defined" anymore.
// (cd and pwd need to be able to modify/view shell's private vars)
// Can't be bothered to create a shell.h, shell.cc and builtins.cc right now.
int Builtins:: cd(const vector<string>& args)
{
	int status = chdir(string_to_charptr(args[0]));
	if (status) cerr << "Directory not found.\n";
	else
	{
		char cwd[PATH_MAX]; // constant defined in climits
		if (getcwd(cwd, sizeof(cwd)) != NULL) // this func is from unistd
		{
			shell.current_working_dir = cwd;
		}
		else
		{
			cerr << "getcwd() error.\n";
			status = 1;
		}
	}
	return status;
}

int Builtins:: pwd(const vector<string>& args)
{
	cout << shell.current_working_dir << '\n';
	return 0;
}

/*------------------------------------------------*/

void Shell:: show_prompt()
{
	cout 	<< "\033[33m" 
			<< getenv("USERNAME") 
			<< " "
			<< current_working_dir
			<<"\033[00m" 
			<< '\n'
			<< "% ";
}

void Shell:: read_line()
{
	getline(cin, line);	
}

void Shell:: parse_line()
{
	vector<string> tokens;
	string token;
	istringstream line_stream(line);
	while (line_stream >> token)
	{
		tokens.push_back(token);
	}
	
	// the command is the first token and the args are the rest
	// `at` used because line may be empty.
	command = tokens.at(0);
	args = tokens;
	args.erase(args.begin());
}


void Shell:: execute_command()
{
	if (command == "ls")
	{
		if (fork() == 0)
		{
			myexecv("/bin/ls", args);
		}
		else
		{
			int status;
			wait(&status);
		}
	}

	else if (command == "cd")
	{
		builtins.cd(args);
	}
	else if (command == "help")
	{
		builtins.help(args);
	}
	else if (command == "pwd")
	{
		builtins.pwd(args);
	}
	else if (command == "echo")
	{
		builtins.echo(args);
	}
}

void Shell:: main_loop()
{
	chdir(getenv("HOME"));
	current_working_dir = getenv("HOME");
	do
	{
		show_prompt();
		read_line();
		parse_line();
		execute_command();
	} while(command != "exit" and command != "q");
}

/*------------------------------------------------*/

int main()
{
	Shell myshell;
	myshell.main_loop();
	return 0;
}
