#pragma once

#include <string>
#include <vector>
#include "sqlite3.h"
#include "ConvertPostfix.h"

using namespace std;

// The Database has to be a static class due to various constraints.
// It is advisable to just add the insert / get functions based on the given examples.
class Database {
public:
	// method to connect to the database and initialize tables in the database
	static void initialize();

	// method to close the database connection
	static void close();

	// method to insert into the database
	static void insertProcedure(string procedureName, string start, string end);
	static void insertStatement(string stmtLine, string stmtEntity,string lhs,string rhs, string postfixRhs);
	static void updateParent(string parent, string stmtLine);
	static void insertConstant(string constValue);
	static void insertVariable(string varName, string varLine, string varCondition);

	// method to get from the database
	static void getProcedures(vector<string>& procedures);
	static void getProceduresFromLine(string stmtLine, string& results);
	static void getConstants(vector<string>& constants);
	static void getVariables(vector<string>& variables);
	static void getStatements(string type, vector<string>& statements);

	static void getParent(bool parentAst, string parent, string parentType, string child, string childType, vector<vector<string>>& results);

	static void getPatternStmt(vector<string> LHSPair, vector<string> RHSPair, vector<vector<string>>& results);
	static void getModifies(vector<vector<string>>& results, string firstArgumentType, string firstArgumentValue, string secondArgumentType, string secondArgumentValue);
	static void getCallsProcedures(vector<string>& calls);
	static void getCalls(bool callsAst, vector<vector<string>>& results, string firstArgumentType, string firstArgumentValue, string secondArgumentType, string secondArgumentValue);
	static void getUses(vector<vector<string>>& results, string firstArgumentType, string firstArgumentValue, string secondArgumentType, string secondArgumentValue);

	static bool isNumber(string text) {
		bool isNum = true;
		for (char c : text) {
			if (!isdigit(c)) {
				isNum = false;
				break;
			}
		}
		return isNum;
	}

	static void printVPResult(vector<pair<string, string>>& result) {
		for (pair <string, string> tempPair : result) {
			string parent = tempPair.first;
			string child = tempPair.second;
			cout << "this is Parent-Child Pair:" << parent << " " << child << endl;
		}
	}


private:
	// the connection pointer to the database
	static sqlite3* dbConnection; 
	// a vector containing the results from the database
	static vector<vector<string>> dbResults; 
	// the error message from the database
	static char* errorMessage;
	// callback method to put one row of results from the database into the dbResults vector
	// This method is called each time a row of results is returned from the database
	static int callback(void* NotUsed, int argc, char** argv, char** azColName);



};

