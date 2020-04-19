#include "RegexToNFA.h"

using namespace std;

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

NFAGama ElementToDFA(Element* group) {
	if (group == nullptr) throw 10;
	if (group->type == 1) {
		if (group->node_loop) {
			return NFAGama({ 0,1 }, { group->content[0] }, { {{0,group->content[0] },{1}}, {{0,'.' },{1}}, {{1,'.' },{0}} }, 0, { 1 });
		}
		return NFAGama({ 0,1 }, { group->content[0] }, { {{0,group->content[0] },{1}} }, 0, { 1 });
	}
	if (group->type == 0) {
		if (group->subgroup == nullptr) throw 11;
		NFAGama result = RecursiveConvertToDFA(group->subgroup);

		if (group->node_loop)
			result.ApplyLoop();
		return result;
	}
	throw 12;		// Should not get here
}
NFAGama RecursiveConvertToDFA(Element* group) {
	if (group == nullptr) throw 10;

	NFAGama result;
	Element* it = group;
	NFAGama prev = ElementToDFA(it);

	while (it != nullptr) {
		if (it->type == 2 || it->type == 4) {
			prev = prev.ApplyOperation(ElementToDFA(it->next), it);
		}
		it = it->next;
	}
	return prev;
}

Element* PostProcess(Element* parent) {
	Element* telomere = new Element;
	*telomere = { -1,string(""),false,parent,nullptr };

	parent = telomere;

	Element* it = parent->next;
	Element* prev_it = parent;

	bool started_group = false;
	Element* replacement = new Element;

	*replacement = { 0, string(""), false, nullptr, nullptr };

	while (it != nullptr) {
		// Recursive
		if (it->subgroup != nullptr) it->subgroup = PostProcess(it->subgroup);

		// Here a new group CAN be started
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
// Convert the text into clusters of objects
Element* ParseRegex(string txt) {
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

bool isMultiplied(Element* prev, Element* current) {
	if (prev->type == 0 || prev->type == 1) return true;
	return false;
}

// This is called when ( is met. It returns the last bracket index of this group
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