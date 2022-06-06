#pragma once
#include<fstream>
#include<sstream>
#include<iostream>
#include<valarray>
using namespace std;

class StatisticFile {
public:
	//for each problem
	valarray<double> suc;
	valarray<double> sucR2;
	valarray<double> train_error;
	valarray<double> test_error;
	valarray<double> testR2;
	valarray<double> eva_times;
	valarray<double> prog_size;
	valarray<double> trainTime;

	fstream f;
	int conv_internal = 50000;

	//for each independent run
	int last_internal = 0;

	char overallAdd[100];
	char fitness_convAdd[100] ;
	char fitness_timesAdd[100];
	char train_errorAdd[100];
	char test_errorAdd[100];
	char test_R2Add[100];
	char prog_sizeAdd[100];
	char example_progAdd[100];
	char trainTimeAdd[100];

	void initFile(int run, int job, char*base);
	void problemInit(int iterative);
	void independentRunInit(int run, int job);
	void procedureRecord(double eval, double fitness);
	void independentRunRecord(int run, int job, int sucFlag, int sucR2, double f_times, double train_error, double test_error, double test_R2, double prog_size, string example_prog, double train_time);
	void problemOveralRecord(int run);
};

void StatisticFile::initFile(int run, int job, char*base) {

	sprintf(overallAdd, "%s/overall_%d_%d.txt", base, run, job);  //the summary of all independent runs. An interface for summary file, if you run all independent runs by a single thread and a for loop.
	sprintf(fitness_convAdd, "%s/fitness_conv_%d_%d.txt", base, run, job); //the fitness convergence for an independent run, slotted every 50000 NNE (see variable "conv_internal")
	sprintf(fitness_timesAdd, "%s/fitness_time_%d_%d.txt", base, run, job); //the actual number of NNE used by SLGP in a certain independent run
	sprintf(train_errorAdd, "%s/train_error_%d_%d.txt", base, run, job);  //the training error (RMSE) at the final generation
	sprintf(test_errorAdd, "%s/test_error_%d_%d.txt", base, run, job);  //the test error (RMSE) of the outputted individual
	sprintf(test_R2Add, "%s/test_R2_%d_%d.txt", base, run, job);  //the test R2 of the outputted individual
	sprintf(prog_sizeAdd, "%s/prog_size_%d_%d.txt", base, run, job);  //the program size of the outputted individual
	sprintf(example_progAdd, "%s/example_prog_%d_%d.txt", base, run, job); //the program of outputted individual
	sprintf(trainTimeAdd, "%s/trainTime_%d_%d.txt", base, run, job); //the total training time of SLGP in an independent run

	f.open(overallAdd, ios::out);
	f.close();
	f.open(fitness_convAdd, ios::out);
	f.close();
	f.open(fitness_timesAdd, ios::out);
	f.close();
	f.open(train_errorAdd, ios::out);
	f.close();
	f.open(test_errorAdd, ios::out);
	f.close();
	f.open(test_R2Add, ios::out);
	f.close();
	f.open(prog_sizeAdd, ios::out);
	f.close();
	f.open(example_progAdd, ios::out);
	f.close();
	f.open(trainTimeAdd, ios::out);
	f.close();
}

void StatisticFile::problemInit(int iterative) {
	suc.resize(iterative, 0);
	sucR2.resize(iterative, 0);
	train_error.resize(iterative, 0);
	test_error.resize(iterative, 0);
	testR2.resize(iterative, 0);
	eva_times.resize(iterative, 0);
	prog_size.resize(iterative, 0);
	trainTime.resize(iterative, 0);
}

void StatisticFile::independentRunInit(int run, int job) {
	last_internal = 0;

	f.open(fitness_convAdd, ios::out|ios::app);
	f << run << "\t" << job << endl;
	f.close();
	f.open(fitness_timesAdd, ios::out|ios::app);
	f << run << "\t" << job << "\t";
	f.close();
	f.open(train_errorAdd, ios::out|ios::app);
	f << run << "\t" << job << "\t";
	f.close();
	f.open(test_errorAdd, ios::out|ios::app);
	f << run << "\t" << job << "\t";
	f.close();
	f.open(test_R2Add, ios::out|ios::app);
	f << run << "\t" << job << "\t";
	f.close();
	f.open(prog_sizeAdd, ios::out|ios::app);
	f << run << "\t" << job << "\t";
	f.close();
	f.open(example_progAdd, ios::out|ios::app);
	f << run << "\t" << job << endl;
	f.close();
	f.open(trainTimeAdd, ios::out|ios::app);
	f << run << "\t" << job << "\t";
	f.close();
}

void StatisticFile::procedureRecord(double eval, double fitness) {
	f.open(fitness_convAdd, ios::out|ios::app);
	int recordeval = (int)eval / conv_internal;
	if(last_internal == 0) f << last_internal << "\t" << fitness << endl;

	for (int i = 1 + last_internal; i <= recordeval; i++) {
		f << i*conv_internal << "\t" << fitness << endl;
	}
	
	last_internal = recordeval;
	f.close();
}

void StatisticFile::independentRunRecord(int run, int job, int sucFlag, int suc_R2, double f_times, double train_err, double test_err, double test_R2, double prog_siz, string example_prog, double train_time) {
	suc[job] = sucFlag;
	sucR2[job] = suc_R2;
	eva_times[job] = f_times;
	train_error[job] = train_err;
	test_error[job] = test_err;
	testR2[job] = test_R2;
	prog_size[job] = prog_siz;
	trainTime[job] = train_time;

	cout << run << "\t" << job << "\tsuc:\t" << suc.sum() / (job + 1)
		<< "\tsucR2:\t" << sucR2.sum() / (job + 1)
		<< "\teva_times:\t" << eva_times.sum() / (job + 1)
		<< "\ttrain_error:\t" << train_error.sum() / (job + 1)
		<< "\ttest_error:\t" << test_error.sum() / (job + 1)
		<< "\ttest_R2:\t" << testR2.sum() / (job + 1)
		<< "\tprog_size:\t" << prog_size.sum() / (job + 1)
		<< "\ttrain_time:\t" << trainTime.sum() / (job + 1)
		<< endl;

	f.open(fitness_timesAdd, ios::out | ios::app);
	f << f_times << endl;
	f.close();
	procedureRecord(f_times, train_err);
	f.open(train_errorAdd, ios::out | ios::app);
	f << train_err << endl;
	f.close();
	f.open(test_errorAdd, ios::out | ios::app);
	f << test_err << endl;
	f.close();
	f.open(test_R2Add, ios::out | ios::app);
	f << test_R2 << endl;
	f.close();
	f.open(prog_sizeAdd, ios::out | ios::app);
	f << prog_siz << endl;
	f.close();
	f.open(example_progAdd, ios::out | ios::app);
	f << example_prog << endl <<endl;
	f.close();
	f.open(trainTimeAdd, ios::out | ios::app);
	f << train_time << endl;
	f.close();

}

void StatisticFile::problemOveralRecord(int run) {
	f.open(overallAdd, ios::out | ios::app);
	f << run << "\tsuc:\t" << suc.sum() / suc.size()
		<< "\tsuc_R2:\t" << sucR2.sum() / sucR2.size()
		<< "\teva_times:\t" << eva_times.sum() / eva_times.size()
		<< "\ttrain_error:\t" << train_error.sum() / train_error.size()
		<< "\ttest_error:\t" << test_error.sum() / test_error.size()
		<< "\ttest_R2:\t" << testR2.sum() / testR2.size()
		<< "\tprog_size:\t" << prog_size.sum() / prog_size.size()
		<< "\ttrain_time:\t" << trainTime.sum() / trainTime.size()
		<< endl;
	f.close();
}