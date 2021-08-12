#ifndef COLLEY_H
#define COLLEY_H

#include <list>
#include <vector>

#include "match.h"
#include "schedule.h"

using namespace std;

void build_colley_matrix(Schedule schedule, vector<vector<double>>& C, vector<double>& b) {
    for (int team_id = 0; team_id < schedule.teams_amount; team_id++) {
        C[team_id][team_id] = 2;
    }

    for (list<Match>::iterator match = schedule.matches.begin(); match != schedule.matches.end(); ++match){    
        C[match->team_1_id-1][match->team_2_id-1] = C[match->team_1_id-1][match->team_2_id-1] - 1;
        C[match->team_2_id-1][match->team_1_id-1] = C[match->team_2_id-1][match->team_1_id-1] - 1;

        C[match->team_1_id-1][match->team_1_id-1] = C[match->team_1_id-1][match->team_1_id-1] + 1;
        C[match->team_2_id-1][match->team_2_id-1] = C[match->team_2_id-1][match->team_2_id-1] + 1;

        if (match->team_1_score > match->team_2_score) {
            b[match->team_1_id-1] = b[match->team_1_id-1] + 1;
            b[match->team_2_id-1] = b[match->team_2_id-1] - 1;
        } else {
            b[match->team_1_id-1] = b[match->team_1_id-1] - 1;
            b[match->team_2_id-1] = b[match->team_2_id-1] + 1;
        }
    }
    
    for (int team_id = 0; team_id < schedule.teams_amount; team_id++) {
        b[team_id] = 1 + (b[team_id] / 2);
    }
}

#endif
