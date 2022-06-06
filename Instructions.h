#pragma once
#include "Operations.h"
#include<vector>
#include<set>
#include <iostream>
#include <algorithm>
#include <string>

class Instruction
{
public:
	int depth;
	int numNode;  //the number of nodes in the whole Instruction
	Op* root;
	string name;
	Instruction();
	~Instruction();
	Op* createRandomNode(int depth);
	Op* createRandomTree(int depth = 1);
	void printInstruction(Op*inInstruction = NULL);
	void getname(Op* inInstruction = NULL);
	Op** DFSvisit(int tsi, int* ci, Op** inInstruction = NULL);
	Op* getRanSubTree();  //return a deep copy of a random sub tree
	Op* getSubTree(int si);  //return a deep copy of a certain sub tree
	int countDep(Op*inInstruction = NULL, int maxdepth = 0, int depth = 1);
	int countNode(Op*inInstruction = NULL);
	int getOpDep(Op*tarOp, int depth = 1, Op*inInstruction = NULL);
	void getDepthSet(vector<Op*> &ds, int tarDep = 1, Op*inInstruction=NULL);
	double eval(int ind = 0);
	void mutateInstruction(Op** inInstruction = NULL, int depth = 1, Op** keyop = NULL);
	void crossOver(Instruction* donatingInstruction, Op** inInstruction = NULL, Op** keyop = NULL);  //get a sub tree from donating Instruction and substitute into original tree, update the depth
	void DE_mutate(Op* tarInTree, Op* r2InTree, Op** inTree = NULL, int depth = 1, Op** keyop = NULL);
	void deleteInstruction(Op* inInstruction = NULL);
	set<int> getReadReg(Op*inInstruction = NULL);
	set<int> getWriteReg(Op*inInstruction = NULL);
	Instruction & operator = (const Instruction&);   //deep clone between Instruction class
	bool operator == (const Instruction & A) { return name == A.name; };
};



Instruction::Instruction()
{
	root = createRandomTree();
	depth = countDep();
	numNode = countNode();
	getname();
}
Instruction::~Instruction()
{
	deleteInstruction();
}
Op* Instruction::createRandomTree(int depth) {
	Op* retFtn = createRandomNode(depth);

	for (int i = 0; i < retFtn->numChildren; i++) {
		retFtn->children[i] = createRandomTree(depth + 1);
	}
	return retFtn;
}
Op* Instruction::createRandomNode(int depth) {
	int randn = 0;
	int numFtnChoices = NUM_TERM;
	Op* retFtn = NULL;
	vector<int> available = { 0,1,2,/*3,*/4,5,6,7/*,8,9,10,11*/ };

	if (depth == 1)
		retFtn = (Op*) new WriteReg();
	else {
		if (depth < MaxDepth) numFtnChoices = available.size();  //if the depth is greater than MaxDepth, only allow const or inputs or ReadReg
		randn = available[rand() % numFtnChoices];

		switch (randn) {	//move random 
			case 0:
				retFtn = (Op*) new ConstNode();
				break;
			case 1:
				retFtn = (Op*) new InputNode();
				break;
			case 2:
				retFtn = (Op*) new ReadReg();
				break;
			case 3:
				retFtn = (Op*) new WriteReg();
				break;
			case 4:
				retFtn = (Op*) new Add();
				break;
			case 5:
				retFtn = (Op*) new Subtract();
				break;
			case 6:
				retFtn = (Op*) new Multiply();
				break;
			case 7:
				retFtn = (Op*) new Divide();
				break;
			case 8:
				retFtn = (Op*) new Sin();
				break;
			case 9:
				retFtn = (Op*) new Cos();
				break;
			case 10:
				retFtn = (Op*) new Exp();
				break;
			case 11:
				retFtn = (Op*) new Ln();
				break;
			default:
				cerr << "invalid random number, bad news\n\n\n";
				break;
		}
	}


	return retFtn;
}
void Instruction::getname(Op*inTree) {
	if (inTree == NULL) {
		inTree = root;
		name = "";
	}
		
	//inorder visit
	if (inTree->numChildren == 2)
	{
		name += "(";
		getname(inTree->children[0]);
		name += inTree->getLabel();
		getname(inTree->children[1]);
		name += ")";
	}
	else if (inTree->numChildren == 1)
	{
		name += inTree->getLabel();
		name += "(";
		getname(inTree->children[0]);
		name += ")";
	}
	else if (inTree->numChildren == 0)
	{
		name += inTree->getLabel();
	}
	return;
}
void Instruction::printInstruction(Op*inTree)
{
	if (inTree == NULL)
		inTree = root;
	//inorder visit
	if (inTree->numChildren == 2)
	{
		printf("(");
		printInstruction(inTree->children[0]);
		printf("%s", inTree->getLabel().c_str());
		printInstruction(inTree->children[1]);
		printf(")");
	}
	else if (inTree->numChildren == 1)
	{
		printf("%s", inTree->getLabel().c_str());
		printf("(");
		printInstruction(inTree->children[0]);
		printf(")");
	}
	else if (inTree->numChildren == 0)
	{
		printf("%s", inTree->getLabel().c_str());
	}
	if (inTree == root) printf("\n");
	return;
}
void Instruction::deleteInstruction(Op*inTree)
{
	if (inTree == NULL)
	{
		inTree = root;
	}
	for (int i = 0; i<inTree->numChildren; i++) {  //free any children first
		deleteInstruction(inTree->children[i]);
	}
	delete inTree;
}
Op** Instruction::DFSvisit(int tsi, int* ci, Op**inTree)
{
	if (tsi > numNode - 1) {
		printf("the tsi is larger than the node number\n");
		return NULL;
	}
	if (inTree == NULL)
	{
		inTree = &root;
	}
	if (*ci == tsi)
		return inTree;
	else
	{
		Op** tmpTree;
		for (int i = 0; i < (*inTree)->numChildren; i++)
		{
			if ((*inTree)->children[i] == NULL)
				printf("the children is NULL\n");
			*ci = *ci + 1;
			tmpTree = DFSvisit(tsi, ci, &(*inTree)->children[i]);
			if (tmpTree)
			{
				return tmpTree;
			}

		}
	}
	return NULL;
}
Op* Instruction::getSubTree(int si)
{
	//si: the visiting order (index) of the root of the sub tree

	//find the subtree
	int cnt = 0;
	Op** subRoot = DFSvisit(si, &cnt);
	if (cnt < si)
	{
		printf("cnt %d <si %d, numNode: %d\n", cnt, si, numNode);
		printf("root: %s\n", root->getLabel().c_str());
	}
	//clone
	return (*subRoot)->clone();
}
Op* Instruction::getRanSubTree()
{
	int si = rand() % numNode;
	return getSubTree(si);
}
int Instruction::countNode(Op*inTree)
{
	if (inTree == NULL)
	{
		inTree = root;
	}
	if (inTree->numChildren == 0)
		return 1;
	else
	{
		int cnt = 0;
		for (int i = 0; i < inTree->numChildren; i++)
		{
			cnt += countNode(inTree->children[i]);
		}
		return 1 + cnt;
	}
}
int Instruction::countDep(Op*inTree, int maxdepth, int depth)
{
	if (inTree == NULL)
	{
		inTree = root;
	}
	if (depth > maxdepth)
		maxdepth = depth;
	for (int i = 0; i < inTree->numChildren; i++)
	{
		maxdepth = countDep(inTree->children[i], maxdepth, depth + 1);
	}
	return maxdepth;
}
int Instruction::getOpDep(Op*tarOp, int depth, Op*inTree)
{
	//get the depth of a certain node
	int tmpdep = -1;
	if (inTree == NULL) {
		inTree = root;
	}
	if (tarOp == inTree) {
		return depth;
	}
	else {
		for (int i = 0; i < inTree->numChildren; i++) {
			tmpdep = getOpDep(tarOp, depth + 1, inTree->children[i]);
			if (tmpdep > 0)
				return tmpdep;
		}
	}
	return -1;
}
void Instruction::getDepthSet(vector<Op*> &ds, int tarDep, Op* inTree)
{
	//get the nodes whose sub tree's depth is less than or equal to tarDep
	if (inTree == NULL) {
		inTree = root;
	}
	if (countDep(inTree) <= tarDep) {
		ds.push_back(inTree);
	}
	for (int i = 0; i < inTree->numChildren; i++) {
		getDepthSet(ds, tarDep, inTree->children[i]);
	}

}
double Instruction::eval(int ind)
{
	return root->eval(ind);
}
void Instruction::mutateInstruction(Op** inTree, int depth, Op** keyop) {
	if (inTree == NULL)
	{
		inTree = &root;
		int cnt = 0;
		keyop = DFSvisit(rand() % numNode, &cnt);
	}

	if (rand() / (double)RAND_MAX < MUTATION_THRESH || inTree == keyop) {
		//create new node
		Op* newNode = createRandomNode(depth);//create a random node to replace current node
		int childrenToMove = min(newNode->numChildren, (*inTree)->numChildren);
		for (int i = 0; i<childrenToMove; i++) {
			newNode->children[i] = (*inTree)->children[i];
		}
		int numNewChildren = newNode->numChildren - childrenToMove;
		if (numNewChildren > 0)
		{
			for (int i = childrenToMove; i<(childrenToMove + numNewChildren); i++) {
				newNode->children[i] = createRandomTree(depth + 1);
			}
		}
		else //clear the memory the subtree of inTree
		{
			for (int i = childrenToMove; i<(*inTree)->numChildren; i++) {
				deleteInstruction((*inTree)->children[i]);
			}
		}
		if (inTree == keyop) keyop = NULL;
		delete *inTree;		//free memory held by old node
		*inTree = newNode;	//replace old ptr with ptr to new
		this->depth = countDep();
		this->numNode = countNode();
		getname();
	}
	for (int i = 0; i<(*inTree)->numChildren; i++) {
		mutateInstruction(&(*inTree)->children[i], depth + 1, keyop);
	}
}
void Instruction::crossOver(Instruction* donateInstr, Op** inTree, Op** keyop)
{
	if (inTree == NULL)
	{
		inTree = &(root->children[0]);
		int cnt = 0;
		keyop = DFSvisit(1 + rand() % (numNode - 1), &cnt);
	}

	if (rand() / (double)RAND_MAX < CROSSOVER_THRESH || inTree == keyop) {
		//get a random sub tree from donateTree
		vector<Op*> ds;
		ds.reserve(numNode);
		donateInstr->getDepthSet(ds, MaxDepth - this->getOpDep(*inTree) + 1);
		Op* si = ds[rand() % ds.size()];
		Op* newSubTree = si->clone();

		if (inTree == keyop) keyop = NULL;
		deleteInstruction(*inTree);
		*inTree = newSubTree;

		depth = countDep();
		numNode = countNode();
		getname();
	}
	else {
		for (int i = 0; i < (*inTree)->numChildren; i++)
		{
			crossOver(donateInstr, &((*inTree)->children[i]), keyop);
		}
	}
}
void Instruction::DE_mutate(Op* tarInTree, Op* r2InTree, Op** inTree, int depth, Op** keyop) {
	//tarInTree: the best individual, r2InTree: another individual

	if (inTree == NULL)
	{
		inTree = &root;
		int cnt = 0;
		keyop = DFSvisit(rand() % numNode, &cnt);
	}

	//DE part
	double F = randval();
	double CR = randval();
	double dd1 = 0, dd2 = 0;

	if (randval() < CR || inTree == keyop) {
		if (tarInTree != NULL && tarInTree->label != (*inTree)->label) dd1 = 1;
		if (r2InTree != NULL && r2InTree->label != (*inTree)->label) dd2 = 1;
		double c_vector = F*dd1 + F*dd2 - F*dd1*F*dd2;

		if (randval() < c_vector || randval() < 0.03) {
			//create new node
			Op* newNode = createRandomNode(depth);//create a random node to replace current node
			int childrenToMove = min(newNode->numChildren, (*inTree)->numChildren);
			for (int i = 0; i<childrenToMove; i++) {
				newNode->children[i] = (*inTree)->children[i];
			}
			int numNewChildren = newNode->numChildren - childrenToMove;
			if (numNewChildren > 0)
			{
				for (int i = childrenToMove; i<(childrenToMove + numNewChildren); i++) {
					newNode->children[i] = createRandomTree(depth + 1);
				}
			}
			else //clear the memory the subtree of inTree
			{
				for (int i = childrenToMove; i<(*inTree)->numChildren; i++) {
					deleteInstruction((*inTree)->children[i]);
				}
			}
			if (inTree == keyop) keyop = NULL;
			delete *inTree;		//free memory held by old node
			*inTree = newNode;	//replace old ptr with ptr to new
			this->depth = countDep();
			this->numNode = countNode();
			getname();
		}
	}

	Op *tarChild = NULL, *r2Child = NULL;
	for (int i = 0; i<(*inTree)->numChildren; i++) {
		tarChild = NULL, r2Child = NULL;
		if (tarInTree != NULL && i < tarInTree->numChildren) {
			tarChild = tarInTree->children[i];
		}
		if (r2InTree != NULL && i < r2InTree->numChildren) {
			r2Child = r2InTree->children[i];
		}
		DE_mutate(tarChild, r2Child, &(*inTree)->children[i], depth + 1, keyop);
	}
}
Instruction& Instruction::operator=(const Instruction& t)
{
	if (t.root == this->root)
		return *this;
	if (this->root != NULL)
		deleteInstruction(this->root);
	this->depth = t.depth;
	this->numNode = t.numNode;
	this->name = t.name;
	this->root = t.root->clone();
	return *this;
}

set<int> Instruction::getReadReg(Op* inTree) {
	if (inTree == NULL) {
		inTree = root;
	}
	set<int> res;
	if (typeid(*inTree) == typeid(ReadReg)) {
		res.insert(((ReadReg*)inTree)->regIndex);
	}
	set<int> tmp;
	for (int i = 0; i < inTree->numChildren; i++) {
		tmp = getReadReg(inTree->children[i]);
		res.insert(tmp.begin(), tmp.end());
	}
	return res;
}

set<int> Instruction::getWriteReg(Op* inTree) {
	if (inTree == NULL) {
		inTree = root;
	}
	set<int> res;
	if (typeid(*inTree) == typeid(WriteReg)) {
		res.insert(((WriteReg*)inTree)->regIndex);
	}
	set<int> tmp;
	for (int i = 0; i < inTree->numChildren; i++) {
		tmp = getWriteReg(inTree->children[i]);
		res.insert(tmp.begin(), tmp.end());
	}
	return res;
}

//
//void tourSel(Tree*tar, Tree*pop, Tree*ofsp)
//{
//	int i = rand() % POP_SIZE;
//	Tree selTree;
//	selTree = pop[i];
//	for (int j = 1; j < TOU_SIZE; j++)
//	{
//		int ii = rand() % POPSIZE;
//		if ((double)rand() / RAND_MAX < 0.5 && ii != i)
//		{
//			if (pop[ii].depth <= MAX_TREE_DEPTH && pop[ii].perfScore < selTree.perfScore)
//			{
//				selTree = pop[ii];
//			}
//		}
//		else
//		{
//			if (ofsp[ii].depth <= MAX_TREE_DEPTH && ofsp[ii].perfScore < selTree.perfScore)
//			{
//				selTree = ofsp[ii];
//			}
//		}
//	}
//	*tar = selTree;
//}
