#pragma once
#include "Parameters.h"
#include "Program.h"
#include "SemanticLibrary.h"
#include "StatisticFile.h"

class Population {
public:
	int generation = 0;
	array<Program, POPSIZE> programList;
	Program bestPro;
	SemLibrary semLib;
	Population();
	void updateBestPro();
	void reproduction();
	void evolve(StatisticFile &SF);
};

Population::Population() {
	//evaluation = 0;
	for (array<Program, POPSIZE>::iterator it = programList.begin(); it != programList.end(); it++) {
		it->program_exe();
	}
	updateBestPro();
}

void Population::updateBestPro() {
	for (array<Program, POPSIZE>::iterator it = programList.begin(); it != programList.end(); it++) {
		if (it->fitness < bestPro.fitness) {
			bestPro = *it;
		}
	}
}

void Population::reproduction() {
	array<Program, POPSIZE> trailProList;
	array<Program, POPSIZE>::iterator it1;
	array<Program, POPSIZE>::iterator it2;

	for (it1 = trailProList.begin(), 
		it2 = programList.begin(); 
		it2 != programList.end(); 
		it1++, it2++) {
		*it1 = *it2;

		if (randval() < 0.5) {
			Program r2;
			int ind = rand() % POPSIZE;
			r2 = programList[ind];
			if (r2 == *it2) r2 = programList[(++ind) % POPSIZE];
			it1->proDEMutate(bestPro, r2);
		}
		else {
			it1->MutateAndDivide(semLib, it1->inputS, it1->targetS);
			it1->get_name();
		}
			

		it1->program_exe();
	}

	//tournament selection, size = 2
	for (it1 = trailProList.begin(), it2 = programList.begin(); it1 != trailProList.end(); it1++, it2++) {
		if (it1->fitness <= it2->fitness) {
			*it2 = *it1;
		}
	}

	updateBestPro();
}

void Population::evolve(StatisticFile &SF) {
	generation = 0;
	while (true) {
		if (generation % 50 == 0) {
			cout << generation << " : " << evaluation << "->" << bestPro.fitness << endl;
			cout << bestPro.name << endl;
		}

		reproduction();
		generation++;

		if (generation % UpdatePeriod == 0) {
			semLib.updateLibrary();
			cout << semLib.countItem() << endl;
		}

		SF.procedureRecord(evaluation, bestPro.fitness);

		if (evaluation > MAXEVAL || bestPro.fitness < 1e-4) {
			cout << generation << " : " << evaluation << "->" << bestPro.fitness << endl;
			cout << bestPro.name << endl;
			break;
		}
	}
}