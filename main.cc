#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <cstdlib>


#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

template <typename T>
void print_container(T & v) {
	for (auto ele: v) cout << ele << endl;
}

char* string_to_charptr(string s)
{
	return const_cast<char*>(s.c_str());
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



class Shell
{
	string line;
	string command;
	vector<string> args;
	string current_working_dir;
	string hostname;
	//vector<pid_t> bg_jobs;
public:
	void main_loop();
	void show_prompt();
	void read_line();
	void parse_line();
	void execute_command();
	
};

void Shell:: show_prompt()
{
	cout << "\033[33m" << getenv("USERNAME") << "@" /*<< getenv("HOSTNAME") */<<"\033[00m" << " % ";
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
	else if (command == "cd") // shell builtin
	{
		int status = chdir(string_to_charptr(args[0]));
		if (status) cerr << "not found\n";
	}
}

void Shell:: main_loop()
{
	chdir(getenv("HOME"));
	do
	{
		show_prompt();
		read_line();
		parse_line();
		execute_command();
	} while(command != "exit" and command != "q");
}

int main()
{
	Shell myshell;
	myshell.main_loop();
	return 0;
}
