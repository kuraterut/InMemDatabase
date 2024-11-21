#include <vector>
#include <variant>

using namespace std;




class Select{
public:
	ResultSet execute();
};

class Insert{
public:
	int execute();
};

class Create{
private:
	Table newTable;

public:
	Create() = delete;
	Create();
	int execute();
};

class Update{
public:
	int execute();
};

class Delete{
public:
	int execute();
};