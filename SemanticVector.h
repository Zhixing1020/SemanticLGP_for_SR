#pragma once
#include"Parameters.h"
#include"Operations.h"
#include"Instructions.h"
//#include<vector>
#include<valarray>

class SV {
public:
	static const int maxsize = MAXSIZE_SV;
	std::valarray<double>content;
	SV() { content.resize(min(input_num, maxsize)*MaxReg, 0); }
	double vec_diff(SV &sv2);
	void setSem(int index, double* sv2, int len = MaxReg);
	void randSetSem(double lowb = -2, double upb = 2);
	SV & operator = (const SV & sv2) { content = sv2.content; return *this; }
	double & operator [] (int i) { return content[i]; }
	bool operator == (const SV & A) { return (content == A.content).min(); }
	int size() { return content.size(); }
};

double SV::vec_diff(SV &sv2) {
	if (sv2.size() != content.size()) {
		printf("dimension inconsistent in vec_diff\n");
		system("pause");
		return Inf;
	}
	return (abs(content - sv2.content)).sum();
}

void SV::setSem(int index, double* sv2, int len) {
	//index: the index of input
	valarray<double> vtmp(sv2, len);
	if(index*MaxReg + len < maxsize)
		content[slice(index*MaxReg, len, 1)] = vtmp;
}

void SV::randSetSem(double lowb, double upb) {
	for (int i = 0; i < content.size(); i++) {
		content[i] = ((double)rand()) / RAND_MAX*(upb - lowb) + lowb;
	}
}