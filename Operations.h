#pragma once
#include "Parameters.h"
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

class Op
{
public:
	string label;
	int numChildren;
	Op* children[MaxChild];
	Op() { } // to be used later
	virtual ~Op() { } // to be used later
	virtual double eval(int index=0) = 0;  //setting the 0 makes it a PURE
	virtual Op* clone() = 0; //make a deep copy of the current tree
	virtual string getLabel() = 0;
};

//class for storing constant values
class ConstNode : public Op {
	double constVal;
public:
	ConstNode();
	ConstNode(double preSetVal);
	double eval(int index = 0);
	ConstNode* clone();
	string getLabel();
};

//class for using inputs
class InputNode : public Op {
	int inputIndex;
public:
	InputNode();
	InputNode(int inputInd);
	double eval(int index = 0);
	InputNode* clone();
	void setValues(int inputInd);
	string getLabel();
};

//Read registers
class ReadReg : public Op {
public:
	int regIndex;
	ReadReg();
	ReadReg(int regInd);
	double eval(int index = 0);
	ReadReg* clone();
	void setValues(int regInd);
	string getLabel();
};

//Write registers
class WriteReg : public Op {
public:
	int regIndex;
	WriteReg();
	WriteReg(int regInd);
	double eval(int index = 0);
	WriteReg* clone();
	void setValues(int regInd);
	string getLabel();
};

//addition
class Add : public Op {
public:
	Add();
	double eval(int index = 0);
	Add* clone();
	string getLabel();
};

//subtraction
class Subtract : public Op {
public:
	Subtract();
	double eval(int index = 0);
	Subtract* clone();
	string getLabel();
};
//multiplication
class Multiply : public Op {
public:
	Multiply();
	double eval(int index = 0);
	Multiply* clone();
	virtual string getLabel();
};
//division
class Divide : public Op {
public:
	Divide();
	double eval(int index = 0);
	Divide* clone();
	string getLabel();
};
//sin
class Sin : public Op {
public:
	Sin();
	double eval(int index = 0);
	Sin* clone();
	string getLabel();
};

//cos
class Cos : public Op {
public:
	Cos();
	double eval(int index = 0);
	Cos* clone();
	string getLabel();
};

//exp
class Exp : public Op {
public:
	Exp();
	double eval(int index = 0);
	Exp* clone();
	string getLabel();
};

//ln
class Ln : public Op {
public:
	Ln();
	double eval(int index = 0);
	Ln* clone();
	string getLabel();
};


//=======================================================================================================================



//
//		ConstNode
//
ConstNode::ConstNode() {
	this->numChildren = 0;
	this->constVal = rand() / (double)RAND_MAX;
	char str[20] = "";
	sprintf(str, "%f", constVal);
	this->label = str;
}
ConstNode::ConstNode(double preSetVal) {
	this->numChildren = 0;
	this->constVal = preSetVal;
	char str[20] = "";
	sprintf(str, "%f", constVal);
	this->label = str;
}
double ConstNode::eval(int index) {
	return this->constVal;
}
ConstNode* ConstNode::clone() {
	ConstNode* retNode = new ConstNode(constVal);
	return retNode;
}
string ConstNode::getLabel() {
	return this->label;
}
//
//		InputNode
//
InputNode::InputNode() {
	this->numChildren = 0;
	inputIndex = rand() % in_dimen;
	setValues(inputIndex);
}
InputNode::InputNode(int inputInd) {
	this->numChildren = 0;
	setValues(inputInd);
}
double InputNode::eval(int index) {
	return inputs[this->inputIndex];
}
void InputNode::setValues(int inIndex) {
	this->inputIndex = inIndex;
	char str[20] = "";
	sprintf(str, "x%d", inIndex);
	this->label = str;
}
InputNode* InputNode::clone() {
	InputNode* retNode = new InputNode();
	retNode->setValues(this->inputIndex);
	return retNode;
}
string InputNode::getLabel() {
	return this->label;
}
//
//     ReadReg
//
ReadReg::ReadReg() {
	this->numChildren = 0;
	this->regIndex = rand() % MaxReg;
	setValues(this->regIndex);
}
ReadReg::ReadReg(int regInd) {
	this->numChildren = 0;
	setValues(regInd);
}
double ReadReg::eval(int index) {
	double res = registers[this->regIndex];
	if (res < -1 * Inf || res > Inf) {
		res = registers[this->regIndex] = Inf;
	}
	return res;
}
void ReadReg::setValues(int regIndex) {
	this->regIndex = regIndex;
	char str[20] = "";
	sprintf(str, "v%d", regIndex);
	this->label = str;
}
ReadReg* ReadReg::clone() {
	ReadReg* retNode = new ReadReg();
	retNode->setValues(this->regIndex);
	return retNode;
}
string ReadReg::getLabel() {
	return this->label;
}
//
//     WriteReg
//
WriteReg::WriteReg() {
	this->numChildren = 1;
	this->regIndex = rand() % MaxReg;
	setValues(this->regIndex);
}
WriteReg::WriteReg(int regInd) {
	this->numChildren = 1;
	setValues(regInd);
}
double WriteReg::eval(int index) {
	if (this->children[0] != NULL) {
		double res = 0;
		res = this->children[0]->eval(index);
		if (res < -1 * Inf || res > Inf) {
			res = Inf;
		}
		registers[this->regIndex] = res;
	}
	else
	{
		cerr << "the child is not defined in WriteReg" << endl;
		return -1.0;
	}
	return registers[this->regIndex];
}
void WriteReg::setValues(int regIndex) {
	this->regIndex = regIndex;
	char str[20] = "";
	sprintf(str, "v%d=", regIndex);
	this->label = str;
}
WriteReg* WriteReg::clone() {
	WriteReg* retNode = new WriteReg();
	retNode->setValues(this->regIndex);
	for (int i = 0; i<this->numChildren; i++) {
		retNode->children[i] = this->children[i]->clone();
	}
	return retNode;
}
string WriteReg::getLabel() {
	return this->label;
}
//
//		Add
//
Add::Add() {
	this->numChildren = 2;
	label = "+";
}
double Add::eval(int index) {
	if (this->children[0] != NULL && this->children[1] != NULL) {
		double a = this->children[0]->eval(index);
		double b = this->children[1]->eval(index);
		double res = a + b;
		if (res < -1 * Inf || res > Inf) {
			res = Inf;
		}
		return res;
	}
	else {
		cerr << "left and right not defined in add" << endl;
		return -1.0;
	}
}
Add* Add::clone() {
	Add* retNode = new Add();
	for (int i = 0; i<numChildren; i++) {
		retNode->children[i] = this->children[i]->clone();
	}
	return retNode;
}
string Add::getLabel() {
	return this->label;
}
//
//		Subtract
//
Subtract::Subtract() {
	this->numChildren = 2;
	this->label = "-";
}
double Subtract::eval(int index) {
	if (this->children[0] != NULL && this->children[1] != NULL) {
		double a = this->children[0]->eval(index);
		double b = this->children[1]->eval(index);
		double res = a - b;
		if (res < -1 * Inf || res > Inf) {
			res = Inf;
		}
		return res;
	}
	else {
		cerr << "left and right not defined in sub" << endl;
		return -1.0;
	}
}
Subtract* Subtract::clone() {
	Subtract* retNode = new Subtract();
	for (int i = 0; i<this->numChildren; i++) {
		retNode->children[i] = this->children[i]->clone();
	}
	return retNode;
}
string Subtract::getLabel() {
	return this->label;
}
//
//		Multiply
//
Multiply::Multiply() {
	this->numChildren = 2;
	this->label = "*";
}
double Multiply::eval(int index) {
	if (this->children[0] != NULL && this->children[1] != NULL) {
		double a = this->children[0]->eval(index);
		double b = this->children[1]->eval(index);
		double res = a * b;
		if (res < -1 * Inf || res > Inf) {
			res = Inf;
		}
		return res;
	}
	else {
		cerr << "left and right not defined in mul" << endl;
		return -1.0;
	}
}
Multiply* Multiply::clone() {
	Multiply* retNode = new Multiply();
	for (int i = 0; i<numChildren; i++) {
		retNode->children[i] = this->children[i]->clone();
	}
	return retNode;
}
string Multiply::getLabel() {
	return this->label;
}
//
//		Divide
//
Divide::Divide() {
	this->numChildren = 2;
	this->label = "/";
}
double Divide::eval(int index) {
	if (this->children[0] != NULL && this->children[1] != NULL) {
		double v0 = children[0]->eval(index);
		double v1 = children[1]->eval(index);
		double res = 0;
		res = v0 / sqrt(1 + v1*v1);
		/*if (v1 != 0)
			res = v0 / v1;
		else
			res = v0;
		if (res < -1 * Inf || res > Inf) {
			res = Inf;
		}*/
		return res;
	}
	else {
		cerr << "left and right not defined in div" << endl;
		return -1.0;
	}

}
Divide* Divide::clone() {
	Divide* retNode = new Divide;
	for (int i = 0; i<numChildren; i++) {
		retNode->children[i] = this->children[i]->clone();
	}
	return retNode;
}
string Divide::getLabel() {
	return this->label;
}

//
//     Sin
//
Sin::Sin() {
	this->numChildren = 1;
	this->label = "sin";
}
double Sin::eval(int index) {
	if (this->children[0] != NULL) {
		double v0 = children[0]->eval(index);
		double res = 0;
		res = sin(v0);

		if (res < -1 * Inf || res > Inf) {
			res = Inf;
		}
		return res;
	}
	else {
		cerr << "left and right not defined in Sin" << endl;
		return -1.0;
	}
}
Sin* Sin::clone() {
	Sin* retNode = new Sin;
	for (int i = 0; i<numChildren; i++) {
		retNode->children[i] = this->children[i]->clone();
	}
	return retNode;
}
string Sin::getLabel() {
	return this->label;
}

//
//     Cos
//
Cos::Cos() {
	this->numChildren = 1;
	this->label = "cos";
}
double Cos::eval(int index) {
	if (this->children[0] != NULL) {
		double v0 = children[0]->eval(index);
		double res = 0;
		res = cos(v0);

		if (res < -1 * Inf || res > Inf) {
			res = Inf;
		}
		return res;
	}
	else {
		cerr << "left and right not defined in Cos" << endl;
		return -1.0;
	}
}
Cos* Cos::clone() {
	Cos* retNode = new Cos;
	for (int i = 0; i<numChildren; i++) {
		retNode->children[i] = this->children[i]->clone();
	}
	return retNode;
}
string Cos::getLabel() {
	return this->label;
}

//
//     Exp
//
Exp::Exp() {
	this->numChildren = 1;
	this->label = "exp";
}
double Exp::eval(int index) {
	if (this->children[0] != NULL) {
		double v0 = children[0]->eval(index);
		double res = 0;

		if (v0 > 10)
			res = exp(10);
		else
			res = exp(v0);

		if (res < -1 * Inf || res > Inf) {
			res = Inf;
		}
		return res;
	}
	else {
		cerr << "left and right not defined in Exp" << endl;
		return -1.0;
	}
}
Exp* Exp::clone() {
	Exp* retNode = new Exp;
	for (int i = 0; i<numChildren; i++) {
		retNode->children[i] = this->children[i]->clone();
	}
	return retNode;
}
string Exp::getLabel() {
	return this->label;
}

//
//     Ln
//
Ln::Ln() {
	this->numChildren = 1;
	this->label = "ln";
}
double Ln::eval(int index) {
	if (this->children[0] != NULL) {
		double v0 = children[0]->eval(index);
		double res = 0;

		if (abs(v0) < 1e-3) 
			res = log(1e-3);
		else
			res = log(abs(v0));

		if (res < -1 * Inf || res > Inf) {
			res = Inf;
		}
		return res;
	}
	else {
		cerr << "left and right not defined in Ln" << endl;
		return -1.0;
	}
}
Ln* Ln::clone() {
	Ln* retNode = new Ln;
	for (int i = 0; i<numChildren; i++) {
		retNode->children[i] = this->children[i]->clone();
	}
	return retNode;
}
string Ln::getLabel() {
	return this->label;
}