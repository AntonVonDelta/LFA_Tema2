#include <iostream>
#include <vector>
#include "Common.h"
#include "DFA.h"
#include "NFAGama.h"

using namespace std;

// Defined in RegexToNFA.cpp and declared here
void viewRegex(Element* tree, int order = 0);
NFAGama ElementToDFA(Element* group);
NFAGama RecursiveConvertToDFA(Element* group);
Element* PostProcess(Element* parent);
Element* ParseRegex(string txt);
bool isMultiplied(Element* prev, Element* current);
int enterParanthese(int index, string txt);


int main() {

	////////////////// DFA TO REGEX
	DFA M;

	ifstream fin("In.txt");
	fin >> M;
	fin.close();

	cout << M.toRegex();
	cout << endl << endl << endl;

	////////////////// REGEX TO DFA

	string regex = M.toRegex();//  "ab(a+b)*+c";// 
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

