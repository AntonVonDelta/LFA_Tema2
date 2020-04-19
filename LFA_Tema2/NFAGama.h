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

class NFAGama {
	set<int> Q, F;
	set<char> Sigma;
	int q0;
	map<pair<int, char>, set<int>> delta;

public:
	NFAGama() { this->q0 = 0; }
	NFAGama(set<int> Q, set<char> Sigma, map<pair<int, char>, set<int>> delta, int q0, set<int> F) {
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
	map<pair<int, char>, set<int>> getDelta() const { return this->delta; }

	friend istream& operator >> (istream&, NFAGama&);


	// Makes this DFA to loop on itself
	// Equivalent to apllying * to this DFA
	void ApplyLoop();
	NFAGama ApplyOperation(NFAGama second, Element* operation);

	set<int> getGammaClosure(set<int>);

	bool isFinalState(set<int>);
	set<int> deltaStar(set<int>, string);
};

