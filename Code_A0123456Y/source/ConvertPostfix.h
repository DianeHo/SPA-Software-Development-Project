#pragma once

#include <iostream>
#include <iostream>
#include <stack>
#include <list>

using namespace std;

class ConvertPostfix {

public:
	ConvertPostfix() {};
	~ConvertPostfix() {};

	static bool isOperator(char c);
	static int getPriority(char c);
	static string infixToPostfix (string infix);
	static list<string> stringtoList(string postfix);
	static bool comparePostfix(bool leftEmpty, bool rightEmpty, string postfix1, string postfix2);

private:

};