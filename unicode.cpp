#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <set>
#include <filesystem>
using namespace std;
namespace tt = std::this_thread;
namespace chr = std::chrono;
namespace fs = std::filesystem;
wstring to_wide(string s) {
	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	wcout<<"Conv: "<<converter.from_bytes(s) << endl;
	return converter.from_bytes(s);
}
wstring trimString(wstring str) {
	const wstring whiteSpaces = to_wide(" \t\n\r\f\v");
	// Remove leading whitespace
	size_t first_non_space = str.find_first_not_of(whiteSpaces);
	str.erase(0, first_non_space);
	// Remove trailing whitespace
	size_t last_non_space = str.find_last_not_of(whiteSpaces);
	str.erase(last_non_space + 1);
	return str;
}
bool starts(wstring a, wstring b) {
	if (b.length() > a.length()) return false;
	if (b.length() == a.length()) return b == a;
	for (int i = 0;i < b.length();i++) if (a[i] != b[i]) return false;
	return true;
}
void error(int line) {
	cerr << "Error on line "<<line;
	cin.get(); //wait user input
}
int movefiles(vector<pair<wstring, wstring>> paths) {
	set<fs::path> to_delete;
	for (auto i : paths) {
		for (const auto& entry : fs::directory_iterator(i.first)) {
			wstring p = i.second + to_wide("/") + to_wide(entry.path().filename().string());
			if (fs::exists(fs::path(p))) { fs::remove_all(p); }
			fs::copy(entry.path(), fs::path(p), fs::copy_options::recursive);
			to_delete.insert(entry.path());
		}
	}
	for(auto i: to_delete) fs::remove_all(i);
	return to_delete.size();
}
int main() {
	wstring cmt = L"//";
	wstring dly = L"DELAY";
	wstring dbg = L"DEBUG";
	wifstream in("./config.txt");
	int delay = 10;
	vector<pair<wstring, wstring>> transit;
	wstring cmd;
	int line = 0;
	bool debug = false;
	while (getline(in,cmd)) {
		line++;
		if (starts(cmd, cmt)) continue;
		if (starts(cmd, dly)) { delay = stoi(cmd.substr(6));if (debug)cout << "Delay: set to " << delay << endl; continue; }
		if (cmd == dbg) { debug = true;continue; }
		wstring from, to;
		bool first = true;
		for (char i : cmd) {
			if (i == '>') { if (first)first = false; else { error(line);return 0; } continue; }
			if (first)from += i;
			else to += i;
		}
		from = trimString(from);
		to = trimString(to);
		if (first) { cout << "Error: no destination" << endl;error(line); };
		if (!fs::is_directory(from)) { wcout << "Error: " << from << " is not a directory" << endl; error(line); };
		if (debug) wcout << "Link: " << from << " to " << to<<endl;
		transit.push_back(make_pair(from,to));
	};
	in.close();
	cout << "Created " << transit.size() << " links; Moving every " << delay << " seconds"<<endl;
	while (true) {
		if (debug)wcout << "Completed moving; moved " << movefiles(transit) << " files" << endl;
		else movefiles(transit);
		tt::sleep_for(chr::seconds(delay));
	}
	return 0;
}