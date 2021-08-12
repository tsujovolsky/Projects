#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <fstream>
#include <list>
#include <vector>
#include <string>

#include "utils.h"
#include "match.h"

using namespace std;

class Schedule {
	public:
		Schedule(int amount_of_teams, int amount_of_matches, list<string> all_matches) {
			teams_amount = amount_of_teams;
			matches_amount = amount_of_matches;
			matches = parse_matches(all_matches);
		}

		int teams_amount;
		int matches_amount;
		list<Match> matches;
	
	private:
		list<Match> parse_matches(list<string> all_matches) {
			list<Match> parsed_matches;

			for (list<string>::iterator match = all_matches.begin(); match != all_matches.end(); ++match){
				vector<string> row = split_row(*match);
				Match current_match = Match(row[0], stoi(row[1]), stoi(row[2]), stoi(row[3]), stoi(row[4]));
				parsed_matches.push_back(current_match);
			}
			
			return parsed_matches;
		}
};

Schedule generate_from(string file_path) {
	fstream input_file;
	input_file.open(file_path, ios::in);
	
	int teams_amount;
	int matches_amount;
	list<string> all_matches;
	vector<string> tokens;

	string line;
	getline(input_file, line);
	tokens = split_row(line);
	teams_amount = stoi(tokens[0]);
	matches_amount = stoi(tokens[1]);

	while(getline(input_file, line)) {
		all_matches.push_back(line);
	}

	input_file.close();
	return Schedule(teams_amount, matches_amount, all_matches);
}

#endif