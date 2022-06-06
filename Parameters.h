#pragma once

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

//data
#define DATA_RNG 2
#define MaxDataDim 130
#define MaxDataNum 5000
#define DataDim 1
#define outDataDim 1
#define Inf 1e7
#define MAXSIZE_SV 3800  

double inputs[MaxDataDim] = { 0 };
double outputs[MaxDataDim];
double raw_train_input[MaxDataNum*MaxDataDim];
double raw_train_output[MaxDataNum*MaxDataDim];
double train_input[MaxDataNum*MaxDataDim];   //for un-normalized dataset, normalize them by standard scalar
double train_output[MaxDataNum*MaxDataDim];
double mean[MaxDataDim], stdu[MaxDataDim];// for standard scalar
double out_mean, out_stdu;
double test_input[MaxDataNum*MaxDataDim];
double raw_test_input[MaxDataNum*MaxDataDim];
double test_output[MaxDataNum*MaxDataDim];

int input_num = 0;
int in_dimen = 0;
int out_dimen = 0;

bool norm_flag = false;

//Op
#define MaxChild 2
#define MaxReg 1
double registers[MaxReg];
#define NUM_TERM 3

//Instruction
#define MaxDepth 5
#define MUTATION_THRESH 0.05
#define CROSSOVER_THRESH 0.05

//Program
#define MaxProLen 5

//Population
#define POPSIZE 50
double evaluation = 0;
#define MAXGEN 100000
#define MAXEVAL 1e7

//Semantic library
#define MaxIns 2000
#define NUMREF 5
#define NumUpdate 1000
#define SLTOUSIZE 20
#define SPTOUSIZE 500
#define DecayF 0.8
#define UpdatePeriod 20
#define Pthresold 0.95
#define DISTRIBUTE UpdatePeriod*POPSIZE

//timing
//LARGE_INTEGER litmp;
//LONGLONG   Begin, End;
double      dfMinus, dfFreq, dfTraTime, dfTesTime;

//global function
double randval(double L = 0, double U = 1) {
	return (double)rand() / RAND_MAX*(U - L) + L;
}

int randint(int L = 0, int U = RAND_MAX) {
	return rand() % (U - L + 1) + L;
}

#endif // !DEF



