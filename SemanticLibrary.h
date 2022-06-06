#pragma once
#include "SemanticVector.h"
#include "Instructions.h"
#include <vector>
#include <array>

typedef struct SVSet{
	array<SV, NUMREF> sem_vec;

	bool operator == (const SVSet& A) {
		for (int i = 0; i < NUMREF; i++) {
			if (!(sem_vec[i] == A.sem_vec[i])) return false;
		}
		return true;
	}
	SVSet & operator = (const SVSet & A) {
		for (int i = 0; i < NUMREF; i++) {
			sem_vec[i] = A.sem_vec[i];
		}
		return *this;
	}

	SV & operator [] (int i) {
		if (i < NUMREF) {
			return sem_vec[i];
		}
		cerr << "SVSet index out of range" << endl;
		return sem_vec[0];
	}
};

class SemLibrary {
	array<SV, NUMREF> inRefSV;
	vector<Instruction> InsList;
	vector<SVSet> outRefSV;
	vector<double> occurFre;
public:
	SemLibrary();
	SVSet evalInstr(Instruction &ins);
	void updateLibrary();
	SV estimateX(const int insInd, SV Y_star);
	SV estimateY(const int insInd, SV X_star);
	void selectInstr(SV inputS, SV Sdes, Instruction * Tinstr, SV * DI, SV * EO);
	int countItem() { return InsList.size(); }
};

SemLibrary::SemLibrary() {
	InsList.reserve(MaxIns); 
	outRefSV.reserve(MaxIns); 
	occurFre.reserve(MaxIns);

	for (array<SV, NUMREF>::iterator it = inRefSV.begin(); it != inRefSV.end(); it++) {
		it->randSetSem(-DATA_RNG, DATA_RNG);
	}

	for (int i = 0; i < MaxIns; i++) {
		Instruction ins;
		if (find(InsList.begin(), InsList.end(), ins) == InsList.end()) { //ins is not in existing library
			SVSet outSV = evalInstr(ins);
			if (find(outRefSV.begin(), outRefSV.end(), outSV) == outRefSV.end()) { //outSV is not in outRefSV
				InsList.push_back(ins);
				(InsList.end() - 1)->root = ins.root->clone();
				outRefSV.push_back(outSV);
				occurFre.push_back(0);
			}
		}
		
	}
}

SVSet SemLibrary::evalInstr(Instruction &ins){
	SVSet tmpout;
	for (int n = 0; n < NUMREF; n++) {
		for (int i = 0; i < min(input_num, SV::maxsize); i++) {
			//set the input and registers
			for (int j = 0; j < in_dimen; j++) {
				inputs[j] = train_input[i*MaxDataDim + j];
			}
			for (int j = 0; j < MaxReg; j++) {
				registers[j] = inRefSV[n][i*MaxReg + j];
			}

			//instruction execution
			ins.eval();
			
			//put into outRefSV
			for (int j = 0; j < MaxReg; j++) {
				tmpout.sem_vec[n][i*MaxReg + j] = registers[j];
			}
		}
	}
	evaluation += NUMREF*ins.numNode;
	return tmpout;
}

void SemLibrary::updateLibrary() {
	array<Instruction, NumUpdate> tmp_ins_list;
	array<int, NumUpdate> elim_list;
	int p1 = -1, p2 = -1, p3 = -1, e1 = -1;
	int ci;

	for (int t = 0; t < NumUpdate; t++) {
		p1 = -1, p2 = -1, p3 = -1, e1 = -1;

		for (int sl = 0; sl < SLTOUSIZE; sl++) {
			ci = rand() % InsList.size();
			if (p1 == -1 || (occurFre[ci] > occurFre[p1] && randval() <= min(occurFre[ci] / (DISTRIBUTE), Pthresold))) {
				p1 = ci;
			}
		}

		for (int sl = 0; sl < SLTOUSIZE; sl++) {
			ci = rand() % InsList.size();
			if (ci != p1 && (p2 == -1 || (occurFre[ci] > occurFre[p2] && randval() <= min(occurFre[ci] / (DISTRIBUTE), Pthresold)))) {
				p2 = ci;
			}
		}

		for (int sl = 0; sl < SLTOUSIZE; sl++) {
			ci = rand() % InsList.size();
			if (ci != p1 &&  ci != p2 && (p3 == -1 || (occurFre[ci] > occurFre[p3] && randval() <= min(occurFre[ci] / (DISTRIBUTE), Pthresold)))) {
				p3 = ci;
			}
		}

		Instruction trail;
		trail = InsList[p1];

		trail.DE_mutate(InsList[p2].root, InsList[p3].root);
		tmp_ins_list[t] = trail;
	}

	for (int t = 0; t < NumUpdate; t++) {
		e1 = -1;
		for (int sl = 0; sl < SLTOUSIZE; sl++) {
			ci = rand() % InsList.size();
			bool A = (ci != p1 && ci != p2 && ci != p3);
			bool B = (e1 == -1 || (occurFre[ci] < occurFre[e1] && randval() > min(occurFre[ci] / (DISTRIBUTE), Pthresold) && find(elim_list.begin(), elim_list.end(), e1) == elim_list.end()));
			if (A && B) {
				e1 = ci;
			}
		}
		elim_list[t] = e1;
	}

	for (int t = 0; t < NumUpdate; t++) {
		Instruction trail;
		trail = tmp_ins_list[t];

		if (find(InsList.begin(), InsList.end(), trail) == InsList.end()) { //ins is not in existing library
			SVSet outSV = evalInstr(trail);
			if (find(outRefSV.begin(), outRefSV.end(), outSV) == outRefSV.end()) { //outSV is not in outRefSV
				if (InsList.size() < MaxIns) {
					InsList.push_back(trail);
					(InsList.end() - 1)->root = trail.root->clone();
					outRefSV.push_back(outSV);
					occurFre.push_back(0);
				}
				else {
					e1 = elim_list[t];
					InsList[e1] = trail;
					outRefSV[e1] = outSV;
					occurFre[e1] = 0;
				}
			}
		}
	}

	for (vector<double>::iterator it = occurFre.begin(); it != occurFre.end(); it++) {
		*it *= DecayF;
	}
}

SV SemLibrary::estimateX(const int insInd, SV Y_star) {
	//find the nearest index
	SVSet recordSV = outRefSV[insInd];
	int index0 = 0;
	double diff = recordSV.sem_vec[index0].vec_diff(Y_star);
	for (int i = 1; i < NUMREF; i++) {
		if (recordSV.sem_vec[i].vec_diff(Y_star) < diff) {
			index0 = i;
			diff = recordSV.sem_vec[i].vec_diff(Y_star);
		}
	}
	if (diff == 0) {
		return inRefSV[index0];
	}
	//get effective registers
	set<int> readRegs = InsList[insInd].getReadReg();

	//estimate X
	SV X_star;
	for (int k = 0; k < X_star.size(); k++) {
		if (readRegs.find(k%MaxReg) == readRegs.end()) {
			X_star[k] = Y_star[k];
		}
		else {
			double x0k = inRefSV[index0][k];
			double dey_star = Y_star[k] - recordSV.sem_vec[index0][k];
			double sumX = 0, sumY = 0;
			for (int i = 0; i < NUMREF; i++) {
				if (i != index0) {
					sumX += inRefSV[i][k] - x0k;
					sumY += recordSV.sem_vec[i][k] - recordSV.sem_vec[index0][k];
				}
			}
			if (dey_star == 0 || sumX == 0) {
				X_star[k] = x0k;
			}
			else if (sumY != 0) {
				X_star[k] = x0k + dey_star*(sumX / sumY);
			}
			else X_star[k] = Inf;

			if (X_star[k] > Inf || X_star[k] < -Inf) X_star[k] = Inf;
		}
	}

	return X_star;
}

SV SemLibrary::estimateY(const int insInd, SV X_star) {
	//find the nearest index
	SVSet recordSV = outRefSV[insInd];
	int index0 = 0;
	double diff = inRefSV[index0].vec_diff(X_star);
	for (int i = 1; i < NUMREF; i++) {
		if (inRefSV[i].vec_diff(X_star) < diff) {
			index0 = i;
			diff = inRefSV[i].vec_diff(X_star);
		}
	}
	if (diff == 0) {
		return recordSV[index0];
	}
	//get effective registers
	set<int> WrRegs = InsList[insInd].getWriteReg();

	//estimate Y
	SV Y_star;
	for (int k = 0; k < Y_star.size(); k++) {
		if (WrRegs.find(k%MaxReg) == WrRegs.end()) {
			Y_star[k] = X_star[k];
		}
		else {
			double y0k = recordSV.sem_vec[index0][k];
			double dex_star = X_star[k] - inRefSV[index0][k];
			double sumX = 0, sumY = 0;
			for (int i = 0; i < NUMREF; i++) {
				if (i != index0) {
					sumX += inRefSV[i][k] - inRefSV[index0][k];
					sumY += recordSV.sem_vec[i][k] - y0k;
				}
			}
			if (dex_star == 0 || sumY == 0) {
				Y_star[k] = y0k;
			}
			else if (sumX != 0) {
				Y_star[k] = y0k + dex_star*(sumY / sumX);
			}
			else Y_star[k] = Inf;

			if (Y_star[k] > Inf || Y_star[k] < -Inf) Y_star[k] = Inf;
		}
	}

	return Y_star;
}

void SemLibrary::selectInstr(SV inputS, SV Sdes, Instruction * Tinstr, SV * DI, SV * EO) {
	int tci=-1, ci=0;
	double min_diff = 1e6, diff=0;
	for (int sp = 0; sp < SPTOUSIZE; sp++) {
		ci = rand() % InsList.size();
		if (tci == -1 || randval() > min(occurFre[ci] / (DISTRIBUTE), Pthresold)) {
			diff = estimateY(ci, inputS).vec_diff(Sdes);
			if (tci == -1 || diff < min_diff || (diff == min_diff && randval()<0.5)) {
				min_diff = diff;
				tci = ci;
			}
		}
	}

	*Tinstr = InsList[tci];
	*DI = estimateX(tci, Sdes);
	*EO = estimateY(tci, inputS);

	occurFre[tci] += 1;
}