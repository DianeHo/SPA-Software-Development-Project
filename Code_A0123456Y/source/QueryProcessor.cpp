#include "QueryProcessor.h"
#include "Tokenizer.h"
#include "Database.h"
#include "SelectClauseEvaluator.h"
#include "ParentClauseEvaluator.h"
#include "ModifiesClauseEvaluator.h"
#include "UsesClauseEvaluator.h"
#include "CallsClauseEvaluator.h"
#include "PatternClauseEvaluator.h"
#include "ResultStorage.h"


// constructor
QueryProcessor::QueryProcessor() {}

// destructor
QueryProcessor::~QueryProcessor() {}

// method to evaluate a query
// This method currently only handles queries for getting all the procedure names,
// using some highly simplified logic.
// You should modify this method to complete the logic for handling all required queries.


void QueryProcessor::evaluate(Query queryObj, vector<string>& output) {
	// clear the output vector
	output.clear();

	ResultStorage rs;

	evaluateSelectClause(queryObj, rs);

	for (int i = 0 ; i < queryObj.suchThatClauses.size() ; i++) 
	{
		// if rel ref == parent
		// else if rel ref = modifies.. etc etc
		string relRef = queryObj.suchThatClauses[0].relRef;

		if (relRef == "Parent") {
			evaluateParentClause(queryObj, rs);
		}
		else if (relRef == "Parent*") {
			evaluateParentTClause(queryObj, rs);
		}
		else if (relRef == "Modifies") {
			evaluateModifiesClause(queryObj, rs);
		}
		else if (relRef == "Uses") {
			evaluateUsesClause(queryObj, rs);
		}
		else if (relRef == "Calls") {
			evaluateCallsClause(queryObj, rs);
		}
		else if (relRef == "Calls*") {
			evaluateCallsTClause(queryObj, rs);
		}
		cout << "Print Such That Result: " << endl;
		rs.printSuchThatResults();
	}

	for (int i = 0; i < queryObj.patternClauses.size(); i++) {
		evaluatePatternClause(queryObj, rs);
	}
	cout << "Print Pattern Result: " << endl;
	rs.printPatternResults();
	
	if ((rs.suchThatResults.empty() && !(queryObj.suchThatClauses.empty())) || (rs.patternResults.empty() && !(queryObj.patternClauses.empty()))) { //meaningless queries or pattern or such that supposed to return stuff but nothing 
		return; // returns nothing. Takes into account meaningless queries for 1. queries with suchThat OR pattern, 2. queries with suchThat AND pattern
	}
	else if (queryObj.suchThatClauses.empty() && queryObj.patternClauses.empty()) { // only select clause exists
		filterOnlySelectClause(rs, output);
	}
	else if (queryObj.suchThatClauses.empty() && !queryObj.patternClauses.empty()) { //only Pattern
		filterOnlyPatternClause(queryObj, rs, output);
	} 
	else if (!queryObj.suchThatClauses.empty() && queryObj.patternClauses.empty()) { //only SuchThat
		filterOnlySuchThatClause(queryObj, rs, output);
	}
	else if (!queryObj.suchThatClauses.empty() && !queryObj.patternClauses.empty()) { //both Pattern and SuchThat
		filterSuchThatPatternClause(queryObj, rs, output);
	}
	return;
}


void QueryProcessor::evaluateSelectClause(Query query, ResultStorage& rs)
{
	SelectClauseEvaluator selectClauseEvaluator;
	for (SelectClause select : query.selectClauses) {
		vector<string>results;
		selectClauseEvaluator.evaluate(results, select.designEntity);
		rs.addSelectResults(results);
	}
	cout << "Print Select Result: " << endl;
	rs.printSelectResults();
}

void QueryProcessor::evaluateParentClause(Query query, ResultStorage& rs)
{
	ParentClauseEvaluator parentClauseEvaluator;
	for (SuchThatClause suchThat : query.suchThatClauses) {
		vector <vector<string>>results;
		parentClauseEvaluator.evaluateParent(results, suchThat.firstArgument[0], suchThat.firstArgument[1], suchThat.secondArgument[0], suchThat.secondArgument[1]);
		rs.addSuchThatResults(results);
	}
}

void QueryProcessor::evaluateParentTClause(Query query, ResultStorage& rs)
{
	ParentClauseEvaluator parentClauseEvaluator;
	for (SuchThatClause suchThat : query.suchThatClauses) {
		vector <vector<string>>results;
		parentClauseEvaluator.evaluateParentT(results, suchThat.firstArgument[0], suchThat.firstArgument[1], suchThat.secondArgument[0], suchThat.secondArgument[1]);
		rs.addSuchThatResults(results);
	}
}

void QueryProcessor::evaluateCallsClause(Query query, ResultStorage& rs)
{
	CallsClauseEvaluator callsClauseEvaluator;
	for (SuchThatClause suchThat : query.suchThatClauses) {
		vector <vector<string>>results;
		callsClauseEvaluator.evaluate(0, results, suchThat.firstArgument[0], suchThat.firstArgument[1], suchThat.secondArgument[0], suchThat.secondArgument[1]);
		rs.addSuchThatResults(results);
	}
}

void QueryProcessor::evaluateCallsTClause(Query query, ResultStorage& rs)
{
	CallsClauseEvaluator callsClauseEvaluator;
	for (SuchThatClause suchThat : query.suchThatClauses) {
		vector <vector<string>>results;
		callsClauseEvaluator.evaluate(1, results, suchThat.firstArgument[0], suchThat.firstArgument[1], suchThat.secondArgument[0], suchThat.secondArgument[1]);
		rs.addSuchThatResults(results);
	}
}

void QueryProcessor::evaluateModifiesClause(Query query, ResultStorage& rs) 
{
	ModifiesClauseEvaluator modifiesClauseEvaluator;
	for (SuchThatClause suchThat : query.suchThatClauses) {
		vector <vector<string>>results;
		modifiesClauseEvaluator.evaluate(results, suchThat.firstArgument[0], suchThat.firstArgument[1], suchThat.secondArgument[0], suchThat.secondArgument[1]);
		rs.addSuchThatResults(results);
	}

}

void QueryProcessor::evaluateUsesClause(Query query, ResultStorage& rs)
{
	UsesClauseEvaluator usesClauseEvaluator;
	for (SuchThatClause suchThat : query.suchThatClauses) {
		vector <vector<string>>results;
		usesClauseEvaluator.evaluate(results, suchThat.firstArgument[0], suchThat.firstArgument[1], suchThat.secondArgument[0], suchThat.secondArgument[1]);
		rs.addSuchThatResults(results);
	}
}

void QueryProcessor::evaluatePatternClause(Query query, ResultStorage& rs)
{
	PatternClauseEvaluator patternClauseEvaluator;
	for (PatternClause pattern : query.patternClauses) {
		vector <vector<string>> results;
		patternClauseEvaluator.evaluate(results, pattern.LHS, pattern.RHS, query.declarationList);
		rs.addPatternResults(results);
	}
}

void QueryProcessor::filterOnlySelectClause(ResultStorage& rs, vector<string>& output) {
	
	vector<string> colResultVector;
	int numSelect = rs.selectResults.size();

	for (int i = 0; i < numSelect; i++) {
		colResultVector = rs.selectResults[i].selectResults;
		for (string p : colResultVector) {
			output.push_back(p);
		}
	}

}

void QueryProcessor::findDuplicates(const vector<string>& vec, vector<string>& duplicates) {
	unordered_map<string, int> freqMap;

	// Iterate through the vector
	for (const auto& str : vec) {
		// Increment the frequency of the string in the map
		freqMap[str]++;

		// If the frequency becomes 2, it's a duplicate
		if (freqMap[str] == 2) {
			duplicates.push_back(str);
		}
	}
}

void QueryProcessor::filterOnlyPatternClause(Query queryObj, ResultStorage& rs, vector<string>& answer) {
	
	vector<string> output;
	vector<vector<string>> output2;

	for (int i = 0; i < queryObj.patternClauses.size(); i++) { //maximum 2 pattern clause

		if (queryObj.patternClauses[i].patternSynonym == queryObj.selectClauses[i].name) { 	// valid pattern filter
			for (vector<string> pr : rs.patternResults[i].patternResults) {
				output.push_back(pr[0]);
			}

		}
		if (queryObj.patternClauses[i].LHS[1] == queryObj.selectClauses[i].name) { 	// valid pattern filter
			for (vector<string> pr : rs.patternResults[i].patternResults) {
				output.push_back(pr[1]);
			}
		}
		else {
			output.insert(output.end(), rs.selectResults[0].selectResults.begin(), rs.selectResults[0].selectResults.end());
		}

	}

	if (queryObj.patternClauses.size() > 1) {//if there is 2 patternClause
		findDuplicates(output, answer);
	}
	else {
		answer = output;
	}

}

bool QueryProcessor::isOverlapped(Query queryObj, int& clause1Arg, int& clause2Arg) {
	auto& clauses = queryObj.suchThatClauses;

	// Store the relevant parts of the query object to simplify the comparisons
	auto& clause_0 = clauses[0];
	auto& clause_1 = clauses[1];

	// Check each combination of arguments and clauses
	bool condition_1 = clause_0.firstArgument[1] == clause_1.firstArgument[1];
	bool condition_2 = clause_0.firstArgument[1] == clause_1.secondArgument[1];
	bool condition_3 = clause_0.secondArgument[1] == clause_1.firstArgument[1];
	bool condition_4 = clause_0.secondArgument[1] == clause_1.secondArgument[1];

	// Check which condition is true
	if (condition_1) {
		clause1Arg = 0;
		clause2Arg = 0;
		return 1;
	}	
	else if (condition_2) {
		clause1Arg = 0;
		clause2Arg = 1;
		return 1;
	}
	else if (condition_3) {
		clause1Arg = 1;
		clause2Arg = 0;
		return 1;
	}
	else if (condition_4) {
		clause1Arg = 1;
		clause2Arg = 1;
		return 1;
	}
	else {
		return 0; // None of the conditions are true
	}
}


void QueryProcessor::filterOnlySuchThatClause(Query queryObj, ResultStorage& rs, vector<string>& output)
{
	//meaningless query: both first Argument synonym in suchThat != select Clause and second Argument synonym in suchThat != select Clause)
	vector<string> overlapping;
	vector<string> tempOutput;
	int clause1Arg, clause2Arg;
	// such that Modifies(a,v) or Parent(p,c) or Uses(a,v)

	if (queryObj.suchThatClauses.size() > 1 && isOverlapped(queryObj, clause1Arg, clause2Arg)) { //find the overlapping argument
		for (vector<string> st : rs.suchThatResults[0].suchThatResults) {
			overlapping.push_back(st[clause1Arg]);
		}
		for (vector<string> st : rs.suchThatResults[1].suchThatResults) {
			overlapping.push_back(st[clause2Arg]);
		}
		findDuplicates(overlapping, tempOutput);

		string overlappedArg;

		if (clause1Arg == 0) {
			overlappedArg = queryObj.suchThatClauses[0].firstArgument[1];
		}
		else if (clause1Arg == 1) {
			overlappedArg = queryObj.suchThatClauses[0].secondArgument[1];
		}
		if (overlappedArg == queryObj.selectClauses[0].name) {
			output = tempOutput;
		}
		else if (queryObj.selectClauses[0].designEntity == "while" || queryObj.selectClauses[0].designEntity == "if") {
			if (queryObj.suchThatClauses[0].relRef == "Parent" || queryObj.suchThatClauses[0].relRef == "Parent*") {
				for (vector<string> st : rs.suchThatResults[0].suchThatResults) {
					if (find(tempOutput.begin(), tempOutput.end(), st[1]) != tempOutput.end()) {
						output.push_back(st[0]);
					}
				}
			}
			else if (queryObj.suchThatClauses[1].relRef == "Parent" || queryObj.suchThatClauses[1].relRef == "Parent*") {
				for (vector<string> st : rs.suchThatResults[1].suchThatResults) {
					if (find(tempOutput.begin(), tempOutput.end(), st[1]) != tempOutput.end()) {
						output.push_back(st[0]);
					}
				}
			}
		}
		else { 
			filterOnlySelectClause(rs, output);
		}
	}
	else { //no overlap or just 1 such that
		for (int i = 0; i < queryObj.suchThatClauses.size(); i++) { //maximum 2 pattern clause
			//Select s modifies (s,_) or parent (s,0) 
			if (queryObj.suchThatClauses[i].firstArgument[1] == queryObj.selectClauses[0].name) { 	// valid pattern filter
				for (vector<string> st : rs.suchThatResults[i].suchThatResults) {
					vector<string> sR = rs.selectResults[i].selectResults;
					if (queryObj.selectClauses[0].designEntity == "procedure") {
						string procedure_name = getProcedureNameFromStmtLine(st[0]);
						tempOutput.push_back(procedure_name);
						cout << procedure_name << endl;
					}
					else {
						for (string s : sR) {
							if (s == st[0]) {
								tempOutput.push_back(st[0]);
							}
						}
					}
				}
			}
			//Select v modifies (a,v) or Select s parent (8,s) 
			else if (queryObj.suchThatClauses[i].secondArgument[1] == queryObj.selectClauses[i].name) {
				for (vector<string> st : rs.suchThatResults[i].suchThatResults) {
					vector<string> sR = rs.selectResults[i].selectResults;
					for (string s : sR) {
						if (s == st[1]) {
							tempOutput.push_back(st[1]);
						}
					}
				}
			}
			else if ((queryObj.suchThatClauses[i].relRef != "Parent" || queryObj.suchThatClauses[i].relRef != "Parent*") && (queryObj.selectClauses[i].designEntity == "assign" || queryObj.selectClauses[i].designEntity == "read" ||
				queryObj.selectClauses[i].designEntity == "print" || queryObj.selectClauses[i].designEntity == "if" || queryObj.selectClauses[i].designEntity == "while")) {
				for (vector<string> st : rs.suchThatResults[i].suchThatResults) {
					vector<string> sR = rs.selectResults[i].selectResults;
					for (string s : sR) {
						if (s == st[0]) {
							tempOutput.push_back(st[0]);
						}
					}
				}
			}
			else if (queryObj.selectClauses[i].designEntity == "variable") {
				for (vector<string> st : rs.suchThatResults[i].suchThatResults) {
					vector<string> sR = rs.selectResults[i].selectResults;
					for (string s : sR) {
						if (s == st[1]) {
							tempOutput.push_back(st[1]);
						}
					}
				}
			}
			else {
				tempOutput = rs.selectResults[i].selectResults;
			}
		}
		if (queryObj.suchThatClauses.size() > 1) {
			findDuplicates(tempOutput,output);
		}
		else {
			output = tempOutput;
		}
	}
}


string QueryProcessor::getProcedureNameFromStmtLine(string stmtLine) {
	string procedureName;
	Database::getProceduresFromLine(stmtLine, procedureName);

	return procedureName;
}

bool QueryProcessor::isStPtOverlapped(Query queryObj, int& stArg, int& ptArg) {
	auto& stClause = queryObj.suchThatClauses[0];
	auto& ptClause = queryObj.patternClauses[0];

	// Check each combination of arguments and clauses
	bool condition_1 = stClause.firstArgument[1] == ptClause.patternSynonym;
	bool condition_2 = stClause.secondArgument[1] == ptClause.patternSynonym;
	bool condition_3 = stClause.firstArgument[1] == ptClause.LHS[1];
	bool condition_4 = stClause.secondArgument[1] == ptClause.LHS[1];

	// Check which condition is true
	if (condition_1) {
		stArg = 0;
		ptArg = 0;
		return 1;
	}
	else if (condition_2) {
		stArg = 1;
		ptArg = 0;
		return 1;
	}
	else if (condition_3) {
		stArg = 0;
		ptArg = 1;
		return 1;
	}
	else if (condition_4) {
		stArg = 1;
		ptArg = 1;
		return 1;
	}
	else {
		return 0; // None of the conditions are true
	}
}

void QueryProcessor::filterSuchThatPatternClause(Query queryObj, ResultStorage& rs, vector<string>& output) {


	//meaningless query: both first Argument synonym in suchThat != select Clause and second Argument synonym in suchThat != select Clause)
	vector<string> overlapping;
	vector<vector <string>> tempResult1;
	vector<vector <string>> tempResult2;
	int stArg, ptArg;
	// such that Modifies(a,v) or Parent(p,c) or Uses(a,v)

	if (isStPtOverlapped(queryObj, stArg, ptArg)) { //find the overlapping argument
		cout << "is Overlapping" << endl;
		for (vector<string> st : rs.suchThatResults[0].suchThatResults) {
			for (vector<string> pt : rs.patternResults[0].patternResults) {
				if (st[stArg] == pt[ptArg]) {
					tempResult1.push_back({ pt[0],pt[1] });
					tempResult2.push_back({ st[0],st[1] });
				}
			}
		}


		if (queryObj.selectClauses[0].name == queryObj.patternClauses[0].patternSynonym) {
			for (vector<string> pt : tempResult1) {
				for (string sl : rs.selectResults[0].selectResults) {
					if (pt[0] == sl) {
						output.push_back(sl);
					}
				}
			}
		}
		else if (queryObj.selectClauses[0].name == queryObj.patternClauses[0].LHS[1]) {
			for (vector<string> pt : tempResult1) {
				for (string sl : rs.selectResults[0].selectResults) {
					if (pt[1] == sl) {
						output.push_back(sl);
					}
				}
			}
		}
		else if (queryObj.selectClauses[0].name == queryObj.suchThatClauses[0].firstArgument[1]) {
			for (vector<string> st : tempResult2) {
				for (string sl : rs.selectResults[0].selectResults) {
					if (st[0] == sl) {
						output.push_back(sl);
					}
				}
			}

		}
		else if (queryObj.selectClauses[0].name == queryObj.suchThatClauses[0].secondArgument[1]) {
			for (vector<string> st : tempResult2) {
				for (string sl : rs.selectResults[0].selectResults) {
					if (st[1] == sl) {
						output.push_back(sl);
					}
				}
			}
		}

		// modify (a,1) pattern a ("x",_)
		// parent (w, a) pattern a ("x", _)
		else if (queryObj.selectClauses[0].designEntity == "while" || queryObj.selectClauses[0].designEntity == "if") {
			if (queryObj.suchThatClauses[0].relRef == "Parent" || queryObj.suchThatClauses[0].relRef == "Parent*") {
				for (vector<string> st : rs.suchThatResults[0].suchThatResults) {
					if (find(overlapping.begin(), overlapping.end(), st[1]) != overlapping.end()) {
						output.push_back(st[0]);
					}
				}
			}
		}
		else {
			filterOnlySelectClause(rs, output);
		}
	}

	// select v modify (s, v) pattern a ("x",_)
	//suchThat + Pattern, but Pattern is meaningless
	//Select synonym == suchThat first Arg or second Arg and Select synonym != pattern synonym or pattern LHS
	else if ((queryObj.selectClauses[0].name == queryObj.suchThatClauses[0].firstArgument[1] || queryObj.selectClauses[0].name == queryObj.suchThatClauses[0].secondArgument[1]) && !(queryObj.selectClauses[0].name == queryObj.patternClauses[0].patternSynonym || queryObj.selectClauses[0].name == queryObj.patternClauses[0].LHS[1])) {
		filterOnlySuchThatClause(queryObj, rs, output);
	}

	// select a modify (s, v) pattern a ("x",_)
	//suchThat + Pattern, but Such That is meaningless
	//Select synonym != suchThat first Arg or second Arg and Select synonym == pattern synonym or pattern LHS
	else if ((queryObj.selectClauses[0].name == queryObj.patternClauses[0].patternSynonym || queryObj.selectClauses[0].name == queryObj.patternClauses[0].LHS[1]) && !(queryObj.selectClauses[0].name == queryObj.suchThatClauses[0].firstArgument[1] || queryObj.selectClauses[0].name == queryObj.suchThatClauses[0].secondArgument[1])) {
		filterOnlyPatternClause(queryObj, rs, output);
	}

	// select r modify (s, v) pattern a ("x",_)
	else {
		filterOnlySelectClause(rs, output);
	}

}



