#include "StructGraph.h"
#include <sstream>


// Add edge from vertex u to vertex v
void Graph::addEdge(char u, char v) {
    adjList[u].push_back(v);
}

// Function to check if there's a path from start to end
bool Graph::hasPath(char start, char end) {
    unordered_map<char, bool> visited;
    stack<char> stk;
    stk.push(start);
    while (!stk.empty()) {
        char current = stk.top();
        stk.pop();
        if (current == end) {
            return true;
        }
        if (!visited[current]) {
            visited[current] = true;
            for (char neighbor : adjList[current]) {
                stk.push(neighbor);
            }
        }
    }
    return false;
}

// Function to build graph from postfix expression
void Graph::buildGraphFromPostfix(const string& postfix) {
    stack<char> s;
    istringstream iss(postfix);
    string token;
    while (iss >> token) {
        if (isalpha(token[0]) || isdigit(token[0])) { // Operand, add to stack
            s.push(token[0]);
        }
        else if (token == "+" || token == "-" || token == "*" || token == "/") { // Operator
            char op2 = s.top(); s.pop();
            char op1 = s.top(); s.pop();
            addEdge(op1, token[0]);
            addEdge(op2, token[0]);
            s.push(token[0]);
        }
    }
}

// Function to compare two postfix expressions
bool comparePostfix(Graph& graph, const string& postfix1, const string& postfix2) {
    stack<char> s;
    istringstream iss(postfix1);
    string token;
    while (iss >> token) {
        if (isalpha(token[0]) || isdigit(token[0])) { // Operand
            s.push(token[0]);
        }
        else if (token == "+" || token == "-" || token == "*" || token == "/") { // Operator
            char op2 = s.top(); s.pop();
            char op1 = s.top(); s.pop();
            // Check if there's a path from op1 to op2 in the graph
            if (!graph.hasPath(op1, op2)) {
                return false;
            }
            s.push(token[0]);
        }
    }
    // Check if the second postfix expression can traverse the graph
    istringstream iss2(postfix2);
    while (iss2 >> token) {
        if (isalpha(token[0]) || isdigit(token[0])) { // Operand
            if (s.empty() || s.top() != token[0]) {
                return false;
            }
            s.pop();
        }
    }
    return s.empty();
}


