#include "QueryParser.h"
#include <iostream>

#include <regex>

#define INTEGER_PATTERN "[0-9]+"
#define NAME_PATTERN "[a-zA-Z][a-zA-Z0-9]*"
#define IDENT_PATTERN "[a-zA-Z][a-zA-Z0-9]*"

constexpr auto SYNONYM_PATTERN = "(print|read|variable|constant|call|assign|while|if|procedure|stmt)";


void QueryParser::parse(list<string> tokens, Query& query)
{
	remainingTokens = tokens;
	currentDeclarationList = {};

	parseDeclarationList(query);
	parseSelectClause(query);


	while (remainingTokens.size() > 0) {
		if (remainingTokens.front() == "such") {
			parseSuchThatClause(query);

		} else if (remainingTokens.front() == "pattern") {
			cout << "check pattern!" << endl;
			parsePatternClause(query);
		}
		else {
			next();
		}
	}
}

// assign a1, a2; variable v
void QueryParser::parseDeclarationList(Query& query)
{
	while (remainingTokens.front() != "Select") {

		string designEntity = remainingTokens.front();
		if (!validateDesignEntity(designEntity)) {
			return;
		}
		next();
		string synonym = remainingTokens.front();
		if (!validateSynonym(synonym, false)) {
			return;
		}
		currentDeclarationList[synonym] = designEntity;
		next();
		string separator = remainingTokens.front();
		while (separator == ",") {
			expect(",");
			string nextSynonym = remainingTokens.front();
			if (!validateSynonym(nextSynonym, false)) {
				return;
			}
			currentDeclarationList[nextSynonym] = designEntity;
			next();
			separator = remainingTokens.front();
		}
		expect(";");
			
	}

	query.addDeclarationList(currentDeclarationList);
	return;

}

void QueryParser::parseSelectClause(Query& currentQuery)
{
	expect("Select");
	string afterSelect = remainingTokens.front();
	if (afterSelect == "<") {
		expect("<");
		afterSelect = remainingTokens.front();
		while (afterSelect != ">") {
			if (!validateSynonym(afterSelect, true)) {
				return;
			}
			currentQuery.addSelectClause(afterSelect, currentDeclarationList[afterSelect]);
			next();
			expect(",");
			afterSelect = remainingTokens.front();
		}
	}
	else {
		if (!validateSynonym(afterSelect, true)) {
			return;
		}
		currentQuery.addSelectClause(afterSelect, currentDeclarationList[afterSelect]);
		cout << "Single Select: Synonym: " << currentQuery.selectClauses[0].name  << " " << currentQuery.selectClauses[0].designEntity << endl;
	}
	next();
	return;
}

void QueryParser::parseSuchThatClause(Query& currentQuery)
{
	expect("such");
	expect("that");
	string relRef = remainingTokens.front();
	next();

	if (remainingTokens.front() == "*") {
		relRef = relRef + "*";	
		next();
	}

	expect("(");
	vector<string> firstArgumentPair;
	vector<string> secondArgumentPair;

	string firstArgument = remainingTokens.front();
	bool first = validateArgumentType(firstArgument, firstArgumentPair, true);
	if (!first) {
		return;
	}
	//cout << "firstArgument:" << firstArgumentPair[0] << " " << firstArgumentPair[1] << endl;
	expect(",");
	
	string secondArgument = remainingTokens.front();
	bool second = validateArgumentType(secondArgument, secondArgumentPair, true);
	//cout << "second:" << secondArgumentPair[0] << " " << secondArgumentPair[1] << endl;
	if (!second) {
		return;
	}
	currentQuery.addSuchThatClause(relRef, firstArgumentPair, secondArgumentPair);

	expect(")");
	cout << "Single Such That: relRef: " << currentQuery.suchThatClauses[0].relRef << "( " << 
		currentQuery.suchThatClauses[0].firstArgument[0] << " " << currentQuery.suchThatClauses[0].firstArgument[1] << " , " <<
		currentQuery.suchThatClauses[0].secondArgument[0] << " " << currentQuery.suchThatClauses[0].secondArgument[1] <<  " )" << endl;

}

void QueryParser::parsePatternClause(Query& currentQuery)
{
	vector<string> LHSPair;
	vector<string> RHSPair;
	expect("pattern");
	string patternSynonym = remainingTokens.front();
	if (!validateSynonym(patternSynonym, true)) {
		return;
	}
	next();
	expect("(");
	
	string LHS = remainingTokens.front();
	if (!validateArgumentType(LHS, LHSPair, false)) { 
		return; 
	}

	expect(",");
	cout << "left argument:" << LHSPair[0] << " " << LHSPair[1] << endl;

	string RHS = remainingTokens.front();

	if (!validatePartialPattern(RHS, RHSPair)) {
		return;
	}

	cout << "right argument:" << RHSPair[0] << " " << RHSPair[1] << endl;

	currentQuery.addPatternClause(patternSynonym, LHSPair, RHSPair);

	cout << "Single Pattern: name: " << currentQuery.patternClauses[0].patternSynonym << "( " <<
		currentQuery.patternClauses[0].LHS[0] << " " << currentQuery.patternClauses[0].LHS[1] << " , " <<
		currentQuery.patternClauses[0].RHS[0] << " " << currentQuery.patternClauses[0].RHS[1] << " )" << endl;

}

bool QueryParser::validatePartialPattern(string argument, vector<string>& argumentPair) {
	string rhs = argument;
	bool leftPartial = false, rightPartial = false;
	bool ident = false;
	string IDENT;
	
	while (rhs != ")") {
		if (rhs == "_") {
			if (!leftPartial && !ident) {
				leftPartial = 1;
			}
			else {
				rightPartial = 1;
			}
		}
		else if (rhs == "\"") {
			ident = true;
		}
		else if (ident) {
			IDENT += rhs;
		}
		next();
		rhs = remainingTokens.front();
	}

	if (ident) 
	{
		if (leftPartial) {
			if (rightPartial)
				argumentPair = { "both partial", IDENT };
			else
				argumentPair = { "left partial", IDENT };
		}
		else {
			if (rightPartial)
				argumentPair = { "right partial", IDENT };
			else
				argumentPair = { "IDENT", IDENT };
		}
	}
	else {
		argumentPair = { "undeclared", argument };
	}
	return true;
}


bool QueryParser::validateArgumentType(string argument, vector<string> &argumentPair, bool lineNumber) {

	if (validateSynonym(argument, true)) {
		argumentPair = { currentDeclarationList[argument], argument };
		next();
	}
	else if (argument == "_") {
		argumentPair = { "undeclared", argument };
		next();
	}
	else if (lineNumber && validateNumber(argument)) {
		argumentPair = { "line number", argument };
		next();
	}
	else if (argument == "\""){
		expect("\"");
		argument = remainingTokens.front();
		if (!validateIdent(argument)) {
			return false;
		}
		argumentPair = { "IDENT", argument };
		next();
		expect("\"");
	}
	else {
		cout << "error: It is not a Valid Argument" << endl;
		return false;
	}
	return true;
}


bool QueryParser::validateDesignEntity(string text)
{
	if (regex_match(text, regex(SYNONYM_PATTERN) )) {
		return true;
	}
	cout << "error with Design Entity!" << endl;
	return false;
}

bool QueryParser::validateIdent(string text)
{
	if (regex_match(text, regex(IDENT_PATTERN))) {
		return true;
	}
	cout << "error: It is not an Ident" << endl;
	return false;

}

bool QueryParser::validateNumber (string text) {

	if (regex_match(text, regex(INTEGER_PATTERN))) {
		return true;
	}
	cout << "error: It is not an Integer" << endl;
	return false;

}

bool QueryParser::validateSynonym(string text, bool checkInDeclaration)
{
	if (!regex_match(text, regex(NAME_PATTERN))) {
		cout << "error: It is not a Synonym Format" << endl;
		return false;
	}
	if (checkInDeclaration && currentDeclarationList.find(text) == currentDeclarationList.end()) {
		cout << "error: It is not a Synonym in the Declaration List" << endl;
		return false;
	}
	return true;
}

void QueryParser::expect(string symbol)
{
	if (match(symbol)) {
		next();
		return;
	}

	cout << "expected: '" << symbol << " got " << remainingTokens.front() << " instead!";
}

bool QueryParser::match(string symbol)
{
	if (symbol == remainingTokens.front()) {
		return 1;
	}
	return 0;
}

void QueryParser::next()
{
	if (!remainingTokens.empty()) {
		remainingTokens.pop_front();
	}
}
