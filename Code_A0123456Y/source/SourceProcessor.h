#pragma once
#include <sstream>
#include <fstream>
#include <vector>
#include "Database.h"
#include "Tokenizer.h"
#include <list>

using namespace std;

class SourceProcessor {
public:
	// method for processing the source program
	list<string>tTokens;
	string expr;

	void process(string program);
	void parse(list<string> tokens);
	void parseProcedure();
	void parseStatement();
	void parseVariable(string varCondition);
	void parseConstant(string value);
	void parseAssignment();
	void traceParent();
	void parseParent();
	string parseCondition(string ifwhile);


private:
	void next();
	bool validateName(string token);
	bool validateConstant(string token);
	bool validateSymbol(string token);
	void expect(string symbol);
	void countLine();
	bool match(string symbol);

};