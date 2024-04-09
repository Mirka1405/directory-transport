#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <filesystem>
#include <set>
using namespace std;
namespace tt = std::this_thread;
namespace chr = std::chrono;
namespace fs = std::filesystem;
string trimString(string str) {
	const string whiteSpaces = " \t\n\r\f\v";
	// Remove leading whitespace
	size_t first_non_space = str.find_first_not_of(whiteSpaces);
	str.erase(0, first_non_space);
	// Remove trailing whitespace
	size_t last_non_space = str.find_last_not_of(whiteSpaces);
	str.erase(last_non_space + 1);
	return str;
}
bool starts(string a, string b) {
	if (b.length() > a.length()) return false;
	if (b.length() == a.length()) return b == a;
	for (int i = 0;i < b.length();i++) if (a[i] != b[i]) return false;
	return true;
}
void error(int line) {
	cerr << "Error on line "<<line;
	cin.get(); //wait user input
}
int movefiles(vector<pair<string, string>> paths) {
	set<fs::path> to_delete;
	for (auto i : paths) {
		for (const auto& entry : fs::directory_iterator(i.first)) {
			if (fs::exists(fs::path(i.second + "/" + entry.path().filename().string()))) { fs::remove_all(i.second + "/" + entry.path().filename().string()); }
			fs::copy(entry.path(), fs::path(i.second+"/"+entry.path().filename().string()), fs::copy_options::recursive);
			to_delete.insert(entry.path());
		}
	}
	for(auto i: to_delete) fs::remove_all(i);
	return to_delete.size();
}
int main() {
	ifstream in("./config.txt");
	int delay = 10;
	vector<pair<string, string>> transit;
	string cmd;
	int line = 0;
	bool debug = false;
	while (getline(in,cmd)) {
		line++;
		if (starts(cmd, "//")) continue;
		if (starts(cmd, "DELAY")) { delay = stoi(cmd.substr(6));if (debug)cout << "Delay: set to " << delay << endl; continue; }
		if (cmd == "DEBUG") { debug = true;continue; }
		string from="", to="";
		bool first = true;
		for (char i : cmd) {
			if (i == '>') { if (first)first = false; else { error(line);return 0; } continue; }
			if (first)from += i;
			else to += i;
		}
		from = trimString(from);
		to = trimString(to);
		if (first) { cout << "Error: no destination" << endl;error(line); };
		if (!fs::is_directory(from)) { cout << "Error: " << from << " is not a directory" << endl; error(line); };
		if (debug) cout << "Link: " << from << " to " << to<<endl;
		transit.push_back(make_pair(from,to));
	};
	in.close();
	cout << "Created " << transit.size() << " links; Moving every " << delay << " seconds"<<endl;
	while (true) {
		if(debug)cout << "Completed moving; moved " << movefiles(transit) << " files"<<endl;
		tt::sleep_for(chr::seconds(delay));
	}
	return 0;
}