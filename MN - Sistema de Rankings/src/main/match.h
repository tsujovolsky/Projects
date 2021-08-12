#ifndef MATCH_H
#define MATCH_H

#include <string>

using namespace std;

class Match {
	public:
		Match(string match_date, int first_team, int first_team_score, int second_team, int second_team_score) {
			date = match_date;
			team_1_id = first_team;
			team_1_score = first_team_score;
			team_2_id = second_team;
			team_2_score = second_team_score;
		}

		string date;
		int team_1_id;
		int team_1_score;
		int team_2_id;
		int team_2_score;
};

#endif