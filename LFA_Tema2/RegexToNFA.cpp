#include "RegexToNFA.h"

using namespace std;

// Traverse this tree and show all elements
void viewRegex(Element* tree, int order) {
	cout << string(order, ' ') << "{" << endl;
	Element* it = tree;

	while (it != nullptr) {
		cout << string(order + 2, ' ') << it->type << " : " << it->content << "  " << (it->node_loop ? "*" : "") << endl;
		if (it->subgroup != nullptr) {
			viewRegex(it->subgroup, order + 2);
		}
		it = it->next;
	}

	cout << string(order, ' ') << "}" << endl;
}

// Transform THIS element into a DFA
NFAGama ElementToNFA(Element* group) {
	if (group == nullptr) throw 10;
	if (group->type == 1) {
		// This means this group is actually just one character. We can easily convert it to a NFA
		if (group->node_loop) {

			// Special case if this caracter is looping on itself e.g a*
			return NFAGama({ 0,1 }, { group->content[0] }, { {{0,group->content[0] },{1}}, {{0,'.' },{1}}, {{1,'.' },{0}} }, 0, { 1 });
		}
		return NFAGama({ 0,1 }, { group->content[0] }, { {{0,group->content[0] },{1}} }, 0, { 1 });
	}
	if (group->type == 0) {
		// The given group contains subgroups. Convert it recursively

		if (group->subgroup == nullptr) throw 11;
		NFAGama result = RecursiveConvertToNFA(group->subgroup);

		// Special care if the group is looping on itself
		if (group->node_loop)
			result.ApplyLoop();
		return result;
	}
	throw 12;		// Should not get here...we throw an error to catch any bug in the logic of this convertor
}

// Travels the current level of the given tree and performs <operation> between groups found
NFAGama RecursiveConvertToNFA(Element* group) {
	// The logic here: we get the first group and read until we hit an operator. Peek the next group and convert it to NFA
	// Apply that operation between the previous NFA and the next one. The net result is that we operate on two groups at a time
	// We are assured that the multiplication is configned to individual groups so reading `a+ab` will not add `a+a` and multiply `b`
	//		we actually get this a+(ab) which is processed as a+ <recursive: a*b>
	if (group == nullptr) throw 10;

	NFAGama result;
	Element* it = group;
	NFAGama prev = ElementToNFA(it);

	while (it != nullptr) {
		if (it->type == 2 || it->type == 4) {
			prev = prev.ApplyOperation(ElementToNFA(it->next), it);
		}
		it = it->next;
	}
	return prev;
}

// Process the tree and converts any "free floating" multiplication operations into Groups so that the Recursive parser can just operate on them without
// any operands-order checks. e.g.  it easier for the input to be represented like this a+(a*b) than a+a*b. The algorithm can just add 'a' with the group (a*b) and solve the second one recursively
Element* PostProcess(Element* parent) {
	// Analyzes the raw elements and separates the multiplication from addition by grouping all multiplciation-operated elements into subgroups in the tree
	//			i.e a+ab becomes a+(ab)

	// We need this start Element altought it will not be returned (a.k.a it will be cut from the output)
	Element* telomere = new Element;
	*telomere = { -1,string(""),false,parent,nullptr };

	parent = telomere;

	Element* it = parent->next;
	Element* prev_it = parent;

	bool started_group = false;
	Element* replacement = new Element;

	*replacement = { 0, string(""), false, nullptr, nullptr };

	while (it != nullptr) {
		// Recursive. Do this post processing for al subgroups
		if (it->subgroup != nullptr) it->subgroup = PostProcess(it->subgroup);

		// Here a new group CAN be started because we detected multilplication
		// The conditions are: the current element is a group or a character and the next element is the multiplication operator
		if ((it->type == 0 || it->type == 1) && (it->next != nullptr && it->next->type == 4)) {
			replacement->content += (it->type == 1 ? it->content : string("(") + (it->content) + string(")"));

			if (!started_group) {
				replacement->subgroup = it;

				prev_it->next = replacement;
				started_group = true;
			}
		}
		if (started_group) {
			// Here a group CAN be ended
			if ((it->type == 0 || it->type == 1) && (it->next == nullptr || it->next->type != 4)) {
				replacement->content += (it->type == 1 ? it->content : string("(") + (it->content) + string(")"));
				replacement->next = it->next;

				it->next = nullptr;
				started_group = false;
			}
		}

		prev_it = it;
		it = it->next;
	}

	Element* res = telomere->next;
	delete telomere;
	return res;
}

// Parse the input into a tree of Elements (like xml parser do). It's much better to use Objects and be able to see in the future, have subgroups then plain strings
Element* ParseRegex(string txt) {
	// All elements are represented either as a group [e.g input= a+(ab) then a+b is a group] or as an character. 
	// Also this parser analyzes and stores the operands(multiplication,loops and addition)

	// One problem arises: in the case a+ab the multiplication should be done first. To ease the algorithm, all elements that are multiplied and are not already in a group,
	//	are added to one. All this is not done here but in PostProcess().
	int i = 0;
	Element* first = new Element;
	Element* parent = first;
	*first = { -1,string(""),false,nullptr,nullptr };

	while (i != txt.size()) {
		if (txt[i] == '+') {
			first->next = new Element;
			*(first->next) = { 2,string("+"),false,nullptr,nullptr };
			first = first->next;

		} else if (txt[i] == '*') {
			first->node_loop = true;

		} else if (txt[i] == '(') {
			int closing = enterParanthese(i, txt);
			string group_text(txt.substr(i + 1, closing - i - 1));
			i = closing + 1;

			Element* current = new Element;
			*current = { 0,group_text,false,nullptr,nullptr };

			if (isMultiplied(first, current)) {
				first->next = new Element;
				*(first->next) = { 4,string("x"),false,nullptr,nullptr };
				first = first->next;
			}

			first->next = current;
			first = first->next;
			first->subgroup = ParseRegex(group_text);
			continue;

		} else {
			Element* current = new Element;
			*current = { 1,string(1,txt[i]),false,nullptr,nullptr };

			if (isMultiplied(first, current)) {
				first->next = new Element;
				*(first->next) = { 4,string("x"),false,nullptr,nullptr };
				first = first->next;
			}

			first->next = current;
			first = first->next;
		}

		i++;
	}

	Element* res = parent->next;
	delete parent;
	return res;
}

// Checks if the operation to be performed is multiplication
bool isMultiplied(Element* prev, Element* current) {
	if (prev->type == 0 || prev->type == 1) return true;
	return false;
}

// Enters the bracket and returns the index in the string at which the closing and corresponding bracket was found
int enterParanthese(int index, string txt) {
	int count = 0;
	int i = -1;

	for (i = index; i < txt.size(); i++) {
		if (txt[i] == '(') count++;
		if (txt[i] == ')') count--;

		if (count == 0) break;
	}
	return i;
}