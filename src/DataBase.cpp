#include "DataBase.hpp"

using namespace std;
using json = nlohmann::json;


bool Database::findAttr(const vector<ATTRIBUTE>& vec, ATTRIBUTE attr){
	auto result{std::find(begin(vec), end(vec), attr)};
    if (result == end(vec))
        return false;
   	return true;
}

bool Database::saveToFile(const string& filePath){
	ofstream fileO(filePath);
	json jsonData;
	for (const auto& [tableName, table] : tables){
        jsonData[tableName]["metadata"]["tableName"] = table.metadata.tableName;
        for(int i = 0; i < table.metadata.columnsInfo.size(); i++){
        	jsonData[tableName]["metadata"]["columnsInfo"][i]["columnName"] = table.metadata.columnsInfo[i].columnName;	
        	jsonData[tableName]["metadata"]["columnsInfo"][i]["isAutoincrement"] = findAttr(table.metadata.columnsInfo[i].attributes, ATTRIBUTE::AUTOINCREMENT);
        	jsonData[tableName]["metadata"]["columnsInfo"][i]["isKey"] = findAttr(table.metadata.columnsInfo[i].attributes, ATTRIBUTE::KEY);
        	jsonData[tableName]["metadata"]["columnsInfo"][i]["isUniqueAttr"] = findAttr(table.metadata.columnsInfo[i].attributes, ATTRIBUTE::UNIQUE);
        	jsonData[tableName]["metadata"]["columnsInfo"][i]["type"] = (int)table.metadata.columnsInfo[i].type;
        	jsonData[tableName]["metadata"]["columnsInfo"][i]["hasSize"] = table.metadata.columnsInfo[i].hasSize;
        	if(table.metadata.columnsInfo[i].hasSize){
        		jsonData[tableName]["metadata"]["columnsInfo"][i]["size"] = table.metadata.columnsInfo[i].size;	
        	}
        	else{
        		jsonData[tableName]["metadata"]["columnsInfo"][i]["size"] = -1;
        	}
        	jsonData[tableName]["metadata"]["columnsInfo"][i]["hasDefaultValue"] = table.metadata.columnsInfo[i].hasDefaultValue;
        	if(table.metadata.columnsInfo[i].hasDefaultValue && table.metadata.columnsInfo[i].type == TYPES::STRING){
        		jsonData[tableName]["metadata"]["columnsInfo"][i]["value"] = get<string>(table.metadata.columnsInfo[i].value);
        	}
        	else if(table.metadata.columnsInfo[i].hasDefaultValue && table.metadata.columnsInfo[i].type == TYPES::BYTES){
        		jsonData[tableName]["metadata"]["columnsInfo"][i]["value"] = get<string>(table.metadata.columnsInfo[i].value);
        	}
        	else if(table.metadata.columnsInfo[i].hasDefaultValue && table.metadata.columnsInfo[i].type == TYPES::INT32){
        		jsonData[tableName]["metadata"]["columnsInfo"][i]["value"] = get<int>(table.metadata.columnsInfo[i].value);
        	}
        	else if(table.metadata.columnsInfo[i].hasDefaultValue && table.metadata.columnsInfo[i].type == TYPES::BOOL){
        		jsonData[tableName]["metadata"]["columnsInfo"][i]["value"] = get<bool>(table.metadata.columnsInfo[i].value);
        	}
        	else{
        		jsonData[tableName]["metadata"]["columnsInfo"][i]["value"] = 0;
        	}
        	jsonData[tableName]["metadata"]["columnsInfo"][i]["isUnique"] = table.metadata.columnsInfo[i].isUnique;
        	if(table.metadata.columnsInfo[i].type == TYPES::STRING || table.metadata.columnsInfo[i].type == TYPES::BYTES){
        		vector<string> set;
        		for(variant<string, int, bool> a: table.metadata.columnsInfo[i].setValues){
        			set.push_back(get<string>(a));
        		}
        		jsonData[tableName]["metadata"]["columnsInfo"][i]["setValues"] = set;
        	}
        	else if(table.metadata.columnsInfo[i].type == TYPES::INT32){
        		vector<int> set;
        		for(variant<string, int, bool> a: table.metadata.columnsInfo[i].setValues){
        			set.push_back(get<int>(a));
        		}
        		jsonData[tableName]["metadata"]["columnsInfo"][i]["setValues"] = set;
        	}
        	else if(table.metadata.columnsInfo[i].type == TYPES::BOOL){
        		vector<bool> set;
        		for(variant<string, int, bool> a: table.metadata.columnsInfo[i].setValues){
        			set.push_back(get<bool>(a));
        		}
        		jsonData[tableName]["metadata"]["columnsInfo"][i]["setValues"] = set;
        	}
        }

        for(int i = 0; i < table.rows.size(); i++){
        	for(int j = 0; j < table.rows[i].size(); j++){
        		if(table.metadata.columnsInfo[j].type == TYPES::BOOL){
	        		jsonData[tableName]["data"][i][j] = get<bool>(table.rows[i][j]);
        		}
        		else if(table.metadata.columnsInfo[j].type == TYPES::INT32){
	        		jsonData[tableName]["data"][i][j] = get<int>(table.rows[i][j]);
        		}
        		else{
	        		jsonData[tableName]["data"][i][j] = get<string>(table.rows[i][j]);
        		}
        	}
        	
        }
	}
	fileO << jsonData;
	return true;
}


Database Database::loadFromFile(const string& filePath){
	Database database = Database();
	map<string, Table> tablesMap;

	string jsonToParse;
	ifstream file(filePath);
	file >> jsonToParse;
	
	auto jsonData = json::parse(jsonToParse);

	for (auto tables : jsonData) {
		Table table = Table();
		TableMetadata tableMetadata = TableMetadata();

		vector<Column> columns;

		string tableName = tables["metadata"]["tableName"];
		for(int i = 0; i < tables["metadata"]["columnsInfo"].size(); i++){
			Column column = Column();
			column.columnName = tables["metadata"]["columnsInfo"][i]["columnName"];
			if(tables["metadata"]["columnsInfo"][i]["isAutoincrement"]) column.attributes.push_back(ATTRIBUTE::AUTOINCREMENT);
			if(tables["metadata"]["columnsInfo"][i]["isKey"]) column.attributes.push_back(ATTRIBUTE::KEY);
			if(tables["metadata"]["columnsInfo"][i]["isUniqueAttr"]) column.attributes.push_back(ATTRIBUTE::UNIQUE);
			column.type = (TYPES)tables["metadata"]["columnsInfo"][i]["type"];
			column.hasSize = tables["metadata"]["columnsInfo"][i]["hasSize"];
			if(column.hasSize) column.size = tables["metadata"]["columnsInfo"][i]["size"];
			column.hasDefaultValue = tables["metadata"]["columnsInfo"][i]["hasDefaultValue"];
			if(column.hasDefaultValue) {
				if(column.type == TYPES::STRING || column.type == TYPES::BYTES){
					string val = tables["metadata"]["columnsInfo"][i]["value"];
					column.value = val;
				}
				else if(column.type == TYPES::INT32){
					int val = tables["metadata"]["columnsInfo"][i]["value"];
					column.value = val;

				}
				else if(column.type == TYPES::BOOL){
					bool val = tables["metadata"]["columnsInfo"][i]["value"];
					column.value = val;
				}
			}
			column.isUnique = tables["metadata"]["columnsInfo"][i]["isUnique"];
			
			if(column.isUnique){
				for(int j = 0; j < tables["metadata"]["columnsInfo"][i]["setValues"].size(); j++){
					if(column.type == TYPES::STRING || column.type == TYPES::BYTES){
						string val = tables["metadata"]["columnsInfo"][i]["setValues"][j];
						column.setValues.insert(val);
					}
					else if(column.type == TYPES::INT32){
						int val = tables["metadata"]["columnsInfo"][i]["setValues"][j];
						column.setValues.insert(val);

					}
					else if(column.type == TYPES::BOOL){
						bool val = tables["metadata"]["columnsInfo"][i]["setValues"][j];
						column.setValues.insert(val);
					}
				}
			}
			columns.push_back(column);
		}
		tableMetadata.tableName = tableName;
		tableMetadata.columnsInfo = columns;

		vector<vector<variant<string, int, bool>>> rows;
		
		for(int i = 0; i < tables["data"].size(); i++){
			vector<variant<string, int, bool>> row;
			for(int j = 0; j < tables["data"][i].size(); j++){
				if(columns[j].type == TYPES::STRING || columns[j].type == TYPES::BYTES){
					string val = tables["data"][i][j];
					row.push_back(val);
				}
				else if(columns[j].type == TYPES::INT32){
					int val = tables["data"][i][j];
					row.push_back(val);

				}
				else if(columns[j].type == TYPES::BOOL){
					bool val = tables["data"][i][j];
					row.push_back(val);
				}

				
			}
			rows.push_back(row);
		}
		table.metadata = tableMetadata;
		table.rows = rows;
		database.tables[tableName] = table;
	}
	return database;
}

string Database::toLowerCase(const string& word){
	string word0 = word;
	transform(word0.cbegin(), word0.cend(), word0.begin(), ::tolower);
	return word0;
}


ResultSet Database::execute(const string& query){
	ResultSet result;
	Parser parser = Parser();
	cmatch resultMatch;
	regex reg(R"((\w+))");
	regex_search(query.c_str(), resultMatch, reg);		
	if(resultMatch.size() < 2){
		result = ResultSet("ERROR: Invalid query type", false);
		return result;	
	}

	string queryType = toLowerCase(resultMatch[1].str());
	if(queryType == "select"){
		try{
			result = ResultSet(parser.parseSelect(query, tables), true);
		}
		catch(const string& errMes){
			result = ResultSet(errMes, false);
		}
	}
	else if(queryType == "create"){
		try{
			parser.parseCreate(query, tables);
			result = ResultSet(true);
		}
		catch(const string& errMes){
			result = ResultSet(errMes, false);
		}

	}
	else if(queryType == "insert"){
		try{
			parser.parseInsert(query, tables);
			result = ResultSet(true);
		}
		catch(const string& errMes){
			result = ResultSet(errMes, false);
		}
	}
	else if(queryType == "delete"){
		try{
			parser.parseDelete(query, tables);
			result = ResultSet(true);	
		}
		catch(const string& errMes){
			result = ResultSet(errMes, false);
		}
	}
	else if(queryType == "update"){
		try{
			parser.parseUpdate(query, tables);
			result = ResultSet(true);	
		}
		catch(const string& errMes){
			result = ResultSet(errMes, false);
		}
	}
	else{
		result = ResultSet("ERROR: Invalid query type", false);
	}
	
	return result;
}