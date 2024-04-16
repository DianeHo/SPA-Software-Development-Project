#pragma once

#include <list>
#include <utility>
#include "Database.h"
#include "Tokenizer.h"
#include "ConvertPostfix.h"
#include "QueryParser.h"
#include "ResultStorage.h"


using namespace std;

class QueryProcessor {
public:
	// default constructor
	QueryProcessor();

	// destructor
	~QueryProcessor();

	// method for evaluating a query
	void evaluate(Query query, vector<string>& results);
	void filterSuchThatPatternClause(Query queryObj, ResultStorage& rs, vector<string>& output);
	void filterOnlySuchThatClause(Query queryObj, ResultStorage& rs, vector<string>& output);
	void filterOnlyPatternClause(Query queryObj, ResultStorage& rs, vector<string>& output);
	void filterOnlySelectClause(ResultStorage& rs, vector<string>& output);

	void evaluateSelectClause(Query query, ResultStorage& rs);
	void evaluatePatternClause(Query query, ResultStorage& rs);
	void evaluateParentClause(Query query, ResultStorage& rs);
	void evaluateParentTClause(Query query, ResultStorage& rs);

	void evaluateCallsClause(Query query, ResultStorage& rs);
	void evaluateCallsTClause(Query query, ResultStorage& rs);
	void evaluateModifiesClause(Query query, ResultStorage& rs);
	void evaluateUsesClause(Query query, ResultStorage& rs);

	string getProcedureNameFromStmtLine(string stmtLine);
	void findDuplicates(const vector<string>& vec, vector<string>& duplicates);
	bool isOverlapped(Query queryObj, int& clause1Arg, int& clause2Arg);
	bool isStPtOverlapped(Query queryObj, int& stArg, int& ptArg);
};