#include "banner.h"
#include "builtins.h"
#include "constants.h"
#include "myutils.h"

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int loop_ctr = 0; //

class Shell
{
protected:
	string line;
	string command;
	vector<string> args;
	string current_working_dir;
	string hostname;
	Builtins builtins;
	//vector<pid_t> bg_jobs;

	const vector<string> path_variable = {
        "/usr/local/sbin",
        "/usr/local/bin",
        "/usr/sbin",
        "/usr/bin",
        "/sbin",
        "/bin"
    };
public:
	Shell() : builtins(*this) {}
	void main_loop();

protected:
	virtual void show_prompt();
	virtual void read_line();
	virtual void parse_line();
	virtual void find_and_execute_command();
	virtual void execute_external_command();
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
			// << loop_ctr ///
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

void Shell:: execute_external_command()
{
	// BUG
	// Ctrl+C not only stops command execution, it also kills the entire shell

	// let child process execute command
	if (fork() == 0)
	{
		myexecv(command, args);
	}
	else
	{
		int status;
		wait(&status);
	}

}

void Shell:: find_and_execute_command()
{
	bool found = true;

	// Checking if command matches builtins
	// Did not use hashmap due to some difficulty with function pointers
	if (command == "cd")
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
	else if (command == "exit" or command == "q")
	{}

	// Checking if command matches one of the external commands in bin, sbin, etc.
	else 
	{
		string modified_command;
		if (IS_CYGWIN and not endswith(command, ".exe")) // Doubt: Will this work for all commands?
			modified_command = command + ".exe";
		
		modified_command = find_path_of_file(modified_command, path_variable, found);
		if (found)
		{
			command = modified_command;
			try {execute_external_command();}
			catch (runtime_error rerr){cerr << rerr.what() << endl;} // should never happen if found is true; it's just there as a sanity check
		}
	}
	
	if (not found) // the command is called by the user by its absolute or relative path
	{
		try{
			ifstream fin(command);
			if (!fin) throw runtime_error("Relative path not found.");
			execute_external_command();
		}
		catch (runtime_error rerr) {cerr << rerr.what() << endl;} // may happen
	}

	// TODO
	// Check if commands match stuff in $PATH variable

}

void Shell:: main_loop()
{
	chdir(getenv("HOME"));
	current_working_dir = getenv("HOME");
	display_banner();

	do
	{
		show_prompt();
		read_line();
		parse_line();
		find_and_execute_command();
		loop_ctr++;//
	} while(command != "exit" and command != "q");
}



