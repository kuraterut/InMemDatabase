#include "DataBase.hpp"


using namespace std;




int main(){
	Database db = Database();
	string create = "cReAte taBLe lol   ({key} name0 :    int32 = 2,    name1 : string[32] = \"haha\")";
	string insert1 = "insert (3,\"hehehe\") to lol";
	string insert2 = "insert (2,) to lol";
	string insert3 = "insert (name1 = \"elelelel\", name0 = 5) to lol";

	string select1 = "select name1, name0 from lol where |name1| = 6 || name0 % 2 = 0";
	string deleteReq = "delete lol where true";
	string select2 = "select name0, name1 from lol where true";

	string update1 = "update lol set name0 = 4, name1=\"fixed\" where name0 = 5";


	ResultSet createPost = db.execute(create);
	if(!createPost.isOk()){cout << createPost.getError() << endl;}

	ResultSet createPostErr = db.execute(create);
	if(!createPostErr.isOk()){cout << createPostErr.getError() << endl;}


	ResultSet insertPost1 = db.execute(insert1);
	if(!insertPost1.isOk()){cout << insertPost1.getError() << endl;}
	ResultSet insertPost2 = db.execute(insert2);
	if(!insertPost2.isOk()){cout << insertPost2.getError() << endl;}
	ResultSet insertPost3 = db.execute(insert3);
	if(!insertPost3.isOk()){cout << insertPost3.getError() << endl;}
	ResultSet updatePost1 = db.execute(update1);
	if(!updatePost1.isOk()){cout << updatePost1.getError() << endl;}

	ResultSet selectGet2 = db.execute(select2);
	if(!selectGet2.isOk()){cout << selectGet2.getError() << endl;}
	for(auto elem : selectGet2){
		cout << elem.getInt("name0") << endl;
		cout << elem.getString("name1") << endl;
		cout << endl;
	}
	cout << endl;

	





	
	// ResultSet selectGet2 = db1.executeGet(select2);
	
	


	// cout << get<int>(db.getMap()["lol"].rows[0][0]) << endl;
	// cout << get<string>(db.getMap()["lol"].rows[0][1]) << endl;
	// cout << get<int>(db.getMap()["lol"].rows[1][0]) << endl;
	// cout << get<string>(db.getMap()["lol"].rows[1][1]) << endl;
	// cout << get<int>(db.getMap()["lol"].rows[2][0]) << endl;
	// cout << get<string>(db.getMap()["lol"].rows[2][1]) << endl;
	return 0;
}