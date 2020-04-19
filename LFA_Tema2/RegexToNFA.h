#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "Common.h"
#include "NFAGama.h"

// Traverse this tree and show all elements
void viewRegex(Element* tree, int order = 0);

// Transform THIS element into a DFA
NFAGama ElementToDFA(Element* group);

// Travels the current level of the given tree and performs <operation> between groups found
NFAGama RecursiveConvertToDFA(Element* tree);

// Process the tree and converts any "free floating" multiplication operations into Groups so that the Recursive parser can just operate on them without
// any operands-order checks. e.g.  it easier for the input to be represented like this a+(a*b) than a+a*b. The algorithm can just add 'a' with (a*b) and solve the second one recursively
Element* PostProcess(Element* parent);

// Parse the input into a tree of Elements (like xml parser do). It's much better to use Objects and be able to see in the future, have subgroups then plain strings
Element* ParseRegex(std::string txt);

// Checks if the operation to be performed is multiplication
bool isMultiplied(Element* prev, Element* current);

// Enters the bracket and returns the index in the string at which the closing and corresponding bracket was found
int enterParanthese(int index, std::string txt);