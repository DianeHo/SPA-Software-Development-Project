#include "ConvertPostfix.h"



// Function to check if the character is an operator
bool ConvertPostfix::  isOperator(char c)
{
	return (!isalpha(c) && !isdigit(c));
}

// Function to get the priority of operators
int ConvertPostfix:: getPriority(char C)
{
	if (C == '-' || C == '+')
		return 1;
	else if (C == '*' || C == '/')
		return 2;
	else if (C == '^')
		return 3;
	return 0;
}

string ConvertPostfix::infixToPostfix(string infix)
{
    int l = infix.size();
    stack<char> char_stack;
    string output;

    for (int i = 0; i < l; i++) {
        if (infix[i] == ' ') continue; // Skip spaces

        // If the scanned character is an operand or operator, add it to output
        if (isalpha(infix[i]) || isdigit(infix[i])) {
            while (isalpha(infix[i]) || isdigit(infix[i])) {
                output += infix[i++];
            }
            output += ' '; // Add space after operand
            i--; // Decrement i to handle the next character
        }
        else if (infix[i] == '(') {
            char_stack.push('(');
        }
        else if (infix[i] == ')') {
            while (!char_stack.empty() && char_stack.top() != '(') {
                output += char_stack.top();
                output += ' '; // Add space after operator
                char_stack.pop();
            }
            if (!char_stack.empty())
                char_stack.pop(); // Remove '(' from stack
        }
        else { // Operator found
            while (!char_stack.empty() && getPriority(infix[i]) <= getPriority(char_stack.top())) {
                output += char_stack.top();
                output += ' '; // Add space after operator
                char_stack.pop();
            }
            char_stack.push(infix[i]);
        }
    }

    // Add remaining operators from stack to output
    while (!char_stack.empty()) {
        output += char_stack.top();
        output += ' '; // Add space after operator
        char_stack.pop();
    }

    return output;
}



list<string> ConvertPostfix::stringtoList(string postfix) {
    string temp;
    list<string> postfixStack;
    for (auto c : postfix) {
        if (c == ' ' && !temp.empty()) {
            postfixStack.push_back(temp);
            temp.erase();
        }
        else {
            temp += c;
        }
    }
    return postfixStack;

}


bool ConvertPostfix::comparePostfix(bool leftEmpty, bool rightEmpty, string postfix1, string postfix2) {
    //cout << "leftEmpty: " << leftEmpty << endl;
    //cout << "rightEmpty: " << rightEmpty << endl;

    bool foundFirst = 0;
    int j = 1;
    list<string> s1 = stringtoList(postfix1);
    list<string> s2 = stringtoList(postfix2);

    if (!leftEmpty) {
        if (!s2.empty() && !s1.empty() && s1.front() != s2.front()) {
            return 0;
        }
    }

    while (!s1.empty() && !s2.empty()) {
        //cout << "s1:" << s1.front() << " ";
        //cout << "s2:" << s2.front() << " ";
        if (!foundFirst && s2.front()==s1.front()) { //for leftEmpty 
           foundFirst = 1;
           s2.pop_front();
        }

        else if (foundFirst) {
            if (s2.front() == s1.front()) {
                s2.pop_front();
                //cout << "the same!" << " ";
            }
            else {
                //cout << "not the same!" << endl;
                return 0;
            }
        }
        s1.pop_front();

    }

    if (!rightEmpty && !s1.empty() ) {
        return 0;
    }

    if (!s2.empty()) {
        return 0;
    }
    return 1;
}