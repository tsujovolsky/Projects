#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <iterator>

using namespace std;

vector<string> split_row(string line) {
	istringstream buffer(line);
	istream_iterator<string> begin(buffer), end;
	vector<string> tokens(begin, end);
	return tokens;
}

ostream& operator<<(ostream& os, const vector<double> &input) {
	os << "|";
	for (auto const& each: input) {
		os << setw(12) << each << " | ";
	}
	return os;
}

ostream& operator<<(ostream& os, const vector<vector<double>> &input) {
	for (auto const& each: input) {
		os << each << endl;
	}
	return os;
}

#endif