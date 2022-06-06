#pragma once
#include"Parameters.h"
#include<cmath>
#include<cstring>

void StandardScalar_transform(double * raw, double * des, int len, double * mean_p, double *stdu_p);
void StandardScalar_fit_transform(double* raw, double* des, int len, double * mean_p, double *stdu_p);

void genData() {
	double d;
	memset(train_input, 0, sizeof(train_input));
	memset(train_output, 0, sizeof(train_output));
	for (int i = 0; i < MaxDataNum; i++) {
		d = randval(0,2);
		train_input[i*MaxDataDim] = d;
		train_output[i*MaxDataDim] = log(d*d + 1) + log(d + 1);
	}
}

void readTrainData(int run, int job) {
	int i, j;
	FILE *f;
	char name[200];
	sprintf(name, "/SLGP_exp/SLGP_for_SR_CYB/F%d_%d_training_data.txt", run, job);

	printf("%s\n", name);
	f = fopen(name, "r");
	int row_num, col_num;
	fscanf(f, "%d\t%d\n", &row_num, &col_num);
	input_num = row_num>MaxDataNum ? MaxDataNum : row_num;                 //save the input_num
																		 //input_num = row_num;
	in_dimen = col_num;  //save the dimension
	out_dimen = 1;
	for (i = 0; i < row_num; i++) {
		for (j = 0; j < col_num; j++) {
			fscanf(f, "%lf\t", &raw_train_input[i*MaxDataDim + j]);    //save the training inputs

		}
		fscanf(f, "%lf\n", &raw_train_output[i*MaxDataDim + 0]);          //save the training ouputs
	}
	fclose(f);

	if (norm_flag) {
		StandardScalar_fit_transform(raw_train_input, train_input, in_dimen, mean, stdu);
		StandardScalar_fit_transform(raw_train_output, train_output, 1, &out_mean, &out_stdu);
	}
	else {
		for (int i = 0; i < input_num; i++) {
			for (int j = 0; j < in_dimen; j++) {
				train_input[i*MaxDataDim + j] = raw_train_input[i*MaxDataDim + j];
				
			}
			train_output[i*MaxDataDim + 0] = raw_train_output[i*MaxDataDim + 0];
		}
		out_mean = 0, out_stdu = 1;
	}
}

void readTestData(int run, int job) {
	int i, j;
	FILE *f;
	char name[200];
	sprintf(name, "/SLGP_exp/SLGP_for_SR_CYB/F%d_%d_testing_data.txt", run, job);

	printf("%s\n", name);
	f = fopen(name, "r");
	int row_num, col_num;
	fscanf(f, "%d\t%d\n", &row_num, &col_num);
	input_num = row_num>MaxDataNum ? MaxDataNum : row_num;                 //save the input_num
																	 //input_num = row_num;
	in_dimen = col_num;  //save the dimension
	out_dimen = 1;
	for (i = 0; i < row_num; i++) {
		for (j = 0; j < col_num; j++) {
			fscanf(f, "%lf\t", &raw_test_input[i*MaxDataDim + j]);    //save the training inputs

		}
		fscanf(f, "%lf\n", &test_output[i*MaxDataDim + 0]);          //save the training ouputs
	}
	fclose(f);

	if (norm_flag) {
		StandardScalar_transform(raw_test_input, test_input, in_dimen, mean, stdu);
		//since we cannot see test data, we use the mean and std of training data to normalize test inputs, and convert SLGP outputs back to the range of test outputs (see "Program.h/get_test_fitness()")
	}
	else {
		for (int i = 0; i < input_num; i++) {
			for (int j = 0; j < in_dimen; j++) {
				test_input[i*MaxDataDim + j] = raw_test_input[i*MaxDataDim + j];

			}
		}
		out_mean = 0, out_stdu = 1;
	}
}


void StandardScalar_transform(double * raw, double * des, int len, double * mean_p, double *stdu_p) {
	for (int j = 0; j < len; j++) {
		//get normalized data
		for (int i = 0; i < input_num; i++) {
			des[i*MaxDataDim + j] = (raw[i*MaxDataDim + j] - mean_p[j]) / stdu_p[j];
		}

	}
}

void StandardScalar_fit_transform(double* raw, double* des, int len, double * mean_p, double *stdu_p) {
	for (int j = 0; j < len; j++) {
		mean_p[j] = 0, stdu_p[j] = 0;

		//get mean
		for (int i = 0; i < input_num; i++) {
			mean_p[j] += raw[i*MaxDataDim + j] / input_num;
		}

		//get std
		for (int i = 0; i < input_num; i++) {
			stdu_p[j] += (raw[i*MaxDataDim + j] - mean_p[j])*(raw[i*MaxDataDim + j] - mean_p[j])/input_num;
		}
		stdu_p[j] = sqrt(stdu_p[j]);

		//get normalized data
		StandardScalar_transform(raw, des, len, mean_p, stdu_p);

	}
	
}