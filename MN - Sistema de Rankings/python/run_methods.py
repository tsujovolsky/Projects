import os
import subprocess
import pandas as pd

# variables -----------------------------------------
temp_path = 'tmp'
data_path = os.path.join('..','data')
scenarios_path = 'scenarios'
img_path = 'img'
csv_path = 'csv'
tp_exe = os.path.join('..','src','tp')

# run methods ---------------------------------------
def filename(input_file):
	return os.path.basename(input_file)

def output_from(input_file, method_name, inside_folder=temp_path, format='out'):
	return os.path.join(inside_folder, (((filename(input_file)).split('.'))[0])+'-'+method_name+'.'+format)

def run_with_method(input_file, method_id, method_name):
	output_file = output_from(input_file, method_name)
	subprocess.run([tp_exe, input_file, output_file, method_id])
	
def run_colley(input_file):
	run_with_method(input_file, '0', 'colley')
	
def run_wp(input_file):
	run_with_method(input_file, '1', 'wp')
	
def run_massey(input_file):
	run_with_method(input_file, '2', 'massey')
	
def run_all(input_file):
	run_colley(input_file)
	run_wp(input_file)
	run_massey(input_file)

# generate pandas df -------------------------------
def nba_teams():
	return pd.read_csv(os.path.join(data_path, "nba_2016_teams.csv"), names = ["id", "conference", "name"])

def build_nba_comparison_dataframe(input_file, method_name, df_to_compare):
	ranking_name = method_name+"_ranking"
	new_ranking_name = method_name+"_new_ranking"
	output_file = output_from(input_file, method_name)
	teams_df = nba_teams()

	new_ranking = pd.read_csv(output_file, names = [new_ranking_name])
	new_ranking['team'] = teams_df['name']
	new_ranking = new_ranking.merge(right=df_to_compare, on=['team'], left_index=True)
	new_ranking.reset_index(inplace=True, drop=True)
	new_ranking.set_index('team', inplace=True)
	new_ranking.sort_values(by=new_ranking_name, ascending=False, inplace=True)
	new_ranking['difference'] = round(new_ranking[new_ranking_name]-new_ranking[ranking_name], 5)
	new_ranking = new_ranking[[ranking_name, new_ranking_name, 'difference']]
	new_ranking.style.applymap(color_positive_green, subset=pd.IndexSlice[['difference']])

	save_dataframe_as_csv(new_ranking, output_from(input_file, method_name, inside_folder=csv_path, format='csv'))
	return new_ranking

def build_nba_dataframe(input_file, method_name):
	ranking_name = method_name+"_ranking"
	output_file = output_from(input_file, method_name)
	teams_df = nba_teams()

	ranking = pd.read_csv(output_file, names = [ranking_name])
	ranking['team'] = teams_df['name']
	ranking['conference'] = teams_df['conference']
	ranking.reset_index(inplace=True, drop=True)
	ranking.set_index('team', inplace=True)
	ranking.sort_values(by=ranking_name, ascending=False, inplace=True)

	save_dataframe_as_csv(ranking, output_from(input_file, method_name, inside_folder=csv_path, format='csv'))
	return ranking

def build_dataframe(input_file, method_name, teams):
	ranking_name = method_name+"_ranking"
	output_file = output_from(input_file, method_name)
	
	ranking = pd.read_csv(output_file, names = [ranking_name])
	ranking['team'] = teams
	ranking.reset_index(inplace=True, drop=True)
	ranking.set_index('team', inplace=True)
	ranking.sort_values(by=ranking_name, ascending=False, inplace=True)

	save_dataframe_as_csv(ranking, output_from(input_file, method_name, inside_folder=csv_path, format='csv'))
	return ranking

def save_dataframe_as_csv(dataframe, output_file):
	dataframe.to_csv(
		path_or_buf=output_file, 
		sep = " ", 
		index= True,
		header = True)

# function for set text color of positive 
# values in Dataframes 
def color_positive_green(val): 
	""" 
	Takes a scalar and returns a string with 
	the css property 'color: green' for positive 
	strings, black otherwise. 
	"""
	if val > 0: 
		color = 'green'
	else: 
		color = 'red'
	return 'background-color: %s' % color