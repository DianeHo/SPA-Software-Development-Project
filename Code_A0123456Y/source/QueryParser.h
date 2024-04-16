#pragma once
#include <sstream>
#include <fstream>
#include<list>
#include <map>
#include <stdexcept>

#include "Query.h"

using namespace std;

class QueryParser
{
public:
	list<string> remainingTokens;
	map<string, string> currentDeclarationList;

	void parse(list<string> tokens, Query& query);
	void parseDeclarationList(Query& currentQuery);
	void parseSelectClause(Query& currentQuery);
	void parseSuchThatClause(Query& currentQuery);
	void parsePatternClause(Query& currentQuery);
	
	bool validatePartialPattern(string argument, vector<string>& argumentPair);
	bool validateArgumentType(string argument, vector<string>& argumentPair, bool lineNumber);
	bool validateSynonym(string symbol, bool checkInDeclaration);
	bool validateDesignEntity(string symbol);
	bool validateIdent(string symbol);
	bool validateNumber(string text);

private:
	void expect(string symbol);
	bool match(string symbol);
	void next();
};

