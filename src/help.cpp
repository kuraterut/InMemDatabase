// #include <iostream>
// #include <string>
// #include <regex>
// #include <variant>
// #include <vector>
// #include <set>
// #include <map>
// #include <stack>
// #include <cctype>

// using namespace std;


// bool isMainBoolOperator(string op){
// 	return op == "&&" || op == "||" || op == "^^" || op == "!";
// }

// bool isBoolOperator(string op){
// 	return op == "<" || op == "<=" || op == ">" || op == ">=" || op == "!=" || op == "=";
// }

// bool isOperator(string op){
// 	return op == "+" || op == "-" || op == "*" || op == "/" || op == "%";
// }


// int precedence(string op){
// 	map<string, int> dict;
// 	dict["&&"] = 2;
// 	dict["||"] = 1;
// 	dict["!"] = 1;
// 	dict["^^"] = 2;
// 	dict["("] = 0;
// 	dict[")"] = 0;

// 	dict["+"] = 1;
// 	dict["-"] = 1;
// 	dict["*"] = 2;
// 	dict["/"] = 2;
// 	dict["%"] = 2;

// 	if(dict.count(op) == 0){
// 		cout << "ERROR: invalid operation" << endl;
// 		exit(-1);
// 	}
// 	return dict[op];
// }

// bool isStringDigit(string str){
// 	for (int i = 0; i < str.size(); i++){
// 		if(!isdigit(str[i])) return false;
// 	}
// 	return true;
// }


// int findColumnByName(string name, Table& table){
// 	vector<Column> columns = table.metadata.columnsInfo;
// 	for (int i = 0; columns.size(); i++){
// 		if (columns[i].columnName == name) return i;
// 	}
// 	return -1;
// }

// variant<string, int, bool> executeCondition(vector<variant<string, int, bool>> row, vector<string> vec, Table& table){
// 	stack<string> operations;
// 	vector<string> postfix;

// 	int type = -1; // -1-undef, 0-string, 1-bytes, 2-int, 3-bool

// 	for(int i = 0; i < vec.size(); i++){
// 		if(isOperator(vec[i]) || vec[i] == "(" || vec[i] == ")") continue;
// 		else if(vec[i].size() > 1 && vec[i][0] == '"' && vec[i][vec[i].size()-1] == '"'){
// 			type = 0;
// 			break;
// 		}
// 		else if(vec[i].size() > 1 && vec[i][0] == '0' && vec[i][1] == 'x'){
// 			type = 1;
// 			break;
// 		}
// 		else if(vec[i] == "true" && vec[i] == "false"){
// 			type = 3;
// 			break;
// 		}
// 		else if(isStringDigit(vec[i])){
// 			type = 2;
// 			break;
// 		}
// 		else if(findColumnByName(vec[i], table) != -1){
// 			int index = findColumnByName(vec[i], table);
// 			TYPES typeCur = table.metadata.columnsInfo[index].type;
// 			if(typeCur == TYPES::BYTES) type = 1;
// 			if(typeCur == TYPES::STRING) type = 0;
// 			if(typeCur == TYPES::INT32) type = 2;
// 			if(typeCur == TYPES::BOOL) type = 3;
// 		}
// 		else{
// 			cout << "ERROR: invalid operand at last lvl" << endl;
// 			exit(-1);
// 		}
// 	}

// 	if(type == -1){
// 		cout << "ERROR: invalid operand at last lvl" << endl;
// 		exit(-1);
// 	}



// 	for(int i = 0; i < vec.size(); i++){
// 		if(vec[i] == "("){
// 			operations.push(vec[i]);
// 		}
// 		else if(vec[i] == ")"){
// 			while(!operations.empty() && operations.top() != "("){
// 				postfix.push_back(operations.top());
// 				operations.pop();
// 			}
// 			if(operations.empty()){
// 				cout << "ERROR: invalid condition(last lvl, make postfix)" << endl;
// 				exit(-1);
// 			}
// 			operations.pop();
// 		}
// 		else if(isOperator(vec[i])){
// 			while(!operations.empty() && precedence(operations.top()) >= precedence(vec[i])){
// 				postfix.push_back(operations.top());
// 				operations.pop();
// 			}
// 			operations.push(vec[i]);
// 		}
// 		else{
// 			postfix.push_back(vec[i]);
// 		}
// 	}

// 	while(!operations.empty()){
// 		postfix.push_back(operations.top());
// 		operations.pop();
// 	}


// 	stack<variant<string, int, bool>> ans;

// 	for(int i = 0; postfix.size(); i++){
// 		if(isOperator(postfix[i])){
// 			variant<string, int, bool> valR;
// 			variant<string, int, bool> valL;
// 			if(!ans.empty()) valR = ans.top();
// 			else{
// 				cout << "ERROR: invalid rvalue and lvalue" << endl;
// 				exit(-1);
// 			}
// 			ans.pop();
// 			if(!ans.empty()) valL = ans.top();
// 			else{
// 				cout << "ERROR: invalid rvalue and lvalue" << endl;
// 				exit(-1);
// 			}
// 			ans.pop();

// 			if(valR.index() != valL.index()){
// 				cout << "ERROR: invalid types in condition" << endl;
// 				exit(-1);
// 			}

// 			if(postfix[i] == "+") ans.push(valL+valR); continue;
// 			if(postfix[i] == "-" && valL.index() == 1) ans.push(valL-valR); continue;
// 			if(postfix[i] == "%" && valL.index() == 1) ans.push(valL%valR); continue;
// 			if(postfix[i] == "/" && valL.index() == 1) ans.push(valL/valR); continue;
// 			if(postfix[i] == "*" && valL.index() == 1) ans.push(valL*valR); continue;
// 			cout << "ERROR: invalid rvalue and lvalue types" << endl;
// 			exit(-1);
// 		}

// 		else{
// 			if((postfix[i][0] != '"' || postfix[i][postfix[i].size()-1] != '"') && (postfix[i][0] != '0' || postfix[i][1] != 'x') && (!isStringDigit(postfix[i])) && postfix[i] != "true" && postfix[i] != "false"){
// 				int index = findColumnByName(postfix[i], table);
// 				if(index == -1){
// 					cout << "ERROR: can't find column" << endl;
// 					exit(-1); 
// 				}
// 				ans.push(row[index]);
// 			}
// 			else{
// 				ans.push(postfix[i]);
// 			}
// 		}
// 	}

// 	if(ans.size() != 1){
// 		cout << "ERROR: invalid condition(stack size != 1)" << endl;
// 		exit(-1);
// 	}
// 	return ans.top();
// }



// bool executeBoolCondition(vector<variant<string, int, bool>> row, string condition, Table& table){
// 	cmatch result
// 	regex reg(R"((\w+|\!\=|\!|\/|\*|\+|\<\=|\>\=|\=|\<|\>|\%|\|\w+\||\"[\w\s\n]+\"|\(|\)))");

// 	auto begin = sregex_iterator(condition.begin(), condition.end(), reg);
//     auto end = sregex_iterator();

//     vector<string> vecConditionLeft;
//     vector<string> vecConditionRight;
//     string operat;
//     int flag = 0; // 0 - left, 1 - right
//     for (sregex_iterator i = begin; i != end; ++i) {
//         smatch matches = *i;
//         if(isBoolOperator(matches[1].str())){
//         	if(flag == 1){
//         		cout << "ERROR: invalid condition syntax(second lvl)" << endl;
//         		exit(-1);
//         	}
//         	operat = matches[1].str();
//         	flag = 1;
//         }
//         else{
//         	if(flag == 0){
//         		vecConditionLeft.push_back(matches[1].str());
//         	}
//         	else{
//         		vecConditionRight.push_back(matches[1].str());
//         	}
//         }
//     }

//     if(flag == 0 && vecConditionLeft.size() == 1){
//     	if(vecCondition[0] == "true") return true;
//     	if(vecCondition[0] == "false") return false; 
//     }

//     if(flag == 0){
//     	cout << "ERROR: bool operator must have left and right values(or only true and false)" << endl;
//     	exit(-1);
//     }

//     variant<string, int, bool> varL = executeCondition(row, vecConditionLeft, table);
//     variant<string, int, bool> varR = executeCondition(row, vecConditionRight, table);
// 	if (varL.index() != varR.index()){
// 		cout << "ERROR: bool operands must have one type" << endl;
// 		exit(-1);
// 	}
// 	if(operat == "=") return varL == varR;
//     if(operat == "<=") return varL <= varR;
//     if(operat == ">=") return varL >= varR;
//     if(operat == "!=") return varL != varR;
//     if(operat == "<") return varL < varR;
//     if(operat == ">") return varL > varR;
// }





// int main(){

// 	string str = "(name0 : type0 = value0, {attr1, attr2} name1 : type1)";
// 	string str2 = "create table lol ({attr1, attr2} name0 : type0, name1 : type1)";
// 	string str3 = "insert (,asf,afsfaf,asfasfsf)to lol";
// 	string str4 = "(  ,  ,    lol  ,   ,   kek, haha = 7)";
// 	string str5 = "id + 7 = user_id || (|name| + 7 = (9+|kek|)*3 )&& 72+3 >1";

// 	cmatch result;
// 	regex regular(R"((\{[\w, ]+\})?\s*(\w+)\s*:\s*(\w+)\s*(=\s*(\w+))?\s*[,]?\s*)");
// 	regex regular1(R"(((\{[\w, ]+\})?\s*(\w+)\s*:\s*(\w+)\s*(=\s*(\w+))?\s*[,]?\s*)+)");
// 	regex regular2(R"((create)\s+(table)\s+(\w+)\s*\(((\{[\w, ]+\})?\s*(\w+)\s*:\s*(\w+)\s*(=\s*(\w+))?\s*[,]?\s*)+\))");
// 	regex regular3(R"([iI][nN][sS][eE][rR][tT]\s*(\([^\)]+\))\s*[tT][oO]\s+(\w+))");
// 	regex regular4(R"(\s*(,)\s*|\s*((\w+)\s*=\s*(\w+))\s*|\s*(\w+)\s*)");
// 	regex regular5(R"((\w+|\&\&|\|\||\^\^|\!\=|\!|\/|\*|\+|\<\=|\>\=|\=|\<|\>|\%|\|\w+\||\"[\w\s\n]+\"|\(|\)))");


// 	std::string input = "Hello\nWorld\nThis is a test.\n";

//     // Регулярное выражение для поиска символа новой строки
//     std::regex newline_regex(R"(([\w\s\n]+))");

//     // Заменяем символы новой строки на пробелы
//     // std::string output = std::regex_replace(input, newline_regex, " ");

//     // Выводим результат
//     // std::cout << "Original string:\n" << input;
//     // std::cout << "Modified string:\n" << output;


// 	//create table lol ()
// 	auto begin = sregex_iterator(str5.begin(), str5.end(), regular5);
//     auto end = sregex_iterator();

//     vector<string> vecCondition;
//     for (sregex_iterator i = begin; i != end; ++i) {
//         smatch matches = *i;
//         vecCondition.push_back(matches[1].str());
//     }

//     stack<string> operations;
//     string nonOperator;
//     vector<string> postfixVector;

//     if(vecCondition[0] == "("){
//     	operations.push("(");
//     }
//     else{
//     	nonOperator += vecCondition[0] + " ";
//     }

//     for(int i = 1; i < vecCondition.size(); i++){
//     	// cout << vecCondition[i] << endl;

//     	if(vecCondition[i] == "(" && isMainBoolOperator(vecCondition[i-1])){
//     		if(nonOperator!="")postfixVector.push_back(nonOperator);
//     		nonOperator = "";
//     		operations.push("(");
//     	}
//     	else if(vecCondition[i] == ")" && (i == vecCondition.size()-1 || isMainBoolOperator(vecCondition[i+1]))){
//     		if(nonOperator!="")postfixVector.push_back(nonOperator);
//     		nonOperator = "";
//     		while(!operations.empty() && operations.top() != "("){
//     			postfixVector.push_back(operations.top());
//     			operations.pop();
//     		}
//     		if(operations.empty()){
//     			cout << "ERROR: invalid condition syntax" << endl;
//     			exit(-1);
//     		}
//     		operations.pop();
//     	}
//     	else if(isMainBoolOperator(vecCondition[i])){
//     		if(nonOperator!="")postfixVector.push_back(nonOperator);
//     		nonOperator = "";
			
//     		while(!operations.empty() && precedence(operations.top()) >= precedence(vecCondition[i])){
//     			postfixVector.push_back(operations.top());
//     			operations.pop();
//     		}
//     		operations.push(vecCondition[i]);
//     	}
//     	else{
//     		nonOperator += vecCondition[i] + " ";
//     	}
//     }

//     if(nonOperator != ""){postfixVector.push_back(nonOperator);}
//     while(!operations.empty()){
//     	postfixVector.push_back(operations.top());
// 		operations.pop();
//     }


//     for(int i = 0; i < postfixVector.size(); i++){
//     	cout << postfixVector[i] << endl;
//     }


//     vector<string> mainPostfixVectorReady;
//     for (int i = 0; i < postfixVector.size(); i++){
//     	if(isMainBoolOperator(postfixVector[i])){
//     		mainPostfixVectorReady.push_back(postfixVector[i]);
//     	}
//     	else{
//     		bool k = executeBoolCondition(postfixVector[i], table);
//     		if(k) mainPostfixVectorReady.push_back("1");
//     		else mainPostfixVectorReady.push_back("0");
//     	}
//     }


// 	// regex_match(input.c_str(), result, newline_regex);
	
// 	// for(int i = 0; i < result.size(); i++){
// 	// 	cout << result[i] << endl;
// 	// }

// 	// cout << (result[0].str().size() == str2.size()) << endl;
	
// 	// vector<variant<int, bool, string>> vec;
// 	// vec.resize(5);
// 	// variant<int, bool, string> a;
// 	// // vec[0] = 1;
// 	// cout << holds_alternative<int>(a) << endl;


// 	// set<variant<int, bool, string>> s;
// 	// variant<int, bool, string> var1 = "asfa";
// 	// variant<int, bool, string> var2 = "asfa";
// 	// s.insert(var1);
// 	// cout << s.count(var2) << endl;
// 	return 0;
// }