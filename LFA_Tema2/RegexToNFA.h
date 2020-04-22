#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "Common.h"
#include "NFAGama.h"

void viewRegex(Element* tree, int order = 0);

NFAGama ElementToNFA(Element* group);

NFAGama RecursiveConvertToNFA(Element* tree);

Element* PostProcess(Element* parent);

Element* ParseRegex(std::string txt);

bool isMultiplied(Element* prev, Element* current);

int enterParanthese(int index, std::string txt);