#pragma once
#include<string>

struct Element {
	int type;	// 0 = bracket group, 1 = character, 2 = +, 4 = x multiplication
	std::string content;
	bool node_loop;
	Element* next;
	Element* subgroup;
};