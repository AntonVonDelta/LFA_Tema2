#include "DFA.h"
#include <vector>

string beautify(string str);

bool DFA::isFinalState(int q) {
	return F.find(q) != F.end();
}

int DFA::deltaStar(int q, string w) {
	if (w.length() == 1) {
		return delta[{q, (char)w[0]}];
	}

	int new_q = delta[{q, (char)w[0]}];
	return deltaStar(new_q, w.substr(1, w.length() - 1));
}

istream& operator >> (istream& f, DFA& M) {
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
		M.delta[{s, ch}] = d;
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

string DFA::toRegex() {
	// Preprocess the DFA:
	//    If there are input lines in Q0 create another starting node
	//    If there are output lines out of Qf create another final node
	//    But we don't care......Just add Qi and Qf
	Sigma.insert('.');

	// Create new start state
	int new_state =*Q.crbegin()+1;
	Q.insert(new_state);
	delta[{new_state,'.'}]=q0;
	q0 = new_state;

	// Create new final state
	new_state = *Q.crbegin() + 1;
	Q.insert(new_state);

	for (auto const& final_state : F) {
		delta[{final_state, '.'}] = new_state;
	}

	F.clear();
	F.insert(new_state);


	// Prepare regexDelta
	for (auto const& [transition, output] : delta) {
		regexDelta[{transition.first,string(1,transition.second)}]=output;
	}

	// The actual driving force for the algorithm
	for (auto const& state : Q) {
		if (state != q0 && state != *F.begin()) {
			regex_removeState(state);
		}
	}

	// Beautify some of the output while constructing it
	string res("");
	for (auto const& [transition, output] : regexDelta) {
		if (transition.first == q0 && output == *F.begin()) {
			res+=beautify(transition.second)+"+";
		}
	}

	return res.size() == 0 ? "(empty)" : res.substr(0,res.size()-1);
}


void DFA::regex_removeState(int state) {
	string loop_transitions("");
	bool found_loop_transitions = false;
	map<int, string> all_input_transition;
	map<int, string> all_output_transition;

	// The end format after the state is deleted is : <input regex>*<loop regex>*<output regex> 
	// The input and output regex is constructed for each input state and output state. 
	//If there one state there are more than one transitions into the current state, they are added together

	// Get all loop-states : that go into itself
	for (auto const& [transition, output] : regexDelta) {
		if (transition.first == state && output == state) {
			loop_transitions += transition.second+"+";
			found_loop_transitions = true;
		}
	} 

	if(found_loop_transitions) loop_transitions=beautify(loop_transitions.substr(0, loop_transitions.size() - 1))+"*";

	// Get input states.
	for (auto const& [transition, output] : regexDelta) {
		if (output == state && transition.first!=state) {
			// If there are more than one transitions then they are added
			all_input_transition[transition.first] += transition.second + "+";
		}
	}

	// Get output states
	for (auto const& [transition, output] : regexDelta) {
		if (transition.first == state && output!=state) {
			// If there are more than one transitions then they are added
			all_output_transition[output] += transition.second + "+";
		}
	}

	// Combine the input regex with the output regex for each input and output states.
	for (auto const& [input_state, input_regex] : all_input_transition) {
		for (auto const& [output_state, output_regex] : all_output_transition) {
			string input = beautify(input_regex.substr(0, input_regex.size()-1));
			string output = beautify( output_regex.substr(0, output_regex.size() - 1));

			// Combined in the final regex as a multiplication
			regexDelta[{input_state,input + loop_transitions + output}] = output_state;
		}
	}

	// Remove this state
	// This is simply done by deleting or transition in and out of this state
	auto itr = regexDelta.begin();
	while (itr != regexDelta.end()) {
		pair<int, string> transition = (*itr).first;
		int output = (*itr).second;

		if ( transition.first==state || output==state ) {
			// The current iterator is invalidated but erase returns a new valid one. 
			// Concerning the end() iterator it is always evaluated so it is always 'correct'
			itr = regexDelta.erase(itr);
		} else {
			++itr;
		}
	}
}

string beautify(string str) {
	if (str.size() > 1) return "(" + str + ")";
	return str;
}