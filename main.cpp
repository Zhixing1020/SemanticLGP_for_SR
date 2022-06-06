#include<iostream>
#include<time.h>
#include"Data.h"
#include"Population.h"
#include"StatisticFile.h"
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
using namespace std;

int main(int argc, char*argv[]) {

	const int iterative = 30;//the total number of independent runs
	int run;
	int job;
	char base[200];

	sscanf(argv[1], "%d", &run);
	sscanf(argv[2], "%d", &job);
	sscanf(argv[3], "%s", base);

	StatisticFile SF;
	SF.initFile(run, job, base);

	clock_t start, end;
	{
		SF.problemInit(iterative);
		{
			if (run >= 10 && run < 13) { //indices of Airfoli, BHouse, Tower
				norm_flag = true;  //perform noramlization (standard scalar)
			}
			else {
				norm_flag = false;
			}

			readTrainData(run, job);
			SF.independentRunInit(run, job);
			double suc = 0, suc_R2 = 0, eva_times = 0, train_err = 1e6, test_err = 1e6, test_R2 = 0, prog_size = 0, time = 0;

			start = clock();
			srand((1 + run)*(job + 1));

			evaluation = 0;
			Population pop;
			pop.evolve(SF);


			end = clock();
			dfTraTime = (double)(end - start) / CLOCKS_PER_SEC;

			if (pop.bestPro.fitness < 1e-4) {
				suc = 1;
			}
			eva_times = evaluation;
			train_err = pop.bestPro.fitness;
			prog_size = pop.bestPro.countProLen();
			time = dfTraTime;

			//testing
			readTestData(run, job);
			pop.bestPro.program_test_exe();
			test_err = pop.bestPro.fitness;
			test_R2 = pop.bestPro.get_test_R2();
			if (test_R2 > 0.999) {
				suc_R2 = 1;
			}

			//load into file
			SF.independentRunRecord(run, job, suc, suc_R2, eva_times, train_err, test_err, test_R2, prog_size, pop.bestPro.name, time);
		}
		SF.problemOveralRecord(run);
	}

	return 0;
}


