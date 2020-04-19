#pragma once

#include <iostream>
#include <map>
#include <string>
#include <set>
#include <fstream>
#include<cmath>
#include<algorithm>
#include "Common.h"

using namespace std; 

class DFA {
	set<int> Q, F;
	set<char> Sigma;
	int q0;
	map<pair<int, char>, int> delta;
	map<pair<int, string>, int> regexDelta;

	void regex_removeState(int);

public:

	DFA() { this->q0 = 0; }
	DFA(set<int> Q, set<char> Sigma, map<pair<int, char>, int> delta, int q0, set<int> F) {
		this->Q = Q;
		this->Sigma = Sigma;
		this->delta = delta;
		this->q0 = q0;
		this->F = F;
	}

	set<int> getQ() const { return this->Q; }
	set<int> getF() const { return this->F; }
	set<char> getSigma() const { return this->Sigma; }
	int getInitialState() const { return this->q0; }
	map<pair<int, char>, int> getDelta() const { return this->delta; }

	// Makes this DFA to loop on itself
	// Equivalent to apllying * to this DFA
	void ApplyLoop();
	DFA ApplyOperation(DFA second, Element* operation);
	string toRegex();


	friend istream& operator >> (istream&, DFA&);

	bool isFinalState(int);
	int deltaStar(int, string);
};
