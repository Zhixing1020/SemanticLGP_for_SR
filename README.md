# SemanticLGP_for_SR
the source code of the CYB paper "Semantic Linear Genetic Programming for Symbolic Regression"

To apply the source code, you should:
1) change the file path in Data.h/readTrainData(..) and Data.h/readTestData(..). The data of every independent run is read based on the indices of problems (i.e., run) and run indices (i.e., job). "F12_0_training_data.txt" and "F12_0_testing_data.txt" are two example training and test data for the source code, in which "12" is the index of problem (i.e., Tower problem in the paper) and "0" is the index of independent run. 
2) the main.cpp accepts three parameters: problem index, run index, and the file address of outputting logs.
3) put all source files into one folder and compile these source files by the following commands:
      g++ main.cpp -o SLGP-ERC -std=c++11 -O3
      
4) SLGP-ERC.sh is an exmple Shell script for running the executable file SLGP-ERC.exe (SLGP-ERC.out for Linux).

On Window System, Visual Studio, you may need to set some PreProcessor parameters:  Project setting ——>C/C++——>Category——>PreProcessor——>PreProcessor Definitions: add "_CRT_SECURE_NO_WARNINGS" and "NOMINMAX"
