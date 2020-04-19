#include "NFAGama.h"
#include <queue>

bool NFAGama::isFinalState(set<int> q) {
	for (auto f : F) {
		for (auto state : q) {
			if (f == state) return true;
		}
	}
	return false;
}

set<int> NFAGama::deltaStar(set<int> q, string w) {
	q= getGammaClosure(q);
	if (w.length() == 0) return q;

	set<int> res = {};

	for (auto state : q) { 
		set<int> new_states = delta[{state, (char)w[0]}];

		res.insert(new_states.begin(), new_states.end());
	} 

	res = getGammaClosure(res);
	if (w.length() == 1) {
		return res;
	}

	return getGammaClosure(deltaStar(res, w.substr(1, w.length() - 1)));
}

istream& operator >> (istream& f, NFAGama& M) {
	int noOfStates;
	f >> noOfStates;
	for (int i = 0; i < noOfStates; ++i) {
		int q;
		f >> q;
		M.Q.insert(q);
	}

	int noOfLetters;
	f >> noOfLetters;
	for (int i = 0; i < noOfLetters; ++i) {
		char ch;
		f >> ch;
		M.Sigma.insert(ch);
	}

	int noOfTransitions;
	f >> noOfTransitions;
	for (int i = 0; i < noOfTransitions; ++i) {
		int s, d;
		char ch;
		f >> s >> ch >> d;
		M.delta[{s, ch}].insert(d);
	}

	f >> M.q0;

	int noOfFinalStates;
	f >> noOfFinalStates;
	for (int i = 0; i < noOfFinalStates; ++i) {
		int q;
		f >> q;
		M.F.insert(q);
	}

	return f;
}


void NFAGama::ApplyLoop() {
	delta[{*(F.begin()), '.'}].insert(q0);
	delta[{q0, '.'}].insert(*F.begin());
}
NFAGama NFAGama::ApplyOperation(NFAGama second, Element* operation) {
	int next_state = 1;
	for (auto state : Q) {
		next_state = max(next_state, state);
	}
	next_state++;

	int first_state = q0;
	for (auto& [transition, outputs] : second.delta) {
		for (auto output : outputs) {
			delta[{transition.first + next_state, transition.second}].insert(output + next_state);

			// Make this DFA complete
			Q.insert(output + next_state);
		}
		
		Q.insert(transition.first + next_state);
		Sigma.insert(transition.second);
	}

	// Addition
	if (operation->type == 2) {
		delta[{q0, '.'}].insert(second.q0 + next_state);
		delta[{*(second.F.begin()) + next_state, '.'}].insert( *F.begin());
	}

	// Multiplication
	if (operation->type == 4) {
		delta[{*F.begin(), '.'}].insert(second.q0 + next_state);

		// Add a new state to buffer the output(meaning any lops on the output state t=do not travel back in this NFA. This is done with a lambda buffer/diode)
		delta[{*(second.F.begin()) + next_state, '.'}].insert(*(second.F.begin()) + next_state + 1);
		Q.insert(*(second.F.begin()) + next_state + 1);

		F = { *(second.F.begin()) + next_state+1 };
	}

	return *this;
}

set<int> NFAGama::getGammaClosure(set<int> states) {
	set<int> temp = {};
	queue<int> list;

	for (auto state : states) {
		list.push(state);
	}
	
	bool found = true;
	while (!list.empty()) {
		int current_state = list.front();
		list.pop();

		found = false;

		temp.insert(current_state);

		for (auto gamma_state : delta[{current_state, '.'}]) {
			if (temp.find(gamma_state) == temp.end()) {
				// State not in list
				found = true;
				list.push(gamma_state);
			}
		}
	}

	return temp;
}
