#include "Parser.hpp"


using namespace std;


string Parser::toLowerCase(const string& word){
	string word0 = word;
	transform(word0.cbegin(), word0.cend(), word0.begin(), ::tolower);
	return word0;
}


void Parser::parseCreate(const string& query, map<string, Table>& tables){
	cmatch result;
	regex regAllSyntax(R"([cC][rR][eE][aA][tT][eE]\s+[tT][aA][bB][lL][eE]\s+(\w+)\s*\(([^\)]+)\))");

	Table table = Table();
	table.rowsCountMax = 0;
	TableMetadata tableMetadata = TableMetadata();

	regex_match(query.c_str(), result, regAllSyntax);
	if(result.size() < 1 || result[0].str().size() != query.size()){
		throw string{"ERROR: invalid create request syntax"};
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
        	throw string{"ERROR: only int32 type can use autoincrement attribute"};
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
        		throw string{"ERROR: invalid type, string and bytes must have '[]'"};
        	}
        	try{column.size = stoi(matches[3].str().substr(posOpen, posClose-posOpen));}
        	catch(...){
        		throw string{"ERROR: invalid value in [] scopes"};
        	}
        	
        	if(column.size < 1){
        		throw string{"ERROR: string and bytes must have size > 0"};
        	}
        }
        else{
        	column.hasSize = false;
        }

        
        if(matches[5].matched){

        	if (findAttr(column.attributes, ATTRIBUTE::AUTOINCREMENT)){
        		throw string{"ERROR: autoincrement column can't have default value"};
        	}

        	column.hasDefaultValue = true;
        	if(column.type == TYPES::STRING){
        		if (matches[6].str().size()-2 <= column.size && matches[6].str()[0] == '"' && matches[6].str()[matches[6].str().size()-1] == '"'){
        			column.value = matches[6].str();
        		}
        		else{
        			throw string{"ERROR: invalid default value"};
        		}
        	}
        	else if(column.type == TYPES::BYTES){
        		if (matches[6].str().size() == column.size){
        			column.value = matches[6].str();
        		}
        		else{
        			throw string{"ERROR: invalid size of string of default value"};
        		}
        	}
        	else if(column.type == TYPES::INT32){
        		try{
        			column.value = stoi(matches[6].str());
        		}
        		catch(...){
        			throw string{"ERROR: can't convert default value to int"};
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
        			throw string{"ERROR: can't convert default value to bool"};
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
    	throw string{"ERROR: can't create table, table with this name has already created"};  	
    }
    tables[tableMetadata.tableName] = table;
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
        	throw string{"ERROR: invalid attribute"};
        }
    }
    return attributes;
}

TYPES Parser::parseType(const string& type){
	if(type == "int32"){return TYPES::INT32;}
	if(type == "bool"){return TYPES::BOOL;}
	if(type.substr(0, 5) == "bytes"){return TYPES::BYTES;}
	if(type.substr(0, 6) == "string"){return TYPES::STRING;}

	throw string{"ERROR: invalid column type"};
}


void Parser::parseInsert(const string& query, map<string, Table>& tables){
	cmatch result;
	regex regAllSyntax(R"([iI][nN][sS][eE][rR][tT]\s*(\([^\)]+\))\s*[tT][oO]\s+(\w+))");

	regex_match(query.c_str(), result, regAllSyntax);
	if(result[0].str().size() != query.size()){
		throw string{"ERROR: invalid insert request syntax"};
	}

	string values = result[1].str();
	string tableName = result[2].str();
	if(!tables.count(tableName)){
		throw string{"ERROR: can't find table " + tableName + " in database"};
	}

	parseInsertValues(values, tables[tableName]);
}

void Parser::parseInsertValues(const string& values, Table& table){
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
        	if(flagValuesType == 2){throw string{"ERROR: invalid values array"};}
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
        		throw string{"ERROR: invalid values array"};
        	}
        	flag = true;
	        columnCount++;
        }   
    }

    if(!flag && flagValuesType != 2){
    	insertDefaultValue(newRow.size()-1, newRow, table);
    	columnCount++;
    }

    if(flagValuesType == 2){
    	for(int i = 0; i < newRow.size(); i++){
    		if(holds_alternative<string>(newRow[i]) && get<string>(newRow[i]).size() == 0){
    			insertDefaultValue(i, newRow, table);
    			columnCount++;
    		}
    	}
    }

    if(columnCount != newRow.size()){
    	throw string{"ERROR: invalid values count"};
    }

    table.rows.push_back(newRow);
    table.rowsCountMax++;
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
		throw string{"ERROR: can't find default value"};
	}
	if(column.hasDefaultValue){
		checkUnique(column.value, table, columnNum, newRow);
	}
	else{
		checkUnique(table.rowsCountMax, table, columnNum, newRow);
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
			throw string{"ERROR: this value is already be in unique column"};
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
		throw string{"ERROR: can't find column to insert value"};
	}

	Column column = table.metadata.columnsInfo[index];
	TYPES columnType = column.type;

	if(findAttr(column.attributes, ATTRIBUTE::AUTOINCREMENT)){
		throw string{"ERROR: autoincrement column must be missed"};
	}

	if(columnType == TYPES::STRING){	
		if(value[0] != '"' || value[value.size()-1] != '"' || value.size() > column.size){
			throw string{"ERROR: invalid value"};
		}
		checkUnique(value, table, index, newRow);
	}
	else if(columnType == TYPES::BYTES){
		if(value[0] != '0' || value[1] != 'x' || value.size()-2 != column.size){
			throw string{"ERROR: invalid value"};
		}
		checkUnique(value, table, index, newRow);
	}
	else if(columnType == TYPES::INT32){
		try{
			int val = stoi(value);
			checkUnique(val, table, index, newRow);
		}
		catch(...){
			throw string{"ERROR: invalid value"};
		}
	}
	else{
		if(value == "true"){checkUnique(true, table, index, newRow);}
		else if(value == "false"){checkUnique(false, table, index, newRow);}
		else{
			throw string{"ERROR: invalid value"};
		}
	}
}

void Parser::insertVal(int columnNum, string value, vector<variant<string, int, bool>>& newRow, Table& table){
	int index = columnNum;
	if(index == -1){
		throw string{"ERROR: can't find column to insert value"};
	}

	Column column = table.metadata.columnsInfo[index];
	TYPES columnType = column.type;

	if(findAttr(column.attributes, ATTRIBUTE::AUTOINCREMENT)){
		throw string{"ERROR: autoincrement column must be missed"};
	}

	if(columnType == TYPES::STRING){	
		if(value[0] != '"' || value[value.size()-1] != '"' || value.size() > column.size){
			throw string{"ERROR: invalid value"};
		}
		checkUnique(value, table, index, newRow);
	}
	else if(columnType == TYPES::BYTES){
		if(value[0] != '0' || value[1] != 'x' || value.size()-2 != column.size){
			throw string{"ERROR: invalid value"};
		}
		checkUnique(value, table, index, newRow);
	}
	else if(columnType == TYPES::INT32){
		try{
			int val = stoi(value);
			checkUnique(val, table, index, newRow);
		}
		catch(...){
			throw string{"ERROR: invalid value"};
		}
	}
	else{
		if(value == "true"){checkUnique(true, table, index, newRow);}
		else if(value == "false"){checkUnique(false, table, index, newRow);}
		else{
			throw string{"ERROR: invalid value"};
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
			throw string{"ERROR: invalid operand at last lvl"};
		}
	}

	if(type == -1){
		throw string{"ERROR: invalid operand at last lvl"};
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
				throw string{"ERROR: invalid condition(last lvl, make postfix)"};
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
				throw string{"ERROR: invalid rvalue and lvalue"};
			}
			ans.pop();
			if(!ans.empty()) valL = ans.top();
			else{
				throw string{"ERROR: invalid rvalue and lvalue"};
			}
			ans.pop();

			if(valR.index() != valL.index()){
				throw string{"ERROR: invalid types in condition"};
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
					throw string{"ERROR: can't use operator '+' with bytes"};
				}
				else{
					throw string{"ERROR: invalid operands for '+'"};
				}
				continue;
			}
			if(postfix[i] == "+" && valL.index() == 1) {ans.push(get<int>(valL)+get<int>(valR)); continue;}
			if(postfix[i] == "+" && valL.index() == 2) {ans.push(get<bool>(valL)+get<bool>(valR)); continue;}
			if(postfix[i] == "-" && valL.index() == 1) {ans.push(get<int>(valL)-get<int>(valR)); continue;}
			if(postfix[i] == "%" && valL.index() == 1) {ans.push(get<int>(valL)%get<int>(valR)); continue;}
			if(postfix[i] == "/" && valL.index() == 1) {ans.push(get<int>(valL)/get<int>(valR)); continue;}
			if(postfix[i] == "*" && valL.index() == 1) {ans.push(get<int>(valL)*get<int>(valR)); continue;}
			throw string{"ERROR: invalid rvalue and lvalue types"};
		}

		else{
			if((postfix[i][0] != '"' || postfix[i][postfix[i].size()-1] != '"') && (postfix[i][0] != '0' || postfix[i][1] != 'x') && (!isStringDigit(postfix[i])) && postfix[i] != "true" && postfix[i] != "false"){
				
				if(postfix[i][0] == '|' && postfix[i][postfix[i].size()-1] == '|'){
					int index = findColIndexByName(postfix[i].substr(1, postfix[i].size()-2), table);
					
					if(index == -1){
						throw string{"ERROR: can't find column"};
					}
					if(!table.metadata.columnsInfo[index].hasSize){
						throw string{"ERROR: invalid type of column. Operation size must be at string and bytes"};
					}
					ans.push((int)get<string>(row[index]).size()-2);	
				}

				else{
					
					int index = findColIndexByName(postfix[i], table);
					if(index == -1){
						throw string{"ERROR: can't find column"};
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
		throw string{"ERROR: invalid condition(stack size != 1)"};
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
		throw string{"ERROR: invalid operation"};
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
				throw string{"ERROR: invalid condition(last lvl, make postfix)"};
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
				throw string{"ERROR: invalid rvalue and lvalue"};
			}
			ans.pop();
			if(!ans.empty()) valL = ans.top();
			else{
				throw string{"ERROR: invalid rvalue and lvalue"};
			}
			ans.pop();

			if(valR.index() != valL.index()){
				throw string{"ERROR: invalid types in condition"};
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
					throw string{"ERROR: can't use operator + with bytes"};
				}
				else{
					throw string{"ERROR: invalid operands for +"};
				}
				continue;
			}
			else if(postfix[i] == "+" && valL.index() == 1) {ans.push(get<int>(valL)		+	get<int>(valR)); continue;}
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

			throw string{"ERROR: invalid rvalue and lvalue types"};
		}

		else{
			if((postfix[i][0] != '"' || postfix[i][postfix[i].size()-1] != '"') && (postfix[i][0] != '0' || postfix[i][1] != 'x') && (!isStringDigit(postfix[i])) && postfix[i] != "true" && postfix[i] != "false"){
				
				if(postfix[i][0] == '|' && postfix[i][postfix[i].size()-1] == '|'){
					int index = findColIndexByName(postfix[i].substr(1, postfix[i].size()-2), table);
					
					if(index == -1){
						throw string{"ERROR: can't find column"};
					}
					if(!table.metadata.columnsInfo[index].hasSize){
						throw string{"ERROR: invalid type of column. Operation size must be at string and bytes"};
					}
					ans.push((int)get<string>(row[index]).size()-2);	
				}

				else{
					int index = findColIndexByName(postfix[i], table);
					if(index == -1){
						throw string{"ERROR: can't find column"};
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
		throw string{"ERROR: invalid condition(stack size != 1 or top type isn't bool)"};
	}
	return get<bool>(ans.top());
}





vector<Row> Parser::parseSelect(const string& query, map<string, Table>& tables){
	cmatch result;
	regex reg(R"(([sS][eE][lL][eE][cC][tT])\s+([\w+\s*\,]+)\s+([fF][rR][oO][mM])\s+(\w+)\s+([wW][hH][eE][rR][eE])\s+(.+))");

    regex_match(query.c_str(), result, reg);
    
    if(result.size() != 7){
    	throw string{"ERROR: invalid select syntax"};
    }

    string columns = result[2].str();
    string tableName = result[4].str();
    string condition = result[6].str();

    if(tables.count(tableName) == 0){
    	throw string{"ERROR: can't find table ins select"};
    }

    Table table = tables[tableName];
    vector<int> columnsIndexes = parseColumnsToVecIndex(columns, table);

    vector<vector<variant<string, int, bool>>> ans;
    vector<vector<variant<string, int, bool>>> allRows = table.rows;
    
    vector<Row> ansRows;

    
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
    		ansRows.push_back(row);
    	}
    }
    return ansRows;

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


    vector<int> ansVec;
    for(int i = 0; i < vec.size(); i++){
    	if(i % 2 == 0 && vec[i] == "," || i % 2 == 1 && vec[i] != ","){
    		throw string{"ERROR: invalid select columns syntax(invalid sequency , and columns name)"};
    	}
    	if(i % 2 == 0){
    		int index = findColIndexByName(vec[i], table);
    		if(index == -1){
    			throw string{"ERROR: can't find column for select"};
    		}
    		ansVec.push_back(index);
    	}
    }
    return ansVec;
}


void Parser::parseDelete(const string& query, map<string, Table>& tables){
	cmatch result;
	regex reg(R"(([dD][eE][lL][eE][tT][eE])\s+(\w+)\s+([wW][hH][eE][rR][eE])\s+(.+))");

    regex_match(query.c_str(), result, reg);
    
    if(result.size() != 5){
    	throw string{"ERROR: invalid delete syntax"};
    }

    string tableName = result[2].str();
    string condition = result[4].str();

    if(tables.count(tableName) == 0){
    	throw string{"ERROR: can't find table for delete"};
    }

    Table table = tables[tableName];
    
    vector<vector<variant<string, int, bool>>> ans;
    vector<vector<variant<string, int, bool>>> rows = table.rows;
    for (int i = 0; i < rows.size(); i++){
    	if(!checkCondition(rows[i], condition, table)) ans.push_back(rows[i]); 
    }
    tables[tableName].rows = ans;
}


void Parser::parseUpdate(const string& query, map<string, Table>& tables){
	cmatch result;
	regex reg(R"([uU][pP][dD][aA][tT][eE]\s+(\w+)\s+[sS][eE][tT]\s+([\w\+\s\*\,\=\/\"\-\|]+)\s+[wW][hH][eE][rR][eE]\s+(.+))");

    regex_match(query.c_str(), result, reg);
    
    if(result.size() != 4){
    	throw string{"ERROR: invalid select syntax"};
    }

    string tableName = result[1].str();
    string assignments = result[2].str();
    string condition = result[3].str();

    if(tables.count(tableName) == 0){
    	throw string{"ERROR: can't find table in update"};
    }

    Table table = tables[tableName];

    
    for(int i = 0; i < table.rows.size(); i++){
    	if(checkCondition(table.rows[i], condition, table)){
			vector<pair<int, variant<string, int, bool>>> vecAssignments = parseAssignments(table.rows[i], assignments, table);
			for(int j = 0; j < vecAssignments.size(); j++){
				table.rows[i][vecAssignments[j].first] = vecAssignments[j].second;
			}    		
    	}
    }
    tables[tableName] = table;
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
	    	throw string{"ERROR: can't find column(left value in update)"};
	    }


	    if((findAttr(table.metadata.columnsInfo[index].attributes, ATTRIBUTE::UNIQUE) || findAttr(table.metadata.columnsInfo[index].attributes, ATTRIBUTE::KEY)) && row[index] != rightVal && table.metadata.columnsInfo[index].setValues.count(rightVal) == 1){
	    	throw string{"ERROR: can't update. This value is already in unique column"};
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
	    	throw string{"ERROR: invalid operands type in assignments of update"};
	    }

    }
    return ans;
}