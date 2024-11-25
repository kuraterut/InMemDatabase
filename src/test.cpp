#include <iostream>
#include <ctime>
#include <typeinfo>
#include "DataBase.hpp"
#include "test_system.hpp"

// //------------------------------------------
// // Creating and Assignment testing functions
// //------------------------------------------

using namespace TestSystem;


bool test_create_empty(){    
    Database db = Database();

    string create = "create table table1 ()";
    ResultSet result = db.execute(create);
    if(result.isOk()) return FAIL;

    return OK;
}

bool test_create_autoincrement(){
    Database db = Database();

    string create = "create table table1 ({autoincrement} autoincrCol : int32)";

    string insert1 = "insert () to table1";
    string insert2 = "insert () to table1";

    string select = "select autoincrCol from table1 where true";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;

    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;

    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;

    result = db.execute(select);
    if(!result.isOk()) return FAIL; 

    std::vector<int> ans;
    for(auto elem : result){
        ans.push_back(elem.getInt("autoincrCol")); 
    }
    if(ans[0] != 0 || ans[1] != 1) return FAIL;
    
    return OK;
}

bool test_create_autoincrement_notOnIntERR(){
    Database db = Database();

    string create = "create table table1 ({autoincrement} autoincrCol : bool)";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return OK;

    return FAIL;
}

bool test_create_key_checkUniqueERR(){
    Database db = Database();

    string create = "create table table1 ({key} keyCol : int32)";

    string insert1 = "insert (1) to table1";
    string insert2 = "insert (2) to table1";
    string insert3 = "insert (1) to table1";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;

    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;

    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;

    result = db.execute(insert3);
    if(!result.isOk()) return OK;


    return FAIL;
}


bool test_create_unique_checkUniqueERR(){
    Database db = Database();

    string create = "create table table1 ({unique} keyCol : int32)";

    string insert1 = "insert (1) to table1";
    string insert2 = "insert (2) to table1";
    string insert3 = "insert (1) to table1";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;

    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;

    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;

    result = db.execute(insert3);
    if(!result.isOk()) return OK;


    return FAIL;
}


bool test_create_autoincrementWithDefaultValue(){
    Database db = Database();

    string create = "create table table1 ({autoincrement} autoincrCol : int32 = 12)";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return OK;

    return FAIL;
}


bool test_create_intDefaultValue(){
    Database db = Database();

    string create = "create table table1 (intCol : int32 = 12)";

    string insert1 = "insert () to table1";
    string insert2 = "insert (2) to table1";

    string select = "select intCol from table1 where true";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;

    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;

    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;

    result = db.execute(select);
    if(!result.isOk()) return FAIL;

    std::vector<int> ans;
    for(auto elem: result){
        ans.push_back(elem.getInt("intCol"));
    }

    if(ans[0] == 12 && ans[1] == 2) return OK;

    return FAIL;
}


bool test_create_intDefaultValue_notIntERR(){
    Database db = Database();

    string create = "create table table1 (intCol : int32 = true)";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return OK;

    return FAIL;
}



bool test_create_boolDefaultValue(){
    Database db = Database();

    string create = "create table table1 (boolCol : bool = true)";

    string insert1 = "insert () to table1";
    string insert2 = "insert (false) to table1";

    string select = "select boolCol from table1 where true";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;

    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;

    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;

    result = db.execute(select);
    if(!result.isOk()) return FAIL;

    std::vector<bool> ans;
    for(auto elem: result){
        ans.push_back(elem.getBool("boolCol"));
    }

    if(ans[0] == true && ans[1] == false) return OK;

    return FAIL;
}


bool test_create_boolDefaultValue_notBoolERR(){
    Database db = Database();

    string create = "create table table1 (boolCol : bool = 12)";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return OK;

    return FAIL;
}





bool test_create_stringDefaultValue(){
    Database db = Database();

    string create = "create table table1 (stringCol : string[10] = \"default\")";

    string insert1 = "insert () to table1";
    string insert2 = "insert (\"not def\") to table1";

    string select = "select stringCol from table1 where true";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;

    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;

    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;

    result = db.execute(select);
    if(!result.isOk()) return FAIL;

    std::vector<std::string> ans;
    for(auto elem: result){
        ans.push_back(elem.getString("stringCol"));
    }

    if(ans[0] == "\"default\"" && ans[1] == "\"not def\"") return OK;

    return FAIL;
}


bool test_create_stringDefaultValue_notStringERR(){
    Database db = Database();

    string create = "create table table1 (stringCol : string[10] = 12)";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return OK;

    return FAIL;
}

bool test_create_stringDefaultValue_stringLongerERR(){
    Database db = Database();

    string create = "create table table1 (stringCol : string[3] = \"more\")";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return OK;

    return FAIL;
}



bool test_create_bytesDefaultValue(){
    Database db = Database();

    string create = "create table table1 (bytesCol : bytes[5] = 0x12F45)";

    string insert1 = "insert () to table1";
    string insert2 = "insert (0x32C55) to table1";

    string select = "select bytesCol from table1 where true";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;

    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;

    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;

    result = db.execute(select);
    if(!result.isOk()) return FAIL;

    std::vector<std::string> ans;
    for(auto elem: result){
        ans.push_back(elem.getBytes("bytesCol"));
    }

    if(ans[0] == "0x12F45" && ans[1] == "0x32C55") return OK;

    return FAIL;
}


bool test_create_bytesDefaultValue_notBytesERR(){
    Database db = Database();

    string create = "create table table1 (bytesCol : bytes[3] = 12)";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return OK;

    return FAIL;
}

bool test_create_bytesDefaultValue_BytesLenNotEqualERR(){
    Database db = Database();

    string create = "create table table1 (bytesCol : bytes[3] = 0x12F45)";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return OK;

    return FAIL;
}


bool test_create_columnsWithOneNameERR(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col1 : bool)";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return OK;

    return FAIL;
}

bool test_create_tablesWithOneNameERR(){
    Database db = Database();

    string create1 = "create table table1 (col1 : int32, col2 : bool)";
    string create2 = "create table table1 (col3 : int32, col4 : bool)";
    ResultSet result;

    result = db.execute(create1);
    if(!result.isOk()) return FAIL;

    result = db.execute(create2);
    if(!result.isOk()) return OK;

    return FAIL;
}

bool test_create_incorrectAttributeERR(){
    Database db = Database();

    string create = "create table table1 ({notattribute} col1 : int32, col2 : bool)";
    
    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return OK;

    
    return FAIL;
}

bool test_create_incorrectTypeERR(){
    Database db = Database();

    string create = "create table table1 (col1 : int64, col2 : bool)";
    
    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return OK;

    
    return FAIL;
}


bool test_create_incorrectColumnInfoSyntaxERR(){
    Database db = Database();

    string create1 = "create table table1 (col1 : , col2 : bool)";
    string create2 = "create table table2 ( : int64, col2 : bool)";
    string create3 = "create table table3 (col1 int64, col2 : bool)";

    ResultSet result1, result2, result3;

    result1 = db.execute(create1);
    result2 = db.execute(create2);
    result3 = db.execute(create3);
    if(!result1.isOk() && !result2.isOk() && !result3.isOk()) return OK;
    
    return FAIL;
}
// //---------------
// // Insert working
// //---------------

bool test_insert_columnEqualValue(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool)";
    
    string insert = "insert (col2 = true, col1 = 12) to table1";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert);
    if(!result.isOk()) return FAIL;
    
    return OK;
}

bool test_insert_onlyValue(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert = "insert (30, false) to table1";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert);
    if(!result.isOk()) return FAIL;
    
    return OK;
}

bool test_insert_defaultValue(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert = "insert (30,) to table1";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert);
    if(!result.isOk()) return FAIL;
    
    return OK;
}

bool test_insert_defaultValueERR(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert = "insert (,) to table1";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_insert_valueTypeERR(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert = "insert (true, 32) to table1";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_insert_stringIncorrectLenERR(){
    Database db = Database();

    string create = "create table table1 (col1 : string[3], col2 : bool = true)";
    
    string insert = "insert (\"hahaha\", false) to table1";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert);
    if(!result.isOk()) return OK;
    
    return FAIL;
}


bool test_insert_bytesIncorrectLenERR(){
    Database db = Database();

    string create = "create table table1 (col1 : bytes[3], col2 : bool = true)";
    
    string insert = "insert (\"0x12F45\", false) to table1";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_insert_invalidTableNameERR(){
    Database db = Database();

    string create = "create table table1 (col1 : bytes[3], col2 : bool = true)";
    
    string insert = "insert (\"0x12F45\", false) to table2";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_insert_valuesMoreThanColumnsERR(){
    Database db = Database();

    string create = "create table table1 (col1 : bytes[3], col2 : bool = true)";
    
    string insert = "insert (\"0x12F45\", false, 12) to table1";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_insert_uniqueValues(){
    Database db = Database();

    string create = "create table table1 ({unique} col1 : int32, col2 : bool = true)";
    
    string insert = "insert (12,) to table1";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_insert_noColumnsForEqualERR(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert = "insert (col1 = 32, col3 = 15) to table1";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

// //--------------
// // Delete tests
// //--------------

bool test_delete_incorrectTableName(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert = "insert (col1 = 32, col2 = false) to table1";

    string deleteReq = "delete table2 where true";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert);
    if(!result.isOk()) return FAIL;
    result = db.execute(deleteReq);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_delete_incorrectColumnNameInCondition(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert = "insert (col1 = 32, col2 = false) to table1";

    string deleteReq = "delete table1 where col3 = 10";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert);
    if(!result.isOk()) return FAIL;
    result = db.execute(deleteReq);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_delete_incorrectOperandsTypeInCondition(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert = "insert (col1 = 32, col2 = false) to table1";

    string deleteReq = "delete table1 where col1 = \"lol\"";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert);
    if(!result.isOk()) return FAIL;
    result = db.execute(deleteReq);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_delete_normal(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 33, col2 = false) to table1";

    string deleteReq = "delete table1 where col1 = 33";

    string select = "select col1, col2 from table1 where true";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(deleteReq);
    if(!result.isOk()) return FAIL;
    result = db.execute(select);
    if(!result.isOk()) return FAIL;

    std::vector<int> ans;
    for(auto elem: result){
        ans.push_back(elem.getInt("col1"));
    }
    if(ans.size() == 1 && ans[0] == 32) return OK;

    
    return FAIL;
}

// //--------------
// // Update tests
// //--------------

bool test_update_incorrectTableName(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";

    string update = "update table2 set col1 = 30 where col1 = 32";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(update);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_update_incorrectColumnNameInAssignment(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";

    string update = "update table1 set col3 = 30 where col1 = 32";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(update);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_update_incorrectColumnNameInCondition(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";

    string update = "update table1 set col1 = 30 where col3 = 32";
    

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(update);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_update_checkOneColumnAssignment(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";

    string update = "update table1 set col1 = 30 where col1 = 32";
    
    string select = "select col1, col2 from table1 where true";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(update);
    if(!result.isOk()) return FAIL;
    result = db.execute(select);
    if(!result.isOk()) return FAIL;
    std::vector<int> ans;
    for(auto elem: result){
        ans.push_back(elem.getInt("col1"));
    }
    if(ans.size() == 2 && ans[0] == 30 && ans[1] == 34) return OK;

    return FAIL;
}

bool test_update_checkTwoColumnAssignment(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";

    string update = "update table1 set col1 = 30, col2 = true where col1 = 32";
    
    string select = "select col1, col2 from table1 where true";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(update);
    if(!result.isOk()) return FAIL;
    result = db.execute(select);
    if(!result.isOk()) return FAIL;
    std::vector<int> ans1;
    std::vector<bool> ans2;
    for(auto elem: result){
        ans1.push_back(elem.getInt("col1"));
        ans2.push_back(elem.getBool("col2"));
    }
    if(ans1.size() == 2 && ans1[0] == 30 && ans1[1] == 34 && ans2.size() == 2 && ans2[0] == true && ans2[1] == true) return OK;

    return FAIL;
}


bool test_update_checkAssignmentWithColumn(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";

    string update = "update table1 set col1 = col1 + 3 where col1 = 32";
    
    string select = "select col1, col2 from table1 where true";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(update);
    if(!result.isOk()) return FAIL;
    result = db.execute(select);
    if(!result.isOk()) return FAIL;
    std::vector<int> ans1;

    for(auto elem: result){
        ans1.push_back(elem.getInt("col1"));
    }
    if(ans1.size() == 2 && ans1[0] == 35 && ans1[1] == 34) return OK;

    return FAIL;
}


bool test_update_invalidOperandTypesInAssignmentERR(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";

    string update = "update table1 set col1 = \"asd\" where col2 = false";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(update);
    if(!result.isOk()) return OK;

    return FAIL;
}

bool test_update_invalidOperandTypesInConditionERR(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";

    string update = "update table1 set col1 = 30 where col1 = false";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(update);
    if(!result.isOk()) return OK;
    
    return FAIL;
}
// //-------------
// // Select tests
// //-------------

bool test_select_emptyColumnsSetERR(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";
    
    string select = "select  from table1 where true";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(select);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_select_checkOneColumn(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";

    
    string select = "select col1 from table1 where true";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(select);
    if(!result.isOk()) return FAIL;
    std::vector<int> ans1;

    for(auto elem: result){
        ans1.push_back(elem.getInt("col1"));
    }
    if(ans1.size() == 2 && ans1[0] == 32 && ans1[1] == 34) return OK;

    return FAIL;
}

bool test_select_checkMoreColumns(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";
    
    string select = "select col1, col2 from table1 where true";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(select);
    if(!result.isOk()) return FAIL;
    std::vector<int> ans1;
    std::vector<bool> ans2;
    for(auto elem: result){
        ans1.push_back(elem.getInt("col1"));
        ans2.push_back(elem.getBool("col2"));
    }
    if(ans1.size() == 2 && ans1[0] == 32 && ans1[1] == 34  && ans2.size() == 2 && ans2[0] == false && ans2[1] == true) return OK;

    return FAIL;
}

bool test_select_tableNameNotFoundERR(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";
    
    string select = "select col1, col2 from table2 where true";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(select);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_select_columnNameNotFoundERR(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";
    
    string select = "select col3, col2 from table1 where true";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(select);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_select_columnNameNotFoundInConditionERR(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";
    
    string select = "select col1, col2 from table1 where col3 = \"haha\"";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(select);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_select_checkScopes(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true, col3 : string[5])";
    
    string insert1 = "insert (32, false, \"str\") to table1";
    string insert2 = "insert (15, , \"nope\") to table1";
    
    string select = "select col1, col2 from table1 where |col3| = (3+5) / 2";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(select);
    if(!result.isOk()) return FAIL;

    std::vector<int> ans;
    for(auto elem: result){
        ans.push_back(elem.getInt("col1"));
    }
    if(ans.size()==1 && ans[0] == 15) return OK;
    
    return FAIL;
}

bool test_select_checkMainBoolOperations(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true, col3 : string[5])";
    
    string insert1 = "insert (32, false, \"str\") to table1";
    string insert2 = "insert (15, , \"nope\") to table1";
    string insert3 = "insert (72, false, \"hehe\") to table1";
    
    string select = "select col1, col2 from table1 where (|col3| = (3+5) / 2 || col1 = 32) && col2 = false";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert3);
    if(!result.isOk()) return FAIL;
    result = db.execute(select);
    if(!result.isOk()) return FAIL;



    std::vector<int> ans;
    for(auto elem: result){
        ans.push_back(elem.getInt("col1"));
    }
    if(ans.size()==2 && ans[0] == 32 && ans[1] == 72) return OK;
    
    return FAIL;
}


bool test_select_incorrectColumnTypeInCondition(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";
    
    string select = "select col1, col2 from table1 where col2 = 70";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(select);
    if(!result.isOk()) return OK;
    
    return FAIL;
}

bool test_select_incorrectColumnNameAfterSelect(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";
    
    string select = "select col2 from table1 where true";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(select);
    if(!result.isOk()) return FAIL;

    std::vector<int> ans;
    for(auto elem: result){
        try{ans.push_back(elem.getInt("col1"));}
        catch(std::string error) {return OK;}
        
    }
    
    return FAIL;
}

bool test_select_incorrectColumnTypeAfterSelect(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";
    
    string select = "select col2 from table1 where true";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(select);
    if(!result.isOk()) return FAIL;

    std::vector<int> ans;
    for(auto elem: result){
        try{ans.push_back(elem.getInt("col2"));}
        catch(std::string error) {return OK;}
    }
    
    return FAIL;
}

bool test_select_incorrectConditionSyntax(){
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";
    
    string select = "select col2 from table1 where true = 32 ||";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;
    result = db.execute(select);
    if(!result.isOk()) return OK;

    return FAIL;
}
// //------------
// // Other tests
// //------------

bool test_loadAndSaveFile(){
    
    Database db = Database();

    string create = "create table table1 (col1 : int32, col2 : bool = true)";
    
    string insert1 = "insert (col1 = 32, col2 = false) to table1";
    string insert2 = "insert (col1 = 34, ) to table1";
    
    string select = "select col1, col2 from table1 where true";

    ResultSet result;

    result = db.execute(create);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert1);
    if(!result.isOk()) return FAIL;
    result = db.execute(insert2);
    if(!result.isOk()) return FAIL;

    db.saveToFile("database/testDB.database");
    Database db1 = db.loadFromFile("database/testDB.database");

    result = db1.execute(select);
    if(!result.isOk()) return FAIL;

    std::vector<int> ans1;
    std::vector<bool> ans2;
    for(auto elem: result){
        ans1.push_back(elem.getInt("col1"));
        ans2.push_back(elem.getBool("col2"));
    }

    if(ans1.size()==2 && ans2.size()==2 && ans1[0]==32 && ans1[1]==34 && !ans2[0] && ans2[1]) return OK;

    return FAIL;
}


// //--------------
// // Test palette
// //--------------

int main(void){
    run_test("CREATE => Проверка ошибки создания таблицы без информации",                                 test_create_empty);
    run_test("CREATE => Проверка работы автоинкремента",                                                  test_create_autoincrement);
    run_test("CREATE => Проверка ошибки создания автоинкремента не на int32",                             test_create_autoincrement_notOnIntERR);
    run_test("CREATE => Проверка уникальности колонки с атрибутом key",                                   test_create_key_checkUniqueERR);
    run_test("CREATE => Проверка уникальности колонки с атрибутом unique",                                test_create_unique_checkUniqueERR);
    run_test("CREATE => Проверка ошибки присвоения default value автоинкремент колонке",                  test_create_autoincrementWithDefaultValue);
    run_test("CREATE => Проверка работы default value на int32",                                          test_create_intDefaultValue);
    run_test("CREATE => Проверка ошибки присвоения default value неправильного типа для int32",           test_create_intDefaultValue_notIntERR);
    run_test("CREATE => Проверка работы default value на bool",                                           test_create_boolDefaultValue);
    run_test("CREATE => Проверка ошибки присвоения default value неправильного типа для bool",            test_create_boolDefaultValue_notBoolERR);
    run_test("CREATE => Проверка работы default value на string",                                         test_create_stringDefaultValue);
    run_test("CREATE => Проверка ошибки присвоения default value неправильной длины для string",          test_create_stringDefaultValue_stringLongerERR);
    run_test("CREATE => Проверка ошибки присвоения default value неправильного типа для string",          test_create_stringDefaultValue_notStringERR);
    run_test("CREATE => Проверка работы default value на bytes",                                          test_create_bytesDefaultValue);
    run_test("CREATE => Проверка ошибки присвоения default value неправильной длины для bytes",           test_create_bytesDefaultValue_BytesLenNotEqualERR);
    run_test("CREATE => Проверка ошибки присвоения default value неправильного типа для bytes",           test_create_bytesDefaultValue_notBytesERR);
    run_test("CREATE => Проверка ошибки создания колонок с одинаковыми именами",                          test_create_columnsWithOneNameERR);
    run_test("CREATE => Проверка ошибки создания таблицы с уже существующим названием",                   test_create_tablesWithOneNameERR);
    run_test("CREATE => Проверка правильности атрибута",                                                  test_create_incorrectAttributeERR);
    run_test("CREATE => Проверка правильности типа",                                                      test_create_incorrectTypeERR);
    run_test("CREATE => Проверка синтаксиса информации о колонке(наличие названия, типа, двоеточия)",     test_create_incorrectColumnInfoSyntaxERR);


    run_test("INSERT => Проверка работы вида Column = Value",               test_insert_columnEqualValue);
    run_test("INSERT => Проверка работы вида Value",                        test_insert_onlyValue);
    run_test("INSERT => Проверка работы вида default Value",                test_insert_defaultValue);
    run_test("INSERT => Ошибка отсутствия default value",                   test_insert_defaultValueERR);
    run_test("INSERT => Ошибка неправильности типа",                        test_insert_valueTypeERR);
    run_test("INSERT => Ошибка неправильности длины string",                test_insert_stringIncorrectLenERR);
    run_test("INSERT => Ошибка неправильности длины bytes",                 test_insert_bytesIncorrectLenERR);
    run_test("INSERT => Ошибка не найдена таблица",                         test_insert_invalidTableNameERR);
    run_test("INSERT => Ошибка неправильное количество значений",           test_insert_valuesMoreThanColumnsERR);
    run_test("INSERT => Проверка уникальности значений",                    test_insert_uniqueValues);
    run_test("INSERT => Ошибка отсутствует столбец (для Column = Value)",   test_insert_noColumnsForEqualERR);


    run_test("DELETE => Проверка правильного удаления",                     test_delete_normal);
    run_test("DELETE => Ошибка не найдена таблица",                         test_delete_incorrectTableName);
    run_test("DELETE => Ошибка не найден столбец в condition",              test_delete_incorrectColumnNameInCondition);
    run_test("DELETE => Ошибка неправильный тип операндов в condition",     test_delete_incorrectOperandsTypeInCondition);
    

    run_test("UPDATE => Ошибка не найдена таблица",                                     test_update_incorrectTableName);
    run_test("UPDATE => Ошибка не найден столбец в Assignment",                         test_update_incorrectColumnNameInAssignment);
    run_test("UPDATE => Ошибка не найден столбец в Condition",                          test_update_incorrectColumnNameInCondition);
    run_test("UPDATE => Проверка с одной колонкой в Assignment",                        test_update_checkOneColumnAssignment);
    run_test("UPDATE => Проверка с НЕ одной колонкой в Assignment",                     test_update_checkTwoColumnAssignment);
    run_test("UPDATE => Проверка присвоения относительно текущего значения столбца",    test_update_checkAssignmentWithColumn);
    run_test("UPDATE => Ошибка неверный тип операндов в Assignment",                    test_update_invalidOperandTypesInAssignmentERR);
    run_test("UPDATE => Ошибка неверный тип операндов в Condition",                     test_update_invalidOperandTypesInConditionERR);


    run_test("SELECT => Ошибка пустая графа колонок",                                   test_select_emptyColumnsSetERR);
    run_test("SELECT => Проверка выбора одной колонки",                                 test_select_checkOneColumn);
    run_test("SELECT => Проверка выбора нескольких колонок",                            test_select_checkMoreColumns);
    run_test("SELECT => Ошибка не найдена таблица",                                     test_select_tableNameNotFoundERR);
    run_test("SELECT => Ошибка не найдена колонка для выбора",                          test_select_columnNameNotFoundERR);
    run_test("SELECT => Ошибка не найдена колонка в Condition",                         test_select_columnNameNotFoundInConditionERR);
    run_test("SELECT => Проверка работы арифметических скобок в Condition",             test_select_checkScopes);
    run_test("SELECT => Проверка работы скобок и операций && || ^^",                    test_select_checkMainBoolOperations);
    run_test("SELECT => Ошибка неверный тип колонки в condition",                       test_select_incorrectColumnTypeInCondition);
    run_test("SELECT => Ошибка не найдена колонка при get",                             test_select_incorrectColumnNameAfterSelect);
    run_test("SELECT => Ошибка не верный тип колнки в get",                             test_select_incorrectColumnTypeAfterSelect);
    run_test("SELECT => Ошибка неверный синтаксис в Condition",                         test_select_incorrectConditionSyntax);
    run_test("SELECT => Проверка работы loadFromFile и loadToFile",                     test_loadAndSaveFile);

    return EXIT_SUCCESS;
}
