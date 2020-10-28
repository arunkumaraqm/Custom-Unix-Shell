#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef MYUTILS_H
#define MYUTILS_H

using namespace std;

// Some functions and system calls only take char*, not string as params
char* string_to_charptr(const string& s)
{
	return const_cast<char*>(s.c_str());
}

template <typename T>
void print_container(T & v) {
	for (auto ele: v) cout << ele << endl;
}

// Returns list of files in folder that are not directories
vector<string> list_dir(const string& path) 
{
    struct dirent *entry;
    vector<string> filenames;

    DIR *dir = opendir(string_to_charptr(path));
    if (dir == NULL) {
        return filenames;
    }

    while ((entry = readdir(dir)) != NULL) {
    	// 0x04 is the d_type for directories
    	// 0x08 is the d_type for others (??)
        if(entry->d_type != 0x04 )
        	filenames.push_back(entry->d_name);
    }

    closedir(dir);
    return filenames;
}

string find_path_of_file(const string& filename, const string& dir_to_search_in, bool& found)
{
	auto all_files = list_dir(dir_to_search_in);
	auto result = find(all_files.begin(), all_files.end(), filename);
	if (result != all_files.end())
	{
		found = true;
		return dir_to_search_in + "/" + filename;
	}
	found = false;
	return "";
}

string find_path_of_file(const string& filename, const vector<string>& list_of_dirs_to_search_in, bool& found)
{
	for(const string& dir: list_of_dirs_to_search_in)
	{
		auto path = find_path_of_file(filename, dir, found);
		if (found) return path;

	}
	found = false;
	return "";
}


// custom execv to take in a vector of strings, not overloaded to avoid confusion
// as second param instead of char* const*
void myexecv(const string& path, const vector<string>& args)
{
	vector<char*> av;
	// first arg has to be the path of executable
	av.push_back(string_to_charptr(path));

    for (const string& a: args) 
        av.push_back(string_to_charptr(a));

    av.push_back(nullptr); // list of args must be null terminated
    auto status = execv(path.c_str(), &av[0]);

    if (status < 0)
    {
    	throw runtime_error("File found is not executable.");
    }
}

bool endswith(string const & value, string const & ending)
{
    if (ending.size() > value.size()) return false;
    return equal(ending.rbegin(), ending.rend(), value.rbegin());
}
#endif
