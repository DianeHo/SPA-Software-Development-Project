#include "SourceProcessor.h"
#include "database.h"
#include "ConvertPostfix.h"
#include <regex>
#include <iostream>
#include <stack>


#define INTEGER_PATTERN "[0-9]+"
#define NAME_PATTERN "[a-zA-Z][a-zA-Z0-9]*"
#define IDENT_PATTERN "[a-zA-Z][a-zA-Z0-9]*"
#define SYMBOL "[+\\-*/\\(\\)\\=\\d+\\!]"
#define TERM "(.*?)([*/%])(.*)"
#define FACTOR "\\((.*)\\)"
#define EXPRESSION "(.*?)([+-])(.*)"
#define REL_FACTOR "\\((.*?)\\s*([><])\\s*(.*?)\\)"

bool elseCondition = 0;
stack<string>parentLevel;
string tempParent;

int line_num = 0;
string sscountLine = to_string(line_num);

string startLine, endLine;
vector<string> procedureList;


// method for processing the source program
// This method currently only inserts the procedure name into the database
// using some highly simplified logic.
// You should modify this method to complete the logic for handling all the required syntax.
void SourceProcessor::process(string program) {
	// initialize the database
	Database::initialize();

	// tokenize the program
	Tokenizer tk;
	vector<string> tokens;
	tk.tokenize(program, tokens);

	//copy the token into a list
	list<string> ltokens;
	size_t i = 0;
	while (i < tokens.size())
	{
		string insert = tokens.at(i);
		ltokens.push_back(insert);
		i++;
	}
	if (!ltokens.empty()) {
		parse(ltokens);
	}

}

void SourceProcessor::parse(list<string>tokens) {
	tTokens = tokens;
	try {
		parseProcedure();

	}
	catch (const std::exception& ex) {
		// Use 'ex' in some way, for example, logging the exception message.
		std::cerr << "Exception caught: " << ex.what() << std::endl;
	}
}

void SourceProcessor::next() { //to pop tToken and move to next token
	if (!tTokens.empty()) {
		tTokens.pop_front();
	}
}

bool SourceProcessor::validateName(string token) { 
	return regex_match(token, regex(NAME_PATTERN));
}

bool SourceProcessor::match(string symbol)
{
    if (!tTokens.empty() && symbol == tTokens.front()) {
        return true;
    }
    return false;
}

bool SourceProcessor::validateSymbol(string token) {
	return regex_match(token, regex(SYMBOL));
}

bool SourceProcessor::validateConstant(string token) {
	return regex_match(token, regex(INTEGER_PATTERN));
}

void SourceProcessor::expect(string symbol) {
	traceParent();
	if (match(symbol)) {
		next();
	}
	return;
}

void SourceProcessor::parseProcedure() {
	while (!tTokens.empty()) {
		if (tTokens.front() == "procedure") {
			next();
			startLine = to_string(line_num + 1);
			if (validateName(tTokens.front())) {
				string procedureName = tTokens.front();
				procedureList.push_back(procedureName);
				next();
				expect("{");
				parseStatement();
				Database::insertProcedure(procedureName, startLine, endLine); // insert the procedure into the database
			}
			else {
				//catch error
			}
		}
		else {
			//catch error
		}
	}
}


void SourceProcessor::countLine() {
	line_num +=1;
	sscountLine = to_string(line_num);
}



void SourceProcessor::parseAssignment() {
	string rhs = "";
	string lhs = tTokens.front();
	parseVariable("LHS");
	next();
	expect("=");
	while (!tTokens.empty() && tTokens.front() != ";") {
		rhs += tTokens.front();
		if (validateConstant(tTokens.front())) {
			parseConstant(tTokens.front());
		}
		if (!validateSymbol(tTokens.front()) && !validateConstant(tTokens.front())) {
			parseVariable("RHS");
		}
		next();
	}
	string rhsPostfix = ConvertPostfix::infixToPostfix(rhs);
	Database::insertStatement(sscountLine, "assign",lhs, rhs, rhsPostfix); // Pass the parent parameter
}

string SourceProcessor::parseCondition(string ifwhile) {
	string cond;
	while (!tTokens.empty() && tTokens.front() != "{") { //access the condition
		if (tTokens.front() == "then") {
			next();
			continue;
		}
		if (validateConstant(tTokens.front())) {
			parseConstant(tTokens.front());
		}
		if (validateName(tTokens.front())) {
			parseVariable(ifwhile);
		}
		cond.append(tTokens.front());
		next();
	}
	return cond;
}

string parse_factor(const string& input);
string parse_expr(const string& input);

string parse_term(const string& input) {

	smatch matches;
	if (regex_match(input, matches, regex(TERM))) {
		string left = parse_term(matches[1]);
		string right = parse_factor(matches[3]);
		return left + matches[2].str() + right;
	}
	else {
		return parse_factor(input);
	}
}

string parse_factor(const string& input) {

	smatch matches;
	if (regex_match(input, matches, regex(FACTOR))) {
		return parse_expr(matches[1]);
	}
	else {
		return input;
	}
}


string parse_expr(const string& input) {

	smatch matches;
	if (regex_match(input, matches, regex(EXPRESSION))) {
		string left = parse_expr(matches[1]);
		string right = parse_term(matches[3]);
		return left + matches[2].str() + right;
	}
	else {
		return parse_term(input);
	}
}

void SourceProcessor::parseStatement() {
	while (!tTokens.empty() && tTokens.front() != "procedure") {
		countLine();
		string token1 = tTokens.front();
		if (token1 == "read" || token1 == "print") {
			next();
			Database::insertStatement(sscountLine, token1, tTokens.front(), "NULL", "NULL");
			if (token1 == "read") {
				parseVariable("READ");
			}
			else {
				parseVariable("PRINT");
			}
			next();
		}
		else if (token1 == "call") {
			next();
			Database::insertStatement(sscountLine, token1, tTokens.front(), "NULL", "NULL");
			next();
		}
		else if (token1 == "if" || token1 == "while") {
			next();
			string cond = parseCondition(token1);
			smatch matches;
			string lhs, rhs;
			cout << "\ncondition: " << cond << " ";
			if (regex_match(cond, matches, regex(REL_FACTOR))) {
				// split into LHS and RHS
				lhs = parse_expr(matches[1]);
				rhs = parse_expr(matches[3]);
				cout << "supposed:" << lhs;
				// Remove outermost brackets
				if (lhs.front() == '(' && lhs.back() == ')') {
					lhs = lhs.substr(1, lhs.size() - 2);
				}
				if (rhs.front() == '(' && rhs.back() == ')') {
					rhs = rhs.substr(1, rhs.size() - 2);
				}
			}
			Database::insertStatement(sscountLine, token1, lhs, rhs, "NULL");
		}
		else if (token1 == "else") {
			elseCondition = 1;
			next();
			expect("{");
			line_num -= 1;
			continue;
		}
		else if (validateName(tTokens.front())) { //if it is an assignment started with variable
			parseAssignment();
		}
		else {
			expect("}");
			line_num -= 1;
			continue;
		}
		parseParent();
		expect("{");
		expect(";");
		expect("}");
		//countLine();
	}
	endLine = sscountLine;
}

void SourceProcessor::traceParent() {
	if (elseCondition) {
		parentLevel.push(tempParent);
		elseCondition = 0;
	}
	else if (tTokens.front() == "{") {
		parentLevel.push(sscountLine);
	}
	else if (tTokens.front() == "}") {
		if (!parentLevel.empty()) {
			tempParent = parentLevel.top();
			parentLevel.pop();
		}
	}
}

void SourceProcessor::parseParent() {
	// put it with countLine
	if (!parentLevel.empty()) {
		Database::updateParent(parentLevel.top(), sscountLine);
	}
}

void SourceProcessor::parseVariable(string varCondition) {
	if (validateName(tTokens.front())) {
		Database::insertVariable(tTokens.front(), sscountLine, varCondition);
	}
}

void SourceProcessor::parseConstant(string value) {
		Database::insertConstant(value);
}
