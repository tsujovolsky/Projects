#ifndef WIN_RATE_H
#define WIN_RATE_H

#include <vector>
#include <tuple>

#include "match.h"
#include "schedule.h"

void build_wins_and_played_vector(Schedule schedule, vector<tuple<int, int>>& wins_and_played){
    for (list<Match>::iterator match = schedule.matches.begin(); match != schedule.matches.end(); ++match) {
        get<1>(wins_and_played[match->team_1_id - 1])++;
        get<1>(wins_and_played[match->team_2_id - 1])++;
        if (match->team_1_score > match->team_2_score) {
            get<0>(wins_and_played[match->team_1_id - 1])++;
        } else {
            get<0>(wins_and_played[match->team_2_id - 1])++;
        }
    }
}

//wins_and_played.size() == rankings.size()
vector<double> calculate_win_rate_ranking (vector<tuple<int, int>>& wins_and_played){
    vector<double> rankings(wins_and_played.size());
    for (int i = 0; i < rankings.size(); ++i) {
        double i_team_wins = double(get<0>(wins_and_played[i]));
        double i_team_played = double(get<1>(wins_and_played[i]));
        rankings[i] = i_team_wins/i_team_played;
    }
    return rankings;
}

#endif
