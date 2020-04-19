#pragma once
#include <string>
#include <iostream>
#include <vector>

// A tree-struct holder for the data parsed from the text input
// This particular configuration allows us to have multiple subclasses
struct Element {
	int type;	// 0 = bracket group, 1 = character, 2 = +, 4 = x multiplication
	std::string content;
	bool node_loop;
	Element* next;
	Element* subgroup;
};