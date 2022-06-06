#pragma once
#include"Instructions.h"
#include"Parameters.h"
#include"SemanticLibrary.h"
#include"SemanticVector.h"

class Program {
public:
	vector<Instruction> pro_body;
	vector<SV> sem_con;
	double fitness;
	string name;
	SV inputS;
	SV targetS;
	double *actual_output;

	Program();
	~Program();
	void ini_program();
	void ini_semcon();
	void get_name();
	void ini_exe(int index);
	void ini_test_exe(int index);
	void record_semantic(int insInd, int in);
	double get_train_fitness();
	double get_test_fitness();
	double get_test_R2();
	void program_exe();
	void program_test_exe();
	int countProLen();
	Program & operator = (const Program & p);
	bool operator == (const Program & A) { return name == A.name; };
	void proMutate();
	void proCrossover(const Program & donatePro);
	void proDEMutate(const Program &target, const Program &r2);
	SV MutateAndDivide(SemLibrary &SemLib, SV InS, SV desS, int head = -1, int tail = MaxProLen - 1, int STEP = 0);
};

Program::Program() {
	pro_body.reserve(MaxProLen);
	sem_con.reserve(MaxProLen);
	ini_program();
	ini_semcon();
	get_name();
	fitness = Inf;

	actual_output = new double[MaxDataNum*outDataDim];
}

Program::~Program() {
	delete[] actual_output;
}

void Program::ini_program() {
	pro_body.clear();

	for (int p = 0; p < MaxProLen; p++) {
		Instruction ins;
		pro_body.push_back(ins);
		(pro_body.end() - 1)->root = ins.root->clone();
	}
}

void Program::get_name() {
	name = "";
	for (vector<Instruction>::iterator it = pro_body.begin(); it != pro_body.end(); it++) {
		name += (it->name + "\n");
	}
}

void Program::ini_semcon() {
	sem_con.clear();
	for (int p = 0; p < MaxProLen; p++) {
		SV tmps;
		sem_con.push_back(tmps);
	}

	//problem specific
	for (int i = 0; i < input_num; i++) {
		if (i*MaxReg < targetS.size())
			targetS[i*MaxReg] = train_output[i*MaxDataDim];
	}
}

void Program::ini_exe(int index) {
	memset(registers, 0, sizeof(registers));
	for (int i = 0; i < in_dimen; i++) {
		inputs[i] = train_input[index*MaxDataDim + i];
	}
	for (int i = 0; i < out_dimen; i++) {
		outputs[i] = train_output[index*MaxDataDim + i];
	}

}

void Program::ini_test_exe(int index) {
	memset(registers, 0, sizeof(registers));
	for (int i = 0; i < in_dimen; i++) {
		inputs[i] = test_input[index*MaxDataDim + i];
	}
	for (int i = 0; i < out_dimen; i++) {
		outputs[i] = test_output[index*MaxDataDim + i];
	}

}

void Program::record_semantic(int insInd, int in) {
	sem_con[insInd].setSem(in, registers);
}

double Program::get_train_fitness() {
	fitness = 0;
	for (int i = 0; i < input_num; i++) {
		for (int j = 0; j < out_dimen; j++) {
			fitness += pow(min((train_output[i*MaxDataDim + j]*out_stdu + out_mean) - (actual_output[i*outDataDim + j]*out_stdu + out_mean), 1e4), 2);
		}
	}
	fitness = sqrt(fitness / input_num);
	return fitness;
}

double Program::get_test_fitness() {
	fitness = 0;
	for (int i = 0; i < input_num; i++) {
		for (int j = 0; j < out_dimen; j++) {
			fitness += pow(min(test_output[i*MaxDataDim + j] - (actual_output[i*outDataDim + j]*out_stdu + out_mean), 1e4), 2);
		}
	}
	fitness = sqrt(fitness / input_num);
	return fitness;
}

double Program::get_test_R2() {
	double res = 0, mean = 0, var = 0;
	//get MSE
	for (int i = 0; i < input_num; i++) {
		for (int j = 0; j < out_dimen; j++) {
			res += pow(min(test_output[i*MaxDataDim + j] - (actual_output[i*outDataDim + j]*out_stdu + out_mean), 1e4), 2);
		}
	}
	res /= input_num;
	
	//get VAR
	for (int i = 0; i < input_num; i++) {
		mean += test_output[i*MaxDataDim + 0];
	}
	mean /= input_num;
	for (int i = 0; i < input_num; i++) {
		var += pow((test_output[i*MaxDataDim + 0] - mean), 2);
	}
	var /= input_num;

	res = 1. - res / var;

	return res;
}

void Program::program_exe() {
	memset(actual_output, 0, sizeof(actual_output));
	for (int i = 0; i < input_num; i++) {
		ini_exe(i);
		for (int ins = 0; ins < MaxProLen; ins++) {
			pro_body[ins].eval();
			record_semantic(ins, i);
		}

		actual_output[i*outDataDim] = registers[0];
	}
	evaluation += countProLen();
	get_train_fitness();
}

void Program::program_test_exe() {
	//initialize the semantic context
	sem_con.clear();
	for (int p = 0; p < MaxProLen; p++) {
		SV tmps;
		sem_con.push_back(tmps);
	}

	memset(actual_output, 0, sizeof(actual_output));
	for (int i = 0; i < input_num; i++) {
		ini_test_exe(i);
		for (int ins = 0; ins < MaxProLen; ins++) {
			pro_body[ins].eval();
			record_semantic(ins, i);
		}

		actual_output[i*outDataDim] = registers[0];
	}
	get_test_fitness();
}

int Program::countProLen() {
	int cnt = 0;
	for (vector<Instruction>::iterator it = pro_body.begin(); it != pro_body.end(); it++) {
		cnt += it->countNode();
	}
	return cnt;
}

Program & Program::operator=(const Program & p) {
	this->pro_body = p.pro_body;
	this->sem_con = p.sem_con;
	this->name = p.name;
	this->fitness = p.fitness;
	this->inputS = p.inputS;
	this->targetS = p.targetS;
	return *this;
}

void Program::proMutate() {
	for (vector<Instruction>::iterator it = pro_body.begin(); it != pro_body.end(); it++) {
		it->mutateInstruction();
	}
	get_name();
}

void Program::proCrossover(const Program & donatePro) {
	Instruction don_ins;
	for (int i = 0; i < MaxProLen; i++) {
		don_ins = donatePro.pro_body[rand() % MaxProLen];
		pro_body[i].crossOver(&don_ins);
	}
	get_name();
}

void Program::proDEMutate(const Program &target, const Program &r2) {
	Instruction tar_ins, r2Ins;
	for (int i = 0; i < MaxProLen; i++) {
		tar_ins = target.pro_body[i];
		r2Ins = r2.pro_body[i];
		pro_body[i].DE_mutate(tar_ins.root, r2Ins.root);
	}
	get_name();
}

SV Program::MutateAndDivide(SemLibrary &SemLib, SV InS, SV desS, int head, int tail, int STEP) {

	if (STEP == 0) {
		STEP = randint(1, MaxProLen);
	}

	int InputSlot = max(head, randint(tail - STEP, tail - 1));
	int MutateSlot = randint(InputSlot + 1, tail);
	SV X_star;
	if (InputSlot == head) {
		X_star = InS;
	}
	else {
		X_star = sem_con[InputSlot];
	}

	SV DI, EO, nestedDI;
	Instruction trail;
	SemLib.selectInstr(X_star, desS, &trail, &DI, &EO);

	this->pro_body[MutateSlot] = trail;
	nestedDI = DI;

	if (MutateSlot < tail) {
		MutateAndDivide(SemLib, EO, desS, MutateSlot, tail, STEP);
	}
	if (InputSlot < MutateSlot - 1) {
		nestedDI = MutateAndDivide(SemLib, X_star, DI, InputSlot, MutateSlot - 1, STEP);
	}
	if (head < InputSlot) {
		nestedDI = MutateAndDivide(SemLib, InS, nestedDI, head, InputSlot, STEP);
	}

	return nestedDI;
}