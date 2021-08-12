#include <fstream>
#include <string>
#include <vector>

#include "utils.h"
#include "schedule.h"
#include "colley.h"
#include "gaussian_elimination.h"
#include "win_rate.h"
#include "massey.h"

using namespace std;

int main(int argc, char* argv[]) {
	/* Help message */
	if (argc < 4) { 
		cout << "Wrong number of arguments! Usage: " << endl;
		cout << std::endl;
		cerr << argv[0] << " INPUT_PATH RANKING_PATH METHOD_ID" << endl;
		cout << std::endl;
		cout << "Possible methods: {0 : CMM; 1 : WP; 2 : MMM}" << endl;
		return 1;
	}

	/* Parsing arguments */
	string input_path = argv[1];
	string ranking_path = argv[2];
	int method_id = atoi(argv[3]);

	/* Read input file */
	Schedule schedule = generate_from(input_path);

	/* Handling methods */
	vector<double> b(schedule.teams_amount, 0);
	vector<vector<double>> A(schedule.teams_amount, b);
	vector<tuple<int, int>> wins_and_played(schedule.teams_amount);
	vector<double> rankings(schedule.teams_amount);


	switch (method_id) {
		case 0:
			build_colley_matrix(schedule, A, b);
			gaussian_elimination(A, b);
			rankings = linear_equations_system_solver(A, b);
			break;
		case 1:
			build_wins_and_played_vector(schedule, wins_and_played);
			rankings = calculate_win_rate_ranking(wins_and_played);
			break;
		case 2:
			build_massey_matrix(schedule, A, b);
			gaussian_elimination(A, b);
			rankings = linear_equations_system_solver(A, b);
			break;
		default:
			cout << "Error! Invalid METHOD_ID. Possible values: {0 : CMM; 1 : WP; 2 : MMM}" << endl;
			break;
	}

	/* Write ranking */
	ofstream output_file (ranking_path);

	for (int i = 0; i < rankings.size(); ++i) {
		output_file << fixed << setprecision(14) << rankings[i] << endl;
	}
	output_file.close();

	return 0;
}