#ifndef MASSEY_H
#define MASSEY_H

#include <vector>

#include "schedule.h"

using namespace std;

void build_massey_matrix(Schedule schedule, vector<vector<double>>& M, vector<double>& p){
    for (list<Match>::iterator match = schedule.matches.begin(); match != schedule.matches.end(); ++match) {
        M[match->team_1_id - 1][match->team_2_id - 1] = M[match->team_1_id - 1][match->team_2_id - 1] - 1;
        M[match->team_2_id - 1][match->team_1_id - 1] = M[match->team_2_id - 1][match->team_1_id - 1] - 1;

        M[match->team_1_id - 1][match->team_1_id - 1] = M[match->team_1_id - 1][match->team_1_id - 1] + 1;
        M[match->team_2_id - 1][match->team_2_id - 1] = M[match->team_2_id - 1][match->team_2_id - 1] + 1;

        p[match->team_1_id - 1] = p[match->team_1_id - 1] + (match->team_1_score - match->team_2_score);
        p[match->team_2_id - 1] = p[match->team_2_id - 1] + (match->team_2_score - match->team_1_score);
    }
    for (int i = 0; i < M.size(); ++i) {
        M[M.size() - 1][i] = 1;
    }
    p[p.size()-1] = 0;
}

#endif //MASSEY_H
