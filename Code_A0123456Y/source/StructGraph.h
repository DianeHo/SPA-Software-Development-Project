#ifndef STRUCTGRAPH_H
#define STRUCTGRAPH_H

#include <unordered_map>
#include <vector>
#include <string>
#include <stack>
#include <sstream>


using namespace std;

class Graph {
private:
    unordered_map<char, vector<char>> adjList; // adjacency list

public:
    // Add edge from vertex u to vertex v
    void addEdge(char u, char v);

    // Function to check if there's a path from start to end
    bool hasPath(char start, char end);

    // Function to build graph from postfix expression
    void buildGraphFromPostfix(const string& postfix);
};

// Function to compare two postfix expressions
bool comparePostfix(Graph& graph, const string& postfix1, const string& postfix2);

#endif // STRUCTGRAPH_H
