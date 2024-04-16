#include "Database.h"
#include <iostream>
#include <utility>

sqlite3* Database::dbConnection;
vector<vector<string>> Database::dbResults;
char* Database::errorMessage;

// method to connect to the database and initialize tables in the database
void Database::initialize() {
	// open a database connection and store the pointer into dbConnection
	sqlite3_open("database.db", &dbConnection);

	// drop the existing procedure table (if any)
	string dropProcedureTableSQL = "DROP TABLE IF EXISTS procedure";
	sqlite3_exec(dbConnection, dropProcedureTableSQL.c_str(), NULL, 0, &errorMessage);

	// create a procedure table
	string createProcedureTableSQL = "CREATE TABLE procedure ( procedureName VARCHAR(255) PRIMARY KEY, start VARCHAR(255), end VARCHAR(255));";
	sqlite3_exec(dbConnection, createProcedureTableSQL.c_str(), NULL, 0, &errorMessage);

	// drop the existing stmt table (if any)
	string dropStatementTableSQL = "DROP TABLE IF EXISTS statement";
	sqlite3_exec(dbConnection, dropStatementTableSQL.c_str(), NULL, 0, &errorMessage);

	// create a statement table
	string createStatementTableSQL = "CREATE TABLE statement ( stmtLine VARCHAR(255) PRIMARY KEY, stmtEntity VARCHAR(255), stmtLHS VARCHAR (255), stmtRHS VARCHAR (255), stmtPostfix VARCHAR (255), parent VARCHAR(255));"; 
	sqlite3_exec(dbConnection, createStatementTableSQL.c_str(), NULL, 0, &errorMessage);

	// drop the existing variable table (if any)
	string dropVariableTableSQL = "DROP TABLE IF EXISTS variable";
	sqlite3_exec(dbConnection, dropVariableTableSQL.c_str(), NULL, 0, &errorMessage);

	// create a variable table
	string createVariableTableSQL = "CREATE TABLE variable ( varName VARCHAR(255), varLine VARCHAR(255) REFERENCES statement(stmtLine), varCondition VARCHAR (255), PRIMARY KEY (varName, varLine) );";
	sqlite3_exec(dbConnection, createVariableTableSQL.c_str(), NULL, 0, &errorMessage);

	// drop the existing constant table (if any)
	string dropConstantTableSQL = "DROP TABLE IF EXISTS constant";
	sqlite3_exec(dbConnection, dropConstantTableSQL.c_str(), NULL, 0, &errorMessage);

	// create a constant table
	string createConstantTableSQL = "CREATE TABLE constant ( constValue VARCHAR(255) PRIMARY KEY);";
	sqlite3_exec(dbConnection, createConstantTableSQL.c_str(), NULL, 0, &errorMessage);
	
	if (errorMessage) {
		//cout << errorMessage << endl;
	}

	// initialize the result vector
	dbResults = vector<vector<string>>();
}

// method to close the database connection
void Database::close() {
	sqlite3_close(dbConnection);
}

// method to insert a procedure into the database
void Database::insertProcedure(string procedureName, string start, string end) {
	string insertProcedureSQL = "INSERT INTO procedure ('procedureName','start','end') VALUES ('" + procedureName + "','" + start + "','" + end + "');";
	sqlite3_exec(dbConnection, insertProcedureSQL.c_str(), NULL, 0, &errorMessage);
}
// method to insert a statement for stmtline, lhs, rhs into the database
void Database::insertStatement(string stmtLine, string stmtEntity, string lhs, string rhs, string postfixRhs) {
	string insertStatementSQL = "INSERT INTO statement ('stmtLine','stmtEntity','stmtLHS','stmtRHS','stmtPostfix') VALUES ('" + stmtLine + "', '" + stmtEntity + "',  '" + lhs + "', '" + rhs + "',  '" + postfixRhs + "');";
	sqlite3_exec(dbConnection, insertStatementSQL.c_str(), NULL, 0, &errorMessage);
}

// method to insert a parent in a statement table into the database
void Database::updateParent(string parent, string stmtLine) {
	string updateParentSQL = "UPDATE statement SET parent = '" + parent + "' WHERE stmtLine = '" + stmtLine + "';";
	sqlite3_exec(dbConnection, updateParentSQL.c_str(), NULL, 0, &errorMessage);
}

// method to insert a constant into the database
void Database::insertConstant(string constValue) {
	string insertConstantSQL = "INSERT INTO constant ('constValue') VALUES ('" + constValue + "');";
	sqlite3_exec(dbConnection, insertConstantSQL.c_str(), NULL, 0, &errorMessage);
}
// method to insert a variable into the database
void Database::insertVariable(string varName, string varLine, string varCondition) {
	string insertVariableSQL = "INSERT INTO variable ('varName','varLine', 'varCondition') VALUES ('" + varName + "', '" + varLine + "','" + varCondition + "');";
	sqlite3_exec(dbConnection, insertVariableSQL.c_str(), NULL, 0, &errorMessage);
}



// method to get all the procedures from the database
void Database::getProcedures(vector<string>& procedures){
	// clear the existing results
	dbResults.clear();

	// retrieve the procedures from the procedure table
	// The callback method is only used when there are results to be returned.
	string getProceduresSQL = "SELECT DISTINCT procedureName FROM procedure;";
	sqlite3_exec(dbConnection, getProceduresSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		procedures.push_back(result);
	}
}


void Database::getCallsProcedures(vector<string>& calls) {
	// clear the existing results
	dbResults.clear();

	// retrieve the procedures from the procedure table
	// The callback method is only used when there are results to be returned.
	string getProceduresSQL = "SELECT DISTINCT stmtLHS FROM statement where stmtEntity='call';";
	sqlite3_exec(dbConnection, getProceduresSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		calls.push_back(result);
	}
}


void Database::getProceduresFromLine(string stmtLine, string& results)
{
	dbResults.clear();

	// retrieve the procedures from the procedure table
	// The callback method is only used when there are results to be returned.
	string getProceduresSQL = "SELECT DISTINCT procedureName FROM procedure WHERE start <= '"+ stmtLine +"' AND end >= '" + stmtLine + "' ;";
	sqlite3_exec(dbConnection, getProceduresSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		results = dbRow.at(0);
	}
}

void Database::getConstants(vector<string>& constants) {
	// clear the existing results
	dbResults.clear();

	// retrieve the procedure from the procedure table
	// The callback method is only used when there are results to be returned.
	string getConstantSQL = "SELECT DISTINCT constValue FROM constant;";
	sqlite3_exec(dbConnection, getConstantSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		constants.push_back(result);
	}
}

// method to get all the variable from the database
void Database::getVariables(vector<string>& variables) {
	// clear the existing results
	dbResults.clear();

	// retrieve the procedure from the procedure table
	// The callback method is only used when there are results to be returned.
	string getVariablesSQL = "SELECT DISTINCT varName FROM variable;";
	sqlite3_exec(dbConnection, getVariablesSQL.c_str(), callback, 0, &errorMessage);

	// postprocess the results from the database so that the output is just a vector of procedure names
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		variables.push_back(result);
	}
}



// method to get all the statement from the database
void Database::getStatements(string type, vector<string>& statements) {
	// clear the existing results
	dbResults.clear();

	// retrieve the variable name from the variable table
	// The callback method is only used when there are results to be returned.
	if (type == "stmt") { // retrieve all statements
		string sql = "SELECT stmtLine FROM statement;";
		sqlite3_exec(dbConnection, sql.c_str(), callback, 0, &errorMessage);
	}
	else { // retrieve read / print / assign statements
		string sql = "SELECT stmtLine FROM statement WHERE stmtEntity = '" + type + "';";
		sqlite3_exec(dbConnection, sql.c_str(), callback, 0, &errorMessage);
	}

	// Add output from database to vector for return
	for (vector<string> dbRow : dbResults) {
		string result;
		result = dbRow.at(0);
		statements.push_back(result);
	}
}

void Database::getParent(bool parentAst, string parent, string parentType, string child, string childType, vector<vector<string>>& results) {
	dbResults.clear();
	string sql;

	vector<vector<string>> firstResult;
	vector<vector<string>> secondResult;

	if (parentType == "line number") {
		if (parentAst) {
			sql = "WITH RECURSIVE xx AS ( SELECT stmtLine AS s, parent AS pr FROM statement WHERE parent = '" + parent + "' UNION ALL SELECT p.stmtLine, p.parent FROM statement p JOIN xx ON p.parent = xx.s ) SELECT pr,s FROM xx; ";
		}
		else {
			sql = "SELECT parent, stmtLine FROM statement WHERE parent = '" + parent + "'; ";
		}
	}
	else if (parentType == "if" || parentType == "while") { // parent(if, )
		if (parentAst) {
			sql = "WITH RECURSIVE xx AS ( SELECT stmtLine AS s, parent AS pr FROM statement WHERE stmtEntity = '" + parentType + "' UNION ALL SELECT p.stmtLine, p.parent FROM statement p JOIN xx ON p.parent = xx.s ) SELECT pr,s FROM xx WHERE pr != '0'; ";
		}
		else {
			sql = "SELECT parent, stmtLine FROM statement WHERE parent IN (SELECT stmtLine FROM statement WHERE stmtEntity = '" + parentType + "');";

		}
	}
	else if (parentType == "undeclared") {
		sql = "SELECT parent, stmtLine FROM statement;";
	}
	else if (parentType == "stmt") { //parent(_, ) or parent (s, )
		sql = "SELECT parent, stmtLine FROM statement WHERE parent != '0';";
	}
	else { // parent(r, ) -- not possible
		return;
	}
	
	sqlite3_exec(dbConnection, sql.c_str(), callback, 0, &errorMessage);
	for (vector<string> dbRow : dbResults) {
		string result;
		string parent = dbRow.at(0);
		string child = dbRow.at(1);
		firstResult.push_back({parent, child});
	}
	/*cout << "firstResult:" << endl;
	printVPResult(firstResult);
*/

	if (childType == "read" || childType == "assign" || childType == "print") { //parent( ,r)
		for (vector<string>tempParentChild : firstResult) {
			string parent = tempParentChild.at(0);
			dbResults.clear();
			sql = "SELECT parent FROM statement WHERE parent = '" + parent + "' AND stmtEntity = '" + childType + "';";
			sqlite3_exec(dbConnection, sql.c_str(), callback, 0, &errorMessage);
			if (!dbResults.empty()) {
				secondResult.push_back(tempParentChild);
			}
		}
	}
	else if (childType == "line number") {
		for (vector<string>tempParentChild : firstResult) {
			string parent = tempParentChild.at(0);
			dbResults.clear();
			sql = "SELECT parent FROM statement WHERE parent = '" + parent + "' AND stmtLine = '" + child + "';";
			cout << sql << endl;
			sqlite3_exec(dbConnection, sql.c_str(), callback, 0, &errorMessage);
			if (!dbResults.empty()) {
				secondResult.push_back({dbResults[0][0],child});
			}
		}
	}
	else if (childType == "stmt" || childType == "undeclared") { // parent( ,s) -- does not cover empty while or if
		secondResult = firstResult;
	}

	//cout << "secondResult:" << endl;
	//printVPResult(secondResult);

	results = secondResult;

}

void Database::getPatternStmt(vector<string> LHSPair, vector<string> RHSPair, vector<vector<string>>& results) {
	
	vector<vector<string>> firstResult;
	dbResults.clear();
	string sql;

	if (LHSPair[0] == "undeclared" || LHSPair[0] == "variable") {
		sql = "SELECT stmtLine, stmtLHS FROM statement WHERE stmtEntity = 'assign'";
	}
	else if (LHSPair[0] == "IDENT") {
		sql = "SELECT stmtLine, stmtLHS FROM statement WHERE stmtEntity = 'assign' AND stmtLHS = '" + LHSPair[1] + "';";
	}
	else {
		return;
	}
	sqlite3_exec(dbConnection, sql.c_str(), callback, 0, &errorMessage);
	if (dbResults.empty()) {
		return;
	}

	for (vector<string> dbRow : dbResults) {
		string stmtLine = dbRow.at(0);
		string stmtLHS = dbRow.at(1);
		firstResult.push_back({ stmtLine, stmtLHS });
	}

	for (vector<string> temp : firstResult) {
		string stmtLine = temp[0];
		string stmtLHS = temp[1];
		sql = "SELECT stmtPostfix FROM statement WHERE stmtLine = '" + stmtLine + "';";
		dbResults.clear();
		sqlite3_exec(dbConnection, sql.c_str(), callback, 0, &errorMessage);

		bool matchQuery = 0;
		string postfixRHS = dbResults[0][0];
		string postfixQuery = ConvertPostfix::infixToPostfix(RHSPair[1]);
		cout <<  "This is the PostfixQuery:" << postfixQuery << endl;
		if (RHSPair[0] == "undeclared") {
			matchQuery = 1;
		}
		else if (postfixRHS == postfixQuery) {
			matchQuery = 1;
		}
		else {
			bool leftEmpty = 0, rightEmpty = 0;
			if (RHSPair[0] == "left partial" || RHSPair[0] == "both partial") {
				leftEmpty = 1;
			}
			if (RHSPair[0] == "right partial" || RHSPair[0] == "both partial") {
				rightEmpty = 1;
			}
			matchQuery = ConvertPostfix::comparePostfix(leftEmpty,rightEmpty,postfixRHS, postfixQuery);
		}

		if (matchQuery) {
			results.push_back({ stmtLine,stmtLHS });
			cout << "Pattern In Datebase: " << stmtLine << ", " << stmtLHS << endl;
		}
	}
}

void Database::getModifies(vector<vector<string>>& results, string firstArgumentType, string firstArgumentValue, string secondArgumentType, string secondArgumentValue)
{	// Clear the existing results
	dbResults.clear();

	string modifiesSQL;
	string modifiesSQL2;

	if (firstArgumentType == "line number") {
		if (secondArgumentType == "IDENT") {
			modifiesSQL = "WITH RECURSIVE xx AS (SELECT stmtLine AS s, stmtEntity AS st, stmtLHS AS var FROM statement WHERE stmtLine = '" + firstArgumentValue + "' UNION ALL   SELECT p.stmtLine, p.stmtEntity, p.stmtLHS   FROM statement p   JOIN xx ON p.parent = xx.s) ,CalledProcedures AS (SELECT DISTINCT p.procedureName, s.stmtLine, s.stmtEntity,s.stmtLHS FROM statement s JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) WHERE s.stmtEntity IN ('read', 'assign') and stmtLHS='" + secondArgumentValue + "' UNION SELECT DISTINCT p.procedureName as nm2, s.stmtLine,s.stmtEntity,s.stmtLHS FROM statement s JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT)JOIN CalledProcedures cp ON  s.stmtEntity = 'call' AND s.stmtLHS = cp.procedureName ) SELECT DISTINCT s,var FROM xx WHERE st IN ('read', 'assign') and stmtLHS='" + secondArgumentValue + "'  AND (SELECT stmtEntity FROM statement WHERE stmtLine = '" + firstArgumentValue + "') IN ('while','if') UNION SELECT DISTINCT stmtLine, stmtLHS FROM statement WHERE stmtLine = '" + firstArgumentValue + "'  and stmtEntity IN ('read', 'assign') and stmtLHS='" + secondArgumentValue + "'  AND NOT EXISTS (SELECT 1 FROM statement WHERE stmtLine = '" + firstArgumentValue + "' AND stmtEntity IN ('while','if')) UNION SELECT DISTINCT cp.stmtLine,cp.stmtLHS FROM statement s JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) JOIN CalledProcedures cp ON s.stmtEntity = 'call' AND s.stmtLHS = cp.procedureName and (cp.procedureName In (select var from xx where st in ('call')) or p.procedureName In (select var from xx where st in ('call'))) and cp.stmtEntity in ('read','assign')and stmtLHS='" + secondArgumentValue + "'  ;; ";
			//"SELECT stmtLine, stmtLHS FROM statement WHERE stmtLine = '" + firstArgumentValue + "' AND stmtLHS = '" + secondArgumentValue + "';";
		}
		else { // secondArgumentType is a _ OR synonym i.e. variable design entity
			modifiesSQL = "WITH RECURSIVE xx AS (SELECT stmtLine AS s, stmtEntity AS st, stmtLHS AS var FROM statement WHERE stmtLine = '" + firstArgumentValue + "' UNION ALL   SELECT p.stmtLine, p.stmtEntity, p.stmtLHS   FROM statement p   JOIN xx ON p.parent = xx.s) ,CalledProcedures AS  (SELECT DISTINCT p.procedureName, s.stmtLine, s.stmtEntity,s.stmtLHS FROM statement s JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) WHERE s.stmtEntity IN ('read', 'assign') UNION SELECT DISTINCT p.procedureName as nm2, s.stmtLine,s.stmtEntity,s.stmtLHS FROM statement s JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT)JOIN CalledProcedures cp ON  s.stmtEntity = 'call' AND s.stmtLHS = cp.procedureName )SELECT DISTINCT s stmtLine,var stmtLHS FROM xx WHERE st IN ('read', 'assign') AND (SELECT stmtEntity FROM statement WHERE stmtLine = '" + firstArgumentValue + "') IN ('while','if') UNION SELECT DISTINCT stmtLine, stmtLHS FROM statement WHERE stmtLine = '" + firstArgumentValue + "'  and stmtEntity IN ('read', 'assign') AND NOT EXISTS (SELECT 1 FROM statement WHERE stmtLine = '" + firstArgumentValue + "' AND stmtEntity IN ('while','if')) UNION SELECT DISTINCT cp.stmtLine,cp.stmtLHS FROM statement s JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) JOIN CalledProcedures cp ON s.stmtEntity = 'call' AND s.stmtLHS = cp.procedureName and (cp.procedureName In (select var from xx where st in ('call')) or p.procedureName In (select var from xx where st in ('call'))) and cp.stmtEntity in ('read','assign')";

			//modifiesSQL = "SELECT stmtLine, stmtLHS FROM statement WHERE stmtLine = '" + firstArgumentValue + "';";
		}
	}

	else if (firstArgumentType == "assign" || firstArgumentType == "read") { // lhs is a synonym
		if (secondArgumentType == "IDENT") {
			modifiesSQL = "SELECT stmtLine, stmtLHS FROM statement WHERE stmtLHS = '" + secondArgumentValue + "' AND stmtEntity = '" + firstArgumentType + "' ;";
		}
		else {  // secondArgumentType is a _ OR synonym i.e. variable design entity
			modifiesSQL = "SELECT stmtLine, stmtLHS FROM statement WHERE stmtEntity = '" + firstArgumentType + "';";
		}
	}

	else if (firstArgumentType == "stmt" ) { // lhs is a synonym
		if (secondArgumentType == "IDENT") {
			modifiesSQL = "WITH RECURSIVE xx AS (SELECT stmtLine AS s,stmtEntity AS st, stmtLHS AS var FROM statement  WHERE st in ('read','assign') AND var = '" + secondArgumentValue + "' UNION ALL SELECT p.stmtLine, p.stmtEntity, p.stmtLHS FROM statement p  JOIN xx ON p.parent = xx.s) SELECT s, var FROM xx;";
			modifiesSQL2 = "WITH RECURSIVE xx AS (SELECT stmtLine AS s,stmtEntity AS st, stmtLHS AS var, parent AS pr FROM statement UNION ALL SELECT p.stmtLine, p.stmtEntity, p.stmtLHS, p.parent FROM statement p  JOIN xx ON p.parent = xx.s) SELECT pr, var FROM xx  WHERE st in ('read','assign')  AND var = '" + secondArgumentValue + "' ;";

		}
		else {  // secondArgumentType is a _ OR synonym i.e. variable design entity
			modifiesSQL = "WITH RECURSIVE xx AS (SELECT stmtLine AS s,stmtEntity AS st, stmtLHS AS var FROM statement  WHERE st in ('read','assign') UNION ALL SELECT p.stmtLine, p.stmtEntity, p.stmtLHS FROM statement p  JOIN xx ON p.parent = xx.s) SELECT s, var FROM xx;";
			modifiesSQL2 = "WITH RECURSIVE xx AS (SELECT stmtLine AS s,stmtEntity AS st, stmtLHS AS var, parent AS pr FROM statement UNION ALL SELECT p.stmtLine, p.stmtEntity, p.stmtLHS, p.parent FROM statement p  JOIN xx ON p.parent = xx.s) SELECT pr, var FROM xx  WHERE st in ('read','assign');";

		}
	}

	else if (firstArgumentType == "if" || firstArgumentType == "while") { // lhs is a synonym
		if (secondArgumentType == "IDENT") {
			modifiesSQL = "WITH RECURSIVE xx AS (SELECT stmtLine AS s,stmtEntity AS st, stmtLHS AS var FROM statement WHERE stmtEntity = '" + firstArgumentType + "' UNION ALL SELECT p.stmtLine, p.stmtEntity, p.stmtLHS FROM statement p JOIN xx ON p.parent = xx.s) SELECT s, var FROM xx WHERE st in ('read','assign') AND var = '" + secondArgumentValue + "';";
			modifiesSQL2 = "WITH RECURSIVE xx AS (SELECT stmtLine AS s,stmtEntity AS st, stmtLHS AS var FROM statement WHERE stmtEntity = '" + firstArgumentType + "' UNION ALL SELECT p.stmtLine, p.stmtEntity, p.stmtLHS FROM statement p JOIN xx ON p.parent = xx.s) SELECT pr, var FROM xx WHERE st in ('read','assign') AND var = '" + secondArgumentValue + "';";
		}
		else {  // secondArgumentType is a _ OR synonym i.e. variable design entity
			modifiesSQL = "WITH RECURSIVE xx AS (SELECT stmtLine AS s,stmtEntity AS st, stmtLHS AS var FROM statement WHERE stmtEntity = '" + firstArgumentType + "' UNION ALL SELECT p.stmtLine, p.stmtEntity, p.stmtLHS FROM statement p JOIN xx ON p.parent = xx.s) SELECT s, var FROM xx WHERE st in ('read','assign');";
			modifiesSQL2 = "WITH RECURSIVE xx AS (SELECT stmtLine AS s,stmtEntity AS st, stmtLHS AS var FROM statement WHERE stmtEntity = '" + firstArgumentType + "' UNION ALL SELECT p.stmtLine, p.stmtEntity, p.stmtLHS FROM statement p JOIN xx ON p.parent = xx.s) SELECT pr, var FROM xx WHERE st in ('read','assign');";

		}
	}
	else if (firstArgumentType == "procedure") { // lhs is a synonym
		if (secondArgumentType == "IDENT") { // Modifies (procedureSynonym, "X")
			modifiesSQL = R"(
				WITH RECURSIVE CalledProcedures AS (SELECT DISTINCT procedureName p1,procedureName,stmtLine,stmtEntity, stmtLHS FROM statement s 
				JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT)
				UNION SELECT DISTINCT cp.p1, p.procedureName, s.stmtLine, s.stmtEntity, s.stmtLHS FROM statement s 
				JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT)
				JOIN CalledProcedures cp ON cp.stmtLHS = p.procedureName ) 
				select DISTINCT s.stmtLine,NULL as var FROM statement s JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT)
				join (select DISTINCT * from CalledProcedures where procedureName in(select DISTINCT procedureName from CalledProcedures where stmtEntity in('read', 'assign')and stmtLHS='" + secondArgumentValue + "') 
				and ((stmtEntity in('read', 'assign') and stmtLHS='" + secondArgumentValue + "') or (stmtEntity = 'call' and stmtLHS in(select DISTINCT procedureName from CalledProcedures where stmtEntity in('read', 'assign')
				and stmtLHS='" + secondArgumentValue + "'))))t on s.stmtLHS=t.procedureName 
				UNION select DISTINCT stmtLine,stmtLHS from CalledProcedures where procedureName 
				in(select DISTINCT procedureName from CalledProcedures where stmtEntity in('read', 'assign')and stmtLHS='" + secondArgumentValue + "') and ((stmtEntity in('read', 'assign') and stmtLHS='" + secondArgumentValue + "') or 
				(stmtEntity = 'call' and stmtLHS in (select DISTINCT procedureName from CalledProcedures where stmtEntity in('read', 'assign')and stmtLHS='" + secondArgumentValue + "')))
				)";
				//"SELECT stmtLine, stmtLHS FROM statement WHERE stmtLHS = '" + secondArgumentValue + "' AND stmtEntity in ('read','assign') ;";
		}
		else {  // Modifies (procedureSynonym,_) OR Modifies (procedureSynonym, variable)
			modifiesSQL = R"(
				WITH RECURSIVE CalledProcedures AS (SELECT DISTINCT procedureName p1,procedureName,stmtLine,stmtEntity, stmtLHS FROM statement s 
				JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT)
				UNION SELECT DISTINCT cp.p1, p.procedureName, s.stmtLine, s.stmtEntity, s.stmtLHS FROM statement s
				JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT)
				JOIN CalledProcedures cp ON cp.stmtLHS = p.procedureName )
				select DISTINCT s.stmtLine, NULL as var FROM statement s JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT)
				join(select DISTINCT * from CalledProcedures where procedureName in(select DISTINCT procedureName from CalledProcedures where stmtEntity in('read', 'assign'))
					and ((stmtEntity in('read', 'assign')) or (stmtEntity = 'call' and stmtLHS in(select DISTINCT procedureName from CalledProcedures where stmtEntity in('read', 'assign')
					))))t on s.stmtLHS = t.procedureName
				UNION select DISTINCT stmtLine, stmtLHS from CalledProcedures where procedureName
				in(select DISTINCT procedureName from CalledProcedures where stmtEntity in('read', 'assign')) and ((stmtEntity in('read', 'assign')) or
					(stmtEntity = 'call' and stmtLHS in(select DISTINCT procedureName from CalledProcedures where stmtEntity in('read', 'assign'))))
				)";
				//"SELECT stmtLine, stmtLHS FROM statement WHERE stmtEntity in ('read','assign') ;";
		}
	}
	else if (firstArgumentType == "IDENT") { // procedure name
		if (secondArgumentType == "IDENT") { // Modifies ("main","x") 
			modifiesSQL = R"(
			WITH RECURSIVE CalledProcedures AS (
				SELECT DISTINCT procedureName p1, procedureName, stmtLine, stmtEntity, stmtLHS 
				FROM statement s 
				JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) 
				WHERE procedureName= '" + firstArgumentValue + "' 
				UNION 
				SELECT DISTINCT cp.p1, p.procedureName, s.stmtLine, s.stmtEntity, s.stmtLHS 
				FROM statement s 
				JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) 
				JOIN CalledProcedures cp ON cp.stmtLHS = p.procedureName
			) 
			SELECT DISTINCT s.stmtLine, NULL AS var 
			FROM statement s 
			JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) 
			JOIN (
				SELECT DISTINCT * 
				FROM CalledProcedures 
				WHERE procedureName IN (
					SELECT DISTINCT procedureName 
					FROM CalledProcedures 
					WHERE stmtEntity IN ('read', 'assign') AND stmtLHS='" + secondArgumentValue + "' 
				) 
				AND (
					(stmtEntity IN ('read', 'assign') AND stmtLHS='" + secondArgumentValue + "') 
					OR 
					(stmtEntity = 'call' AND stmtLHS IN (
						SELECT DISTINCT procedureName 
						FROM CalledProcedures 
						WHERE stmtEntity IN ('read', 'assign') AND stmtLHS='" + secondArgumentValue + "'
					))
				)
			) t ON s.stmtLHS=t.procedureName 
			UNION 
			SELECT DISTINCT stmtLine, stmtLHS 
			FROM CalledProcedures 
			WHERE procedureName 
			IN (
				SELECT DISTINCT procedureName 
				FROM CalledProcedures 
				WHERE stmtEntity IN ('read', 'assign') AND stmtLHS='" + secondArgumentValue + "' 
			) 
			AND (
				(stmtEntity IN ('read', 'assign') AND stmtLHS='" + secondArgumentValue + "') 
				OR 
				(stmtEntity = 'call' AND stmtLHS IN (
					SELECT DISTINCT procedureName 
					FROM CalledProcedures 
					WHERE stmtEntity IN ('read', 'assign') AND stmtLHS='" + secondArgumentValue + "'
				))
			)
		)";
		}
		else {  // Modifies ("main",_) OR Modifies ("main", variable)
			modifiesSQL = R"(
			WITH RECURSIVE CalledProcedures AS (
				SELECT DISTINCT procedureName p1, procedureName, stmtLine, stmtEntity, stmtLHS 
				FROM statement s 
				JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) 
				WHERE procedureName= '" + firstArgumentValue + "' 
				UNION 
				SELECT DISTINCT cp.p1, p.procedureName, s.stmtLine, s.stmtEntity, s.stmtLHS 
				FROM statement s 
				JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) 
				JOIN CalledProcedures cp ON cp.stmtLHS = p.procedureName
			) 
			SELECT DISTINCT s.stmtLine, NULL AS var 
			FROM statement s 
			JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) 
			JOIN (
				SELECT DISTINCT * 
				FROM CalledProcedures 
				WHERE procedureName IN (
					SELECT DISTINCT procedureName 
					FROM CalledProcedures 
					WHERE stmtEntity IN ('read', 'assign')
				) 
				AND (
					(stmtEntity IN ('read', 'assign')) 
					OR 
					(stmtEntity = 'call' AND stmtLHS IN (
						SELECT DISTINCT procedureName 
						FROM CalledProcedures 
						WHERE stmtEntity IN ('read', 'assign')
					))
				)
			) t ON s.stmtLHS = t.procedureName 
			UNION 
			SELECT DISTINCT stmtLine, stmtLHS 
			FROM CalledProcedures 
			WHERE procedureName 
			IN (
				SELECT DISTINCT procedureName 
				FROM CalledProcedures 
				WHERE stmtEntity IN ('read', 'assign')
			) 
			AND (
				(stmtEntity IN ('read', 'assign')) 
				OR 
				(stmtEntity = 'call' AND stmtLHS IN (
					SELECT DISTINCT procedureName 
					FROM CalledProcedures 
					WHERE stmtEntity IN ('read', 'assign')
				))
			)
		)";
		}
	}

	sqlite3_exec(dbConnection, modifiesSQL.c_str(), callback, 0, &errorMessage);
	for (vector<string> dbRow : dbResults) {
		results.push_back({ dbRow.at(0), dbRow.at(1) });
	}
	dbResults.clear();
	sqlite3_exec(dbConnection, modifiesSQL2.c_str(), callback, 0, &errorMessage);
\
	for (vector<string> dbRow : dbResults) {
		results.push_back({dbRow.at(0), dbRow.at(1)});
	}

}

void Database::getCalls (bool callsAst, vector<vector<string>>& results, string firstArgumentType, string firstArgumentValue, string secondArgumentType, string secondArgumentValue)
{
	dbResults.clear();
	string callsSQL;

	vector<vector<string>> firstResult;
	vector<vector<string>> secondResult;
	if (callsAst) {
		if (firstArgumentType == "IDENT") {
			if (secondArgumentType == "IDENT") { // call ("main","Procedure1")
				cout << "\ncall1";
				callsSQL = "WITH RECURSIVE CalledProcedures AS(SELECT DISTINCT procedureName, stmtLHS, stmtLine, stmtEntity FROM statement s JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) where procedureName = '" + firstArgumentValue + "' and stmtEntity = 'call' AND s.stmtLHS = '" + secondArgumentValue + "' UNION SELECT DISTINCT p.procedureName, s.stmtLHS, s.stmtLine, s.stmtEntity  FROM statement s  JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) JOIN CalledProcedures cp ON s.stmtEntity = 'call' and cp.stmtLHS = p.procedureName)SELECT DISTINCT procedureName p, stmtLHS q FROM CalledProcedures cp;";
			}
			else { // ("main",p)
				cout << "\ncall2";

				callsSQL = "WITH RECURSIVE CalledProcedures AS(SELECT DISTINCT procedureName, stmtLHS, stmtLine, stmtEntity FROM statement s JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) where procedureName = '" + firstArgumentValue + "' and stmtEntity = 'call'UNION SELECT DISTINCT p.procedureName, s.stmtLHS, s.stmtLine, s.stmtEntity  FROM statement s  JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) JOIN CalledProcedures cp ON s.stmtEntity = 'call' and cp.stmtLHS = p.procedureName)SELECT DISTINCT procedureName p, stmtLHS q FROM CalledProcedures cp;";
			}
		}
		else {  
			if (secondArgumentType == "IDENT") { // (p1,"Procedure1")
				cout << "\ncall3";

				callsSQL = "WITH RECURSIVE CalledProcedures AS(SELECT DISTINCT p.procedureName, s.stmtLHS  FROM statement s   JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) WHERE s.stmtEntity = 'call' AND s.stmtLHS = '" + secondArgumentValue + "' UNION SELECT DISTINCT p.procedureName, s.stmtLHS  FROM statement s  JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT)  JOIN CalledProcedures cp ON s.stmtEntity = 'call' AND cp.procedureName = s.stmtLHS)SELECT DISTINCT procedureName p, stmtLHS q FROM CalledProcedures;";
			}
			else { // (p1,p2)
				cout << "\ncall4";

				callsSQL = "WITH RECURSIVE CalledProcedures AS(SELECT DISTINCT p.procedureName, s.stmtLHS  FROM statement s   JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) WHERE s.stmtEntity = 'call' UNION SELECT DISTINCT p.procedureName, s.stmtLHS  FROM statement s  JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT)  JOIN CalledProcedures cp ON s.stmtEntity = 'call' AND cp.procedureName = s.stmtLHS)SELECT DISTINCT procedureName p, stmtLHS q FROM CalledProcedures;";

			}
		}
	}
	else {
		if (firstArgumentType == "IDENT") {
			if (secondArgumentType == "IDENT") { // calls* ("main","Procedure1")
				cout << "\ncall5";

				callsSQL = "SELECT DISTINCT procedureName p, stmtLHS q from (SELECT * from procedure)a LEFT JOIN (SELECT * FROM statement WHERE stmtEntity = 'call')b on cast(stmtLine as int) BETWEEN cast(a.start as int) and cast(a.end as int) where stmtLine NOTNULL and procedureName = '" + firstArgumentValue + "' and stmtLHS = '" + secondArgumentValue + "';";
			}
			else { // ("main",p)
				cout << "\ncall6";

				callsSQL = "SELECT DISTINCT procedureName p, stmtLHS q from (SELECT * from procedure)a LEFT JOIN (SELECT * FROM statement WHERE stmtEntity = 'call')b on cast(stmtLine as int) BETWEEN cast(a.start as int) and cast(a.end as int) where stmtLine NOTNULL and procedureName = '" + firstArgumentValue + "';";
			}
		}
		else {

			if (secondArgumentType == "IDENT") { //calls* (p1,"Procedure1")
				cout << "\ncall7";

				callsSQL = "SELECT DISTINCT procedureName p, stmtLHS q from(SELECT * from procedure)a LEFT JOIN (SELECT * FROM statement WHERE stmtEntity = 'call')b on cast(stmtLine as int) BETWEEN cast(a.start as int) and cast(a.end as int) WHERE stmtLine NOTNULL and stmtLHS = '" + secondArgumentValue + "';";
			}
			else { //calls* (p1,p2)
				cout << "\ncall8";

				callsSQL = "SELECT DISTINCT procedureName p, stmtLHS q from(SELECT * from procedure)a LEFT JOIN (SELECT * FROM statement WHERE stmtEntity = 'call')b on cast(stmtLine as int) BETWEEN cast(a.start as int) and cast(a.end as int) where stmtLine NOTNULL;";
			}
		}
	}

	sqlite3_exec(dbConnection, callsSQL.c_str(), callback, 0, &errorMessage);
	for (vector<string> dbRow : dbResults) {
		string initialProcedure = dbRow.at(0);
		string endProcedure = dbRow.at(1);
		results.push_back({ initialProcedure, endProcedure });
	}

}

void Database::getUses (vector<vector<string>>& results, string firstArgumentType, string firstArgumentValue, string secondArgumentType, string secondArgumentValue)
{
	results.clear();
	//cout << "\ntype" << type << " p1:" << procedure1 << " p2:" << procedure2;
	string usesSQL;
	string usesSQL2;
	if (firstArgumentType == "line number") {
		if (secondArgumentType == "IDENT") {
			usesSQL = "select varLine, varName from variable WHERE varLine='" + firstArgumentValue + "' AND varName = '" + secondArgumentValue + "'  AND varCondition IN ('PRINT','if', 'while','RHS')  ;";
		}
		else { // secondArgumentType is a _ OR synonym i.e. variable design entity
			usesSQL = "select varLine, varName from variable WHERE varLine='" + firstArgumentValue + "' AND varCondition IN ('PRINT','if', 'while','RHS')  ;";
		}
	}

	else if (firstArgumentType == "stmt") { // lhs is a synonym
		if (secondArgumentType == "IDENT") {
			usesSQL = "SELECT varLine, varName from variable WHERE (varName = '" + secondArgumentValue + "' AND varCondition IN ('PRINT','if', 'while','RHS')) ;";
		}
		else {  // secondArgumentType is a _ OR synonym i.e. variable design entity
			usesSQL = "SELECT varLine, varName from variable WHERE varCondition IN ('PRINT','if', 'while','RHS')  ;";
		}
	}

	else if (firstArgumentType == "assign") { // lhs is a synonym
		if (secondArgumentType == "IDENT") {
			usesSQL = "SELECT  varLine, varName from variable WHERE varName = '" + secondArgumentValue + "' AND varCondition = 'RHS' ;";
		}
		else {  // secondArgumentType is a _ OR synonym i.e. variable design entity
			usesSQL = "SELECT  varLine, varName from variable WHERE varCondition = 'RHS' ;";
		}
	}

	else if (firstArgumentType == "print") { // lhs is a synonym
		if (secondArgumentType == "IDENT") {
			usesSQL = "SELECT  varLine, varName from variable WHERE varName = '" + secondArgumentValue + "' AND varCondition = 'PRINT' ;";
		}
		else {  // secondArgumentType is a _ OR synonym i.e. variable design entity
			usesSQL = "SELECT  varLine, varName from variable WHERE varCondition = 'PRINT' ;";
		}
	}

	else if (firstArgumentType == "if" || firstArgumentType == "while") { // lhs is a synonym
		if (secondArgumentType == "IDENT") {
			usesSQL = "SELECT  varLine, varName from variable WHERE varName = '" + secondArgumentValue + "' AND varCondition = '"+ firstArgumentType +"' ;";
			usesSQL2 = R"(
						WITH RECURSIVE xx AS (
						SELECT stmtLine, stmtEntity, stmtLHS, parent, stmtRHS 
						FROM statement s 
						WHERE stmtEntity = '" + firstArgumentType + "' 
						UNION ALL  
						SELECT p.stmtLine, p.stmtEntity, p.stmtLHS, p.parent, p.stmtRHS 
						FROM statement p  
						JOIN xx ON p.parent = xx.stmtLine
						)
						SELECT DISTINCT 
						CASE 
							WHEN a2.stmtEntity = '" + firstArgumentType + "' THEN a2.stmtLine 
							WHEN s2.stmtEntity = '" + firstArgumentType + "' THEN s2.stmtLine 
						END AS stmtLine,
						b.varName 
						FROM (
						SELECT * 
						FROM (
							SELECT DISTINCT * 
							FROM xx
						) a 
						LEFT JOIN (
							SELECT * 
							FROM variable
						) b ON a.stmtLine = b.varLine 
						WHERE varName = '" + secondArgumentValue + "'
						AND varCondition IN ('PRINT','if', 'while','RHS')
						) a2 
						JOIN statement s2 ON a2.parent = s2.stmtLine;
						)";
		}
		else {  // secondArgumentType is a _ OR synonym i.e. variable design entity
			usesSQL = "SELECT  varLine, varName from variable WHERE varCondition = '" + firstArgumentType + "' ;";
			usesSQL2 = R"(
						WITH RECURSIVE xx AS (
						SELECT stmtLine, stmtEntity, stmtLHS, parent, stmtRHS 
						FROM statement s 
						WHERE stmtEntity = '" + firstArgumentType + "' 
						UNION ALL  
						SELECT p.stmtLine, p.stmtEntity, p.stmtLHS, p.parent, p.stmtRHS 
						FROM statement p  
						JOIN xx ON p.parent = xx.stmtLine
						)
						SELECT DISTINCT 
						CASE 
							WHEN a2.stmtEntity = '" + firstArgumentType + "' THEN a2.stmtLine 
							WHEN s2.stmtEntity = '" + firstArgumentType + "' THEN s2.stmtLine 
						END AS stmtLine,
						b.varName 
						FROM (
						SELECT * 
						FROM (
							SELECT DISTINCT * 
							FROM xx
						) a 
						LEFT JOIN (
							SELECT * 
							FROM variable
						) b ON a.stmtLine = b.varLine 
						WHERE varName IS NOT NULL 
						AND varCondition IN ('PRINT','if', 'while','RHS')
						) a2 
						JOIN statement s2 ON a2.parent = s2.stmtLine;
						)";
		}
	}
	else if (firstArgumentType == "IDENT") { // Procedure name
		if (secondArgumentType == "IDENT") { // Modifies (procedureSynonym, "X")
			usesSQL = R"(
					WITH RECURSIVE CalledProcedures AS (
						SELECT DISTINCT procedureName, stmtLHS, stmtLine, stmtEntity 
						FROM statement s 
						JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) 
						WHERE procedureName = '" + firstArgumentType + "' AND stmtEntity = 'call'  
						UNION 
						SELECT DISTINCT p.procedureName, s.stmtLHS, s.stmtLine, s.stmtEntity  
						FROM statement s  
						JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT)  
						JOIN CalledProcedures cp ON s.stmtEntity = 'call' AND cp.stmtLHS = p.procedureName
					)
					SELECT DISTINCT b.varLine AS varLine, b.varName AS varName
					FROM procedure a 
					JOIN variable b ON CAST(b.varLine AS INT) BETWEEN CAST(a.start AS INT) AND CAST(a.end AS INT) 
					WHERE (procedureName IN (
						SELECT DISTINCT stmtLHS 
						FROM CalledProcedures cp
					) OR procedureName = '" + firstArgumentType + "') AND b.varName = '" + secondArgumentValue + "' AND varCondition IN ('PRINT','if', 'while','RHS')
				)";

		}
		else {  // Modifies (procedureSynonym,_) OR Modifies (procedureSynonym, variable)
			usesSQL = R"(
					WITH RECURSIVE CalledProcedures AS (
						SELECT DISTINCT procedureName, stmtLHS, stmtLine, stmtEntity 
						FROM statement s 
						JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) 
						WHERE procedureName = '" + firstArgumentType + "' AND stmtEntity = 'call'  
						UNION 
						SELECT DISTINCT p.procedureName, s.stmtLHS, s.stmtLine, s.stmtEntity  
						FROM statement s  
						JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT)  
						JOIN CalledProcedures cp ON s.stmtEntity = 'call' AND cp.stmtLHS = p.procedureName
					)
					SELECT DISTINCT stmtLine,varName 
					FROM procedure a 
					JOIN variable b ON CAST(varLine AS INT) BETWEEN CAST(a.start AS INT) AND CAST(a.end AS INT) 
					WHERE procedureName IN (
						SELECT DISTINCT stmtLHS 
						FROM CalledProcedures cp
					) OR procedureName = '" + firstArgumentType + "' AND varCondition IN ('PRINT','if', 'while','RHS')
				)";
		}
	}
	else if (firstArgumentType == "procedure") { // synonym procedure
		if (secondArgumentType == "IDENT") { // Modifies ("main","x") 
			usesSQL = R"(
					WITH RECURSIVE CalledProcedures AS (
						SELECT DISTINCT procedureName, stmtLHS, stmtLine, stmtEntity 
						FROM statement s 
						JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) 
						WHERE stmtEntity = 'call'  
						UNION 
						SELECT DISTINCT p.procedureName, s.stmtLHS, s.stmtLine, s.stmtEntity  
						FROM statement s  
						JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT)  
						JOIN CalledProcedures cp ON s.stmtEntity = 'call' AND cp.stmtLHS = p.procedureName
					)
					SELECT DISTINCT b.varLine, b.varName 
					FROM procedure a 
					JOIN variable b ON CAST(b.varLine AS INT) BETWEEN CAST(a.start AS INT) AND CAST(a.end AS INT) 
					JOIN statement c ON b.varLine = c.stmtLine 
					WHERE (procedureName IN (
						SELECT DISTINCT stmtLHS 
						FROM CalledProcedures cp
					) OR varCondition IN ('PRINT','if', 'while','RHS') AND b.varName = '" + secondArgumentValue + "' AND varName = '" + secondArgumentValue + "'
				)";
		}
		else {  // Modifies ("main",_) OR Modifies ("main", variable)
			usesSQL = R"(
					WITH RECURSIVE CalledProcedures AS (
						SELECT DISTINCT procedureName, stmtLHS, stmtLine, stmtEntity 
						FROM statement s 
						JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT) 
						WHERE stmtEntity = 'call'  
						UNION 
						SELECT DISTINCT p.procedureName, s.stmtLHS, s.stmtLine, s.stmtEntity  
						FROM statement s  
						JOIN procedure p ON CAST(s.stmtLine AS INT) BETWEEN CAST(p.start AS INT) AND CAST(p.end AS INT)  
						JOIN CalledProcedures cp ON s.stmtEntity = 'call' AND cp.stmtLHS = p.procedureName
					)
					SELECT DISTINCT b.varLine, b.varName 
					FROM procedure a 
					JOIN variable b ON CAST(b.varLine AS INT) BETWEEN CAST(a.start AS INT) AND CAST(a.end AS INT) 
					JOIN statement c ON b.varLine = c.stmtLine 
					WHERE (procedureName IN (
						SELECT DISTINCT stmtLHS 
						FROM CalledProcedures cp
					) OR varCondition IN ('PRINT','if', 'while','RHS') AND varName = '" + secondArgumentValue + "'
				)";
		}
	}

	dbResults.clear();
	sqlite3_exec(dbConnection, usesSQL.c_str(), callback, 0, &errorMessage);
	for (vector<string> dbRow : dbResults) {
		results.push_back({ dbRow.at(0) ,dbRow.at(1) });
	}
	dbResults.clear();
	sqlite3_exec(dbConnection, usesSQL2.c_str(), callback, 0, &errorMessage);
	for (vector<string> dbRow : dbResults) {
		results.push_back({ dbRow.at(0) ,dbRow.at(1) });
	}

}

// callback method to put one row of results from the database into the dbResults vector
// This method is called each time a row of results is returned from the database
int Database::callback(void* NotUsed, int argc, char** argv, char** azColName) {
	NotUsed = 0;
	vector<string> dbRow;

	// argc is the number of columns for this row of results
	// argv contains the values for the columns
	// Each value is pushed into a vector.
	for (int i = 0; i < argc; i++) {
		dbRow.push_back(argv[i]);
	}

	// The row is pushed to the vector for storing all rows of results 
	dbResults.push_back(dbRow);

	return 0;
}
