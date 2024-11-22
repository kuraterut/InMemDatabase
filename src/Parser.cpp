#include "Parser.hpp"


using namespace std;


string Parser::toLowerCase(const string& word){
	string word0 = word;
	transform(word0.cbegin(), word0.cend(), word0.begin(), ::tolower);
	return word0;
}


int Parser::parsePost(const string& query, map<string, Table>& tables){
	cmatch result;
	regex reg(R"((\w+))");
	regex_search(query.c_str(), result, reg);		
	if(result.size() < 2){
		cout << "ERROR: invalid query type" << endl;
		exit(-1);	
	}
	string queryType = toLowerCase(result[1].str());
	if(queryType == "create"){return parseCreate(query, tables);}
	if(queryType == "insert"){return parseInsert(query, tables);}
	if(queryType == "delete"){return parseDelete(query, tables);}
	if(queryType == "update"){return parseUpdate(query, tables);}

	cout << "ERROR: invalid query type" << endl;
	exit(-1);
	return -1;
}

ResultSet Parser::parseGet(const string& query, map<string, Table>& tables){
	cmatch result;
	regex reg(R"((\w+))");
	regex_search(query.c_str(), result, reg);		
	if(result.size() < 2){
		cout << "ERROR: invalid query type" << endl;
		exit(-1);	
	}
	string queryType = toLowerCase(result[1].str());
	if(queryType == "select"){return parseSelect(query, tables);}

	cout << "ERROR: invalid query type" << endl;
	exit(-1);
	return ResultSet(false);
}


int Parser::parseCreate(const string& query, map<string, Table>& tables){
	cmatch result;
	regex regAllSyntax(R"([cC][rR][eE][aA][tT][eE]\s+[tT][aA][bB][lL][eE]\s+(\w+)\s*\(([^\)]+)\))");

	Table table = Table();
	TableMetadata tableMetadata = TableMetadata();

	regex_match(query.c_str(), result, regAllSyntax);
	if(result.size() < 1 || result[0].str().size() != query.size()){
		cout << "ERROR: invalid create request syntax" << endl;
		exit(-1);
		return -1;
	}

	tableMetadata.tableName = result[1].str();
	string columnsInfo = result[2].str();

	regex regColumnsInfo(R"((\{[\w, ]+\})?\s*(\w+)\s*:\s*(\w+(\[[\-0-9]*\])?)\s*(=\s*((\w+)|(\"[\w\s\n]+\")))?\s*[,]?\s*)");

	auto begin = sregex_iterator(columnsInfo.begin(), columnsInfo.end(), regColumnsInfo);
    auto end = sregex_iterator();

    for (sregex_iterator i = begin; i != end; ++i) {
        smatch matches = *i;
		
        Column column = Column();

        if(matches[1].matched){
        	column.attributes = parseAttributes(matches[1].str());
        }
        column.columnName = matches[2].str();
        column.type = parseType(toLowerCase(matches[3].str()));
        if(column.type != TYPES::INT32 && findAttr(column.attributes, ATTRIBUTE::AUTOINCREMENT)){
        	cout << "ERROR: only int32 type can use autoincrement attribute" << endl;
        	exit(-1);
        }
        if(findAttr(column.attributes, ATTRIBUTE::KEY) || findAttr(column.attributes, ATTRIBUTE::UNIQUE)){
        	column.isUnique = true;
        }
        else{
        	column.isUnique = false;
        }
        if (column.type == TYPES::STRING || column.type == TYPES::BYTES){
        	column.hasSize = true;
        	int posOpen = matches[3].str().find("[")+1;
        	int posClose = matches[3].str().find("]");
        	if(posOpen == -1 || posClose == -1){
        		cout << "ERROR: invalid type, string and bytes must have '[]'" << endl;
        		exit(-1);
        	}
        	try{column.size = stoi(matches[3].str().substr(posOpen, posClose-posOpen));}
        	catch(...){
        		cout << "ERROR: invalid value in [] scopes" << endl; 
        		exit(-1);
        	}
        	
        	if(column.size < 1){
        		cout << "ERROR: string and bytes must have size > 0" << endl;
        		exit(-1);
        	}
        }
        else{
        	column.hasSize = false;
        }

        
        if(matches[5].matched){

        	if (findAttr(column.attributes, ATTRIBUTE::AUTOINCREMENT)){
        		cout << "ERROR: autoincrement column can't have default value" << endl;
        		exit(-1);
        	}

        	column.hasDefaultValue = true;
        	if(column.type == TYPES::STRING){
        		if (matches[6].str().size()-2 <= column.size && matches[6].str()[0] == '"' && matches[6].str()[matches[6].str().size()-1] == '"'){
        			column.value = matches[6].str();
        		}
        		else{
        			cout << "ERROR: invalid default value" << endl;
        			exit(-1);
        		}
        	}
        	else if(column.type == TYPES::BYTES){
        		if (matches[6].str().size() == column.size){
        			column.value = matches[6].str();
        		}
        		else{
        			cout << "ERROR: invalid size of string of default value" << endl;
        			exit(-1);
        		}
        	}
        	else if(column.type == TYPES::INT32){
        		try{
        			column.value = stoi(matches[6].str());
        		}
        		catch(...){
        			cout << "ERROR: can't convert default value to int" << endl;
        			exit(-1);
        		}
        	}
        	else if(column.type == TYPES::BOOL){
        		if(toLowerCase(matches[6].str()) == "true"){
        			column.value = true;
        		}
        		else if(toLowerCase(matches[6].str()) == "false"){
        			column.value = false;
        		}
        		else{
        			cout << "ERROR: can't convert default value to bool" << endl;
        			exit(-1);
        		}
        	}
        }
        else{
        	column.hasDefaultValue = false;
        }
        tableMetadata.columnsInfo.push_back(column);
    }
    table.metadata = tableMetadata;
    if(tables.count(tableMetadata.tableName)){
    	cout << "ERROR: can't create table, table with this name has already created" << endl;
    	exit(-1);
    	return 0;
    }
    tables[tableMetadata.tableName] = table;
    return 1;
}



vector<ATTRIBUTE> Parser::parseAttributes(const string& attributesStr){
	cmatch result;
	regex regAttr(R"((\w+)\s*,?\s*)");
	vector<ATTRIBUTE> attributes;

	auto begin = sregex_iterator(attributesStr.begin(), attributesStr.end(), regAttr);
    auto end = sregex_iterator();

    for (sregex_iterator i = begin; i != end; ++i) {
        smatch matches = *i;
        string attrLC = toLowerCase(matches[1].str());
        if(attrLC == "key"){attributes.push_back(ATTRIBUTE::KEY);}
        else if(attrLC == "autoincrement"){attributes.push_back(ATTRIBUTE::AUTOINCREMENT);}
        else if(attrLC == "unique"){attributes.push_back(ATTRIBUTE::UNIQUE);}
        else{
        	cout << "ERROR: invalid attribute" << endl;
        	exit(-1);
        }
    }
    return attributes;
}

TYPES Parser::parseType(const string& type){
	if(type == "int32"){return TYPES::INT32;}
	if(type == "bool"){return TYPES::BOOL;}
	if(type.substr(0, 5) == "bytes"){return TYPES::BYTES;}
	if(type.substr(0, 6) == "string"){return TYPES::STRING;}

	cout << "ERROR: invalid column type" << endl;
	exit(-1);
}


int Parser::parseInsert(const string& query, map<string, Table>& tables){
	cmatch result;
	regex regAllSyntax(R"([iI][nN][sS][eE][rR][tT]\s*(\([^\)]+\))\s*[tT][oO]\s+(\w+))");

	regex_match(query.c_str(), result, regAllSyntax);
	if(result[0].str().size() != query.size()){
		cout << "ERROR: invalid insert request syntax" << endl;
		exit(-1);
	}

	string values = result[1].str();
	string tableName = result[2].str();
	if(!tables.count(tableName)){
		cout << "ERROR: can't find table " << tableName << " in database" << endl;
		exit(-1);
	}

	return parseInsertValues(values, tables[tableName]);
}

int Parser::parseInsertValues(const string& values, Table& table){
	cmatch result;
	regex regValueSyntax(R"(\s*(,)\s*|\s*((\w+)\s*=\s*(\w+))\s*|\s*((\w+)\s*=\s*(\"[\w\s\n]+\"))\s*|\s*(\w+)\s*|\s*(\"[\w\s\n]+\")\s*)");

	auto begin = sregex_iterator(values.begin(), values.end(), regValueSyntax);
    auto end = sregex_iterator();

    vector<variant<string, int, bool>> newRow;
    newRow.resize(table.metadata.columnsInfo.size());

    int columnCount = 0;
    bool flag = false;
    int flagValuesType = 0;//0 - undef, 1 - defined val, 2 - defined col = val
    


    for (sregex_iterator i = begin; i != end; ++i) {
        smatch matches = *i;

        if(matches[1].matched && !flag){
        	if(flagValuesType == 0){flagValuesType = 1;}
        	if(flagValuesType == 2){cout << "ERROR: invalid values array" << endl; exit(-1);}
        	insertDefaultValue(columnCount, newRow, table);
        	columnCount++;
        }
        else if(matches[1].matched && flag){
        	flag = false;
        }
        else if(!matches[1].matched){
        	if(matches[2].matched && (flagValuesType == 0 || flagValuesType == 2)){
        		if(flagValuesType == 0){flagValuesType = 2;}
        		insertColVal(matches[3].str(), matches[4].str(), newRow, table);
        	}
        	else if(matches[5].matched && (flagValuesType == 0 || flagValuesType == 2)){
        		if(flagValuesType == 0){flagValuesType = 2;}
        		insertColVal(matches[6].str(), matches[7].str(), newRow, table);
        	}
        	else if(matches[8].matched && (flagValuesType == 0 || flagValuesType == 1)){
        		if(flagValuesType == 0){flagValuesType = 1;}
        		insertVal(columnCount, matches[8].str(), newRow, table);
        	}
        	else if(matches[9].matched && (flagValuesType == 0 || flagValuesType == 1)){
        		if(flagValuesType == 0){flagValuesType = 1;}
        		insertVal(columnCount, matches[9	].str(), newRow, table);
        	}
        	else{
        		cout << "ERROR: invalid values array" << endl;
        		exit(-1);
        	}

        	flag = true;
	        columnCount++;
        }   
    }

    if(flagValuesType == 2){
    	for(int i = 0; i < newRow.size(); i++){
    		if(holds_alternative<string>(newRow[i]) && get<string>(newRow[i]).size() == 0){
    			insertDefaultValue(i, newRow, table);
    		}
    	}
    }


    table.rows.push_back(newRow);
    return 1;
}

bool Parser::findAttr(const vector<ATTRIBUTE>& vec, ATTRIBUTE attr){
	auto result{std::find(begin(vec), end(vec), attr)};
    if (result == end(vec))
        return false;
   	return true;
}

void Parser::insertDefaultValue(int columnNum, vector<variant<string, int, bool>>& newRow, Table& table){
	Column column = table.metadata.columnsInfo[columnNum];
	if(!findAttr(column.attributes, ATTRIBUTE::AUTOINCREMENT) && !column.hasDefaultValue){
		cout << "ERROR: can't find default value" << endl;
		exit(-1);
	}
	if(column.hasDefaultValue){
		checkUnique(column.value, table, columnNum, newRow);
	}
	else{
		if(table.rows.size() == 0){
			checkUnique(0, table, columnNum, newRow);	
		}
		else{
			variant<string, int, bool> val = get<int>(table.rows[table.rows.size()-1][columnNum]) + 1;	
			checkUnique(val, table, columnNum, newRow);
		}
	}
}



void Parser::checkUnique(variant<string, int, bool> val, Table& table, int columnNum, vector<variant<string, int, bool>>& newRow){
	Column column = table.metadata.columnsInfo[columnNum];
	if(column.isUnique){
		if(column.setValues.count(val) == 0){
			newRow[columnNum] = val;
			table.metadata.columnsInfo[columnNum].setValues.insert(val);
		}
		else{
			cout << "ERROR: this value is already be in unique column" << endl;
			exit(-1);
		}
	}	
	else{
		newRow[columnNum] = val;
	}
}



int Parser::findColIndexByName(const string& columnName, Table& table){
	vector<Column> columns = table.metadata.columnsInfo;
	for(int i = 0; i < columns.size(); i++){
		if(columns[i].columnName == columnName) return i;
	}
	return -1;
}

void Parser::insertColVal(string columnName, string value, vector<variant<string, int, bool>>& newRow, Table& table){
	int index = findColIndexByName(columnName, table);
	if(index == -1){
		cout << "ERROR: can't find column to insert value" << endl;
		exit(-1);
	}

	Column column = table.metadata.columnsInfo[index];
	TYPES columnType = column.type;

	if(findAttr(column.attributes, ATTRIBUTE::AUTOINCREMENT)){
		cout << "ERROR: autoincrement column must be missed" << endl;
		exit(-1);
	}

	if(columnType == TYPES::STRING){	
		if(value[0] != '"' || value[value.size()-1] != '"' || value.size() > column.size){
			cout << "ERROR: invalid value" << endl;
			exit(-1);
		}
		checkUnique(value, table, index, newRow);
	}
	else if(columnType == TYPES::BYTES){
		if(value[0] != '0' || value[1] != 'x' || value.size()-2 != column.size){
			cout << "ERROR: invalid value" << endl;
			exit(-1);
		}
		checkUnique(value, table, index, newRow);
	}
	else if(columnType == TYPES::INT32){
		try{
			int val = stoi(value);
			checkUnique(val, table, index, newRow);
		}
		catch(...){
			cout << "ERROR: invalid value" << endl;
			exit(-1);
		}
	}
	else{
		if(value == "true"){checkUnique(true, table, index, newRow);}
		else if(value == "false"){checkUnique(false, table, index, newRow);}
		else{
			cout << "ERROR: invalid value" << endl;
			exit(-1);
		}
	}
}

void Parser::insertVal(int columnNum, string value, vector<variant<string, int, bool>>& newRow, Table& table){
	int index = columnNum;
	if(index == -1){
		cout << "ERROR: can't find column to insert value" << endl;
		exit(-1);
	}

	Column column = table.metadata.columnsInfo[index];
	TYPES columnType = column.type;

	if(findAttr(column.attributes, ATTRIBUTE::AUTOINCREMENT)){
		cout << "ERROR: autoincrement column must be missed" << endl;
		exit(-1);
	}

	if(columnType == TYPES::STRING){	
		if(value[0] != '"' || value[value.size()-1] != '"' || value.size() > column.size){
			cout << "ERROR: invalid value" << endl;
			exit(-1);
		}
		checkUnique(value, table, index, newRow);
	}
	else if(columnType == TYPES::BYTES){
		if(value[0] != '0' || value[1] != 'x' || value.size()-2 != column.size){
			cout << "ERROR: invalid value" << endl;
			exit(-1);
		}
		checkUnique(value, table, index, newRow);
	}
	else if(columnType == TYPES::INT32){
		try{
			int val = stoi(value);
			checkUnique(val, table, index, newRow);
		}
		catch(...){
			cout << "ERROR: invalid value" << endl;
			exit(-1);
		}
	}
	else{
		if(value == "true"){checkUnique(true, table, index, newRow);}
		else if(value == "false"){checkUnique(false, table, index, newRow);}
		else{
			cout << "ERROR: invalid value" << endl;
			exit(-1);
		}
	}
}


variant<string, int, bool> Parser::executeArithmeticCondition(const vector<variant<string, int, bool>>& row, const vector<string>& vec, Table& table){
	stack<string> operations;
	vector<string> postfix;

	int type = -1; // -1-undef, 0-string, 1-bytes, 2-int, 3-bool

	for(int i = 0; i < vec.size(); i++){
		if(isArithmeticOperator(vec[i]) || vec[i] == "(" || vec[i] == ")") continue;
		else if(vec[i].size() > 1 && vec[i][0] == '"' && vec[i][vec[i].size()-1] == '"'){
			type = 0;
			break;
		}
		else if(vec[i].size() > 1 && vec[i][0] == '|' && vec[i][vec[i].size()-1] == '|'){
			type = 2;
			break;
		}
		else if(vec[i].size() > 1 && vec[i][0] == '0' && vec[i][1] == 'x'){
			type = 1;
			break;
		}
		else if(vec[i] == "true" && vec[i] == "false"){
			type = 3;
			break;
		}
		else if(isStringDigit(vec[i])){
			type = 2;
			break;
		}
		else if(findColIndexByName(vec[i], table) != -1){
			int index = findColIndexByName(vec[i], table);
			TYPES typeCur = table.metadata.columnsInfo[index].type;
			if(typeCur == TYPES::BYTES) type = 1;
			if(typeCur == TYPES::STRING) type = 0;
			if(typeCur == TYPES::INT32) type = 2;
			if(typeCur == TYPES::BOOL) type = 3;
		}
		else{
			cout << "ERROR: invalid operand at last lvl" << endl;
			exit(-1);
		}
	}

	if(type == -1){
		cout << "ERROR: invalid operand at last lvl" << endl;
		exit(-1);
	}



	for(int i = 0; i < vec.size(); i++){
		if(vec[i] == "("){
			operations.push(vec[i]);
		}
		else if(vec[i] == ")"){
			while(!operations.empty() && operations.top() != "("){
				postfix.push_back(operations.top());
				operations.pop();
			}
			if(operations.empty()){
				cout << "ERROR: invalid condition(last lvl, make postfix)" << endl;
				exit(-1);
			}
			operations.pop();
		}
		else if(isArithmeticOperator(vec[i])){
			while(!operations.empty() && precedence(operations.top()) >= precedence(vec[i])){
				postfix.push_back(operations.top());
				operations.pop();
			}
			operations.push(vec[i]);
		}
		else{
			postfix.push_back(vec[i]);
		}
	}

	while(!operations.empty()){
		postfix.push_back(operations.top());
		operations.pop();
	}


	stack<variant<string, int, bool>> ans;


	for(int i = 0; i < postfix.size(); i++){
		
		if(isOperator(postfix[i])){
			variant<string, int, bool> valR;
			variant<string, int, bool> valL;
			if(!ans.empty()) valR = ans.top();
			else{
				cout << "ERROR: invalid rvalue and lvalue" << endl;
				exit(-1);
			}
			ans.pop();
			if(!ans.empty()) valL = ans.top();
			else{
				cout << "ERROR: invalid rvalue and lvalue" << endl;
				exit(-1);
			}
			ans.pop();

			if(valR.index() != valL.index()){
				cout << "ERROR: invalid types in condition" << endl;
				exit(-1);
			}



			if(postfix[i] == "+" && valL.index() == 0) {
				
				string strL = get<string>(valL);
				string strR = get<string>(valR);
				if(strL.size() > 1 && strR.size() > 1 && strL[0] == '"' && strR[0] == '"' && strL[strL.size()-1] == '"' && strR[strR.size()-1] == '"'){
					string ansStr = "\"";
					ansStr+=strL.substr(1, strL.size()-2);
					ansStr+=strR.substr(1, strR.size()-2);
					ansStr+="\"";
					ans.push(ansStr);
				}
				else if(strL.size() > 2 && strR.size() > 2 && strL[0] == '0' && strR[0] == '0' && strL[1] == 'x' && strR[1] == 'x'){
					cout << "ERROR: can't use operator + with bytes" << endl;
					exit(-1);
				}
				else{
					cout << "ERROR: invalid operands for +" << endl;
					exit(-1);
				}
				continue;
				}
			if(postfix[i] == "+" && valL.index() == 1) {ans.push(get<int>(valL)+get<int>(valR)); continue;}
			if(postfix[i] == "+" && valL.index() == 2) {ans.push(get<bool>(valL)+get<bool>(valR)); continue;}
			if(postfix[i] == "-" && valL.index() == 1) {ans.push(get<int>(valL)-get<int>(valR)); continue;}
			if(postfix[i] == "%" && valL.index() == 1) {ans.push(get<int>(valL)%get<int>(valR)); continue;}
			if(postfix[i] == "/" && valL.index() == 1) {ans.push(get<int>(valL)/get<int>(valR)); continue;}
			if(postfix[i] == "*" && valL.index() == 1) {ans.push(get<int>(valL)*get<int>(valR)); continue;}
			cout << "ERROR: invalid rvalue and lvalue types" << endl;
			exit(-1);
		}

		else{
			if((postfix[i][0] != '"' || postfix[i][postfix[i].size()-1] != '"') && (postfix[i][0] != '0' || postfix[i][1] != 'x') && (!isStringDigit(postfix[i])) && postfix[i] != "true" && postfix[i] != "false"){
				
				if(postfix[i][0] == '|' && postfix[i][postfix[i].size()-1] == '|'){
					int index = findColIndexByName(postfix[i].substr(1, postfix[i].size()-2), table);
					
					if(index == -1){
						cout << "ERROR: can't find column" << endl;
						exit(-1); 
					}
					if(!table.metadata.columnsInfo[index].hasSize){
						cout << "ERROR: invalid type of column. Operation size must be at string and bytes" << endl;
						exit(-1);
					}
					ans.push((int)get<string>(row[index]).size()-2);	
				}

				else{
					
					int index = findColIndexByName(postfix[i], table);
					if(index == -1){
						cout << "ERROR: can't find column" << endl;
						exit(-1); 
					}
					ans.push(row[index]);
				}
			}
			else if(postfix[i] == "true"){ans.push(true);}
			else if(postfix[i] == "false"){ans.push(false);}
			else if (isStringDigit(postfix[i])){
				ans.push(stoi(postfix[i]));
			}
			else{
				ans.push(postfix[i]);
			}
		}
	}

	

	if(ans.size() != 1){
		cout << "ERROR: invalid condition(stack size != 1)" << endl;
		exit(-1);
	}
	return ans.top();
}





bool Parser::isStringDigit(const string& str){
	for (int i = 0; i < str.size(); i++){
		if(!isdigit(str[i])) return false;
	}
	return true;
}

bool Parser::isArithmeticOperator(const string& op){
	return op == "+" || op == "-" || op == "*" || op == "/" || op == "%";
}

bool Parser::isOperator(const string& op){
	return op == "+" || op == "-" || op == "*" || op == "/" || op == "%" || op == "<" || op == "<=" || op == ">" || op == ">=" || op == "!=" || op == "=" || op == "&&" || op == "||" || op == "^^" || op == "!";
}


int Parser::precedence(const string& op){
	map<string, int> dict;
	dict["&&"] = 2;
	dict["||"] = 1;
	dict["!"] = 1;
	dict["^^"] = 2;
	dict["("] = 0;
	dict[")"] = 0;

	dict["<"] = 3;
	dict[">"] = 3;
	dict["<="] = 3;
	dict[">="] = 3;
	dict["!="] = 3;
	dict["="] = 3;

	dict["+"] = 4;
	dict["-"] = 4;
	dict["*"] = 5;
	dict["/"] = 5;
	dict["%"] = 5;

	if(dict.count(op) == 0){
		cout << "ERROR: invalid operation" << endl;
		exit(-1);
	}
	return dict[op];
}


bool Parser::checkCondition(const vector<variant<string, int, bool>>& row, const string& condition, Table& table){
	regex regAllCondition(R"((\w+|\&\&|\|\||\^\^|\!\=|\!|\/|\*|\+|\-|\<\=|\>\=|\=|\<|\>|\%|\|\w+\||\"[\w\s\n]+\"|\(|\)))");

	auto begin = sregex_iterator(condition.begin(), condition.end(), regAllCondition);
    auto end = sregex_iterator();

    vector<string> vec;
    for (sregex_iterator i = begin; i != end; ++i) {
        smatch matches = *i;
        vec.push_back(matches[1].str());
    }
    stack<string> operations;
	vector<string> postfix;

	for(int i = 0; i < vec.size(); i++){
		if(vec[i] == "("){
			operations.push(vec[i]);
		}
		else if(vec[i] == ")"){
			while(!operations.empty() && operations.top() != "("){
				postfix.push_back(operations.top());
				operations.pop();
			}
			if(operations.empty()){
				cout << "ERROR: invalid condition(last lvl, make postfix)" << endl;
				exit(-1);
			}
			operations.pop();
		}
		else if(isOperator(vec[i])){
			while(!operations.empty() && precedence(operations.top()) >= precedence(vec[i])){
				postfix.push_back(operations.top());
				operations.pop();
			}
			operations.push(vec[i]);
		}
		else{
			postfix.push_back(vec[i]);
		}
	}

	while(!operations.empty()){
		postfix.push_back(operations.top());
		operations.pop();
	}


	stack<variant<string, int, bool>> ans;
	for (int i = 0; i < postfix.size(); i++){
		if(isOperator(postfix[i])){
			variant<string, int, bool> valR;
			variant<string, int, bool> valL;
			if(!ans.empty()) valR = ans.top();
			else{
				cout << "ERROR: invalid rvalue and lvalue" << endl;
				exit(-1);
			}
			ans.pop();
			if(!ans.empty()) valL = ans.top();
			else{
				cout << "ERROR: invalid rvalue and lvalue" << endl;
				exit(-1);
			}
			ans.pop();

			if(valR.index() != valL.index()){
				cout << "ERROR: invalid types in condition" << endl;
				exit(-1);
			}



			if(postfix[i] == "+" && valL.index() == 0) {
				string strL = get<string>(valL);
				string strR = get<string>(valR);
				if(strL.size() > 1 && strR.size() > 1 && strL[0] == '"' && strR[0] == '"' && strL[strL.size()-1] == '"' && strR[strR.size()-1] == '"'){
					string ansStr = "\"";
					ansStr+=strL.substr(1, strL.size()-2);
					ansStr+=strR.substr(1, strR.size()-2);
					ansStr+="\"";
					ans.push(ansStr);
				}
				else if(strL.size() > 2 && strR.size() > 2 && strL[0] == '0' && strR[0] == '0' && strL[1] == 'x' && strR[1] == 'x'){
					cout << "ERROR: can't use operator + with bytes" << endl;
					exit(-1);
				}
				else{
					cout << "ERROR: invalid operands for +" << endl;
					exit(-1);
				}
				continue;
				}
			if(postfix[i] == "+" && valL.index() == 1) 		{ans.push(get<int>(valL)		+	get<int>(valR)); continue;}
			else if(postfix[i] == "-" && valL.index() == 1) {ans.push(get<int>(valL)		-	get<int>(valR)); continue;}
			else if(postfix[i] == "%" && valL.index() == 1) {ans.push(get<int>(valL)		%	get<int>(valR)); continue;}
			else if(postfix[i] == "/" && valL.index() == 1) {ans.push(get<int>(valL)		/	get<int>(valR)); continue;}
			else if(postfix[i] == "*" && valL.index() == 1) {ans.push(get<int>(valL)		*	get<int>(valR)); continue;}
			
			else if(postfix[i] == "<" && valL.index() == 0) {ans.push(get<string>(valL)		<	get<string>(valR)); continue;}
			else if(postfix[i] == "<" && valL.index() == 1) {ans.push(get<int>(valL)		<	get<int>(valR)); continue;}
			else if(postfix[i] == "<" && valL.index() == 2) {ans.push(get<bool>(valL)		<	get<bool>(valR)); continue;}

			else if(postfix[i] == "<=" && valL.index() == 0) {ans.push(get<string>(valL)	<=	get<string>(valR)); continue;}
			else if(postfix[i] == "<=" && valL.index() == 1) {ans.push(get<int>(valL)		<=	get<int>(valR)); continue;}
			else if(postfix[i] == "<=" && valL.index() == 2) {ans.push(get<bool>(valL)		<=	get<bool>(valR)); continue;}

			else if(postfix[i] == ">" && valL.index() == 0) {ans.push(get<string>(valL)		>	get<string>(valR)); continue;}
			else if(postfix[i] == ">" && valL.index() == 1) {ans.push(get<int>(valL)		>	get<int>(valR)); continue;}
			else if(postfix[i] == ">" && valL.index() == 2) {ans.push(get<bool>(valL)		>	get<bool>(valR)); continue;}

			else if(postfix[i] == ">=" && valL.index() == 0) {ans.push(get<string>(valL)	>=	get<string>(valR)); continue;}
			else if(postfix[i] == ">=" && valL.index() == 1) {ans.push(get<int>(valL)		>=	get<int>(valR)); continue;}
			else if(postfix[i] == ">=" && valL.index() == 2) {ans.push(get<bool>(valL)		>=	get<bool>(valR)); continue;}

			else if(postfix[i] == "=" && valL.index() == 0) {ans.push(get<string>(valL)		==	get<string>(valR)); continue;}
			else if(postfix[i] == "=" && valL.index() == 1) {ans.push(get<int>(valL)		==	get<int>(valR)); continue;}
			else if(postfix[i] == "=" && valL.index() == 2) {ans.push(get<bool>(valL)		==	get<bool>(valR)); continue;}

			else if(postfix[i] == "!=" && valL.index() == 0) {ans.push(get<string>(valL)	!=	get<string>(valR)); continue;}
			else if(postfix[i] == "!=" && valL.index() == 1) {ans.push(get<int>(valL)	 	!=	get<int>(valR)); continue;}
			else if(postfix[i] == "!=" && valL.index() == 2) {ans.push(get<bool>(valL)	 	!=	get<bool>(valR)); continue;}

			else if(postfix[i] == "&&" && valL.index() == 2) {ans.push(get<bool>(valL)		&&	get<bool>(valR)); continue;}
			else if(postfix[i] == "||" && valL.index() == 2) {ans.push(get<bool>(valL)		||	get<bool>(valR)); continue;}
			else if(postfix[i] == "^^" && valL.index() == 2) {ans.push(get<bool>(valL)		^	get<bool>(valR)); continue;}

			cout << "ERROR: invalid rvalue and lvalue types" << endl;
			exit(-1);
		}

		else{
			if((postfix[i][0] != '"' || postfix[i][postfix[i].size()-1] != '"') && (postfix[i][0] != '0' || postfix[i][1] != 'x') && (!isStringDigit(postfix[i])) && postfix[i] != "true" && postfix[i] != "false"){
				
				if(postfix[i][0] == '|' && postfix[i][postfix[i].size()-1] == '|'){
					int index = findColIndexByName(postfix[i].substr(1, postfix[i].size()-2), table);
					
					if(index == -1){
						cout << "ERROR: can't find column" << endl;
						exit(-1); 
					}
					if(!table.metadata.columnsInfo[index].hasSize){
						cout << "ERROR: invalid type of column. Operation size must be at string and bytes" << endl;
						exit(-1);
					}
					ans.push((int)get<string>(row[index]).size()-2);	
				}

				else{
					int index = findColIndexByName(postfix[i], table);
					if(index == -1){
						cout << "ERROR: can't find column" << endl;
						exit(-1); 
					}
					ans.push(row[index]);
				}
			}
			else if(postfix[i] == "true"){ans.push(true);}
			else if(postfix[i] == "false"){ans.push(false);}
			else if (isStringDigit(postfix[i])){
				ans.push(stoi(postfix[i]));
			}
			else{
				ans.push(postfix[i]);
			}
		}
	}

	
	if(ans.size() != 1 || ans.top().index() != 2){
		cout << "ERROR: invalid condition(stack size != 1 or top type isn't bool)" << endl;
		exit(-1);
	}
	return get<bool>(ans.top());
}





ResultSet Parser::parseSelect(const string& query, map<string, Table>& tables){
	cmatch result;
	regex reg(R"(([sS][eE][lL][eE][cC][tT])\s+([\w+\s*\,]+)\s+([fF][rR][oO][mM])\s+(\w+)\s+([wW][hH][eE][rR][eE])\s+(.+))");

    regex_match(query.c_str(), result, reg);
    
    if(result.size() != 7){
    	cout << "ERROR: invalid select syntax" << endl;
    	exit(-1);
    }

    string columns = result[2].str();
    string tableName = result[4].str();
    string condition = result[6].str();

    if(tables.count(tableName) == 0){
    	cout << "ERROR: can't find table ins select" << endl;
    	exit(-1);
    }

    Table table = tables[tableName];
    vector<int> columnsIndexes = parseColumnsToVecIndex(columns, table);

    vector<vector<variant<string, int, bool>>> ans;
    vector<vector<variant<string, int, bool>>> allRows = table.rows;
    ResultSet resSet = ResultSet(true);
    for(int i = 0; i < allRows.size(); i++){
    	if(checkCondition(allRows[i], condition, table)){
    		vector<variant<string, int, bool>> ansRow;
    		vector<pair<string, TYPES>> columnsInfo;
    		
    		for(int j = 0; j < columnsIndexes.size(); j++){
    			ansRow.push_back(allRows[i][columnsIndexes[j]]);
    			
    			pair<string, TYPES> columnMetadata;
    			columnMetadata.first = table.metadata.columnsInfo[columnsIndexes[j]].columnName;
    			columnMetadata.second = table.metadata.columnsInfo[columnsIndexes[j]].type;
    			
    			columnsInfo.push_back(columnMetadata);
    		}
    		Row row = Row(ansRow, columnsInfo);
    		resSet.addRow(row);
    	}
    }
    return resSet;

}


vector<int> Parser::parseColumnsToVecIndex(const string& columns, Table& table){
	regex reg(R"((\w+)|(,))");

	auto begin = sregex_iterator(columns.begin(), columns.end(), reg);
    auto end = sregex_iterator();

    vector<string> vec;
    for (sregex_iterator i = begin; i != end; ++i) {
        smatch matches = *i;
        vec.push_back(matches[0]); 
    }

    if(vec[0] == "," || vec[vec.size()-1] == ","){
    	cout << "ERROR: invalid select columns syntax";
    	exit(-1);
    }

    vector<int> ansVec;
    for(int i = 0; i < vec.size(); i++){
    	if(i % 2 == 0 && vec[i] == "," || i % 2 == 1 && vec[i] != ","){
    		cout << "ERROR: invalid select columns syntax(invalid sequency , and columns name)" << endl;
    		exit(-1);
    	}
    	if(i % 2 == 0){
    		int index = findColIndexByName(vec[i], table);
    		if(index == -1){
    			cout << "ERROR: can't find column for select" << endl;
    			exit(-1);
    		}
    		ansVec.push_back(index);
    	}
    }
    return ansVec;
}


int Parser::parseDelete(const string& query, map<string, Table>& tables){
	cmatch result;
	regex reg(R"(([dD][eE][lL][eE][tT][eE])\s+(\w+)\s+([wW][hH][eE][rR][eE])\s+(.+))");

    regex_match(query.c_str(), result, reg);
    
    if(result.size() != 5){
    	cout << "ERROR: invalid delete syntax" << endl;
    	exit(-1);
    }

    string tableName = result[2].str();
    string condition = result[4].str();

    if(tables.count(tableName) == 0){
    	cout << "ERROR: can't find table for delete" << endl;
    	exit(-1);
    }

    Table table = tables[tableName];
    int countForDelete = 0;
    vector<vector<variant<string, int, bool>>> ans;
    vector<vector<variant<string, int, bool>>> rows = table.rows;
    for (int i = 0; i < rows.size(); i++){
    	if(checkCondition(rows[i], condition, table)) countForDelete++;
    	else{ans.push_back(rows[i]);}
    }
    tables[tableName].rows = ans;
    return countForDelete;
}


int Parser::parseUpdate(const string& query, map<string, Table>& tables){
	cmatch result;
	regex reg(R"([uU][pP][dD][aA][tT][eE]\s+(\w+)\s+[sS][eE][tT]\s+([\w\+\s\*\,\=\/\"\-\|]+)\s+[wW][hH][eE][rR][eE]\s+(.+))");

    regex_match(query.c_str(), result, reg);
    
    if(result.size() != 4){
    	cout << "ERROR: invalid select syntax" << endl;
    	exit(-1);
    }

    string tableName = result[1].str();
    string assignments = result[2].str();
    string condition = result[3].str();

    if(tables.count(tableName) == 0){
    	cout << "ERROR: can't find table in update" << endl;
    	exit(-1);
    }

    Table table = tables[tableName];

    int countUpdated = 0;
    for(int i = 0; i < table.rows.size(); i++){
    	if(checkCondition(table.rows[i], condition, table)){
    		countUpdated++;
			vector<pair<int, variant<string, int, bool>>> vecAssignments = parseAssignments(table.rows[i], assignments, table);
			for(int j = 0; j < vecAssignments.size(); j++){
				table.rows[i][vecAssignments[j].first] = vecAssignments[j].second;
			}    		
    	}
    }
    tables[tableName] = table;
    return countUpdated;


}

vector<pair<int, variant<string, int, bool>>> Parser::parseAssignments(const vector<variant<string, int, bool>>& row, const string& assignments, Table& table){
	regex reg(R"((\w+)\s*=\s*([\w\s\+\-\*\/\"\%]+))");
	regex regRight(R"((\+|\-|\*|\|\%|\"\w+\"|\|\w+\||\w+))");
	auto begin = sregex_iterator(assignments.begin(), assignments.end(), reg);
    auto end = sregex_iterator();

    vector<pair<int, variant<string, int, bool>>> ans;

    for (sregex_iterator i = begin; i != end; ++i) {
        smatch matches = *i;
		string columnName = matches[1].str();
		string right = matches[2].str();

		auto beginIn = sregex_iterator(right.begin(), right.end(), regRight);
	    auto endIn = sregex_iterator();

	    vector<string> vecRight;
	    for (sregex_iterator j = beginIn; j != endIn; ++j){
	    	smatch matchesIn = *j;
	    	vecRight.push_back(matchesIn[0].str());
	    }

	    variant<string, int, bool> rightVal = executeArithmeticCondition(row, vecRight, table);
	    int index = findColIndexByName(columnName, table);
	    if(index == -1){
	    	cout << "ERROR: can't find column(left value in update)" << endl;
	    	exit(-1);
	    }

	    TYPES type = table.metadata.columnsInfo[index].type;
	    if((type == TYPES::STRING || type == TYPES::BYTES) && rightVal.index() == 0){
	    	pair<int, variant<string, int, bool>> para;
	    	para.first = index;
	    	para.second = rightVal;
	    	ans.push_back(para);
	    } 
	    else if(type == TYPES::INT32 && rightVal.index() == 1){
	    	pair<int, variant<string, int, bool>> para;
	    	para.first = index;
	    	para.second = rightVal;
	    	ans.push_back(para);
	    } 
	    else if(type == TYPES::BOOL && rightVal.index() == 2){
	    	pair<int, variant<string, int, bool>> para;
	    	para.first = index;
	    	para.second = rightVal;
	    	ans.push_back(para);
	    } 
	    else{
	    	cout << "ERROR: invalid operands type in assignments of update" << endl;
	    	exit(-1);
	    }

    }
    return ans;
}