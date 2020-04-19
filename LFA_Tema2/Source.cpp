#include <iostream>
#include <vector>
#include "DFA.h"
#include "NFAGama.h"
#include "RegexToNFA.h"

using namespace std;


int main() {

	////////////////// DFA TO REGEX
	DFA M;

	ifstream fin("In.txt");
	fin >> M;
	fin.close();

	cout << M.toRegex();
	cout << endl << endl << endl;

	////////////////// REGEX TO DFA

	string regex = "a"; //M.toRegex();//  "ab(a+b)*+c";// 
	string test_words[] = { "a","aa","aba","abbbbbbbbbbbbbbba","abbaa","abbbcaaaa","ab","abababababababababa","c","ac" };
	cout << "Regex : " << regex << endl;

	Element* processed_data = PostProcess(ParseRegex(regex));
	viewRegex(processed_data);
	cout << endl<<endl;


	NFAGama nfa = RecursiveConvertToDFA(processed_data);

	/// Testing this NFA
	set<int> start = { nfa.getInitialState() };

	for (string text : test_words) {
		set<int> lastState = nfa.deltaStar(start, text);

		if (nfa.isFinalState(lastState)) {
			cout << "\""<<text<<"\" este acceptat";
		} else {
			cout << "\"" << text << "\" este respins";
		}

		cout << endl;
	}

	return 0;
}

