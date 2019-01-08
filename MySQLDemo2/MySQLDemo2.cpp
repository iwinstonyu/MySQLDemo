// MySQLDemo2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

using namespace std;

void Example2()
{
	cout << endl;
	cout << "Let's have MySQL count from 10 to 1..." << endl;

	try {
		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;
		sql::PreparedStatement *pstmt;

		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3307", "lp", "123456");
		/* Connect to the MySQL test database */
		con->setSchema("test");

		stmt = con->createStatement();
		stmt->execute("DROP TABLE IF EXISTS test");
		stmt->execute("CREATE TABLE test(id INT)");
		delete stmt;

		/* '?' is the supported placeholder syntax */
		pstmt = con->prepareStatement("INSERT INTO test(id) VALUES (?)");
		for (int i = 1; i <= 10; i++) {
			pstmt->setInt(1, i);
			pstmt->executeUpdate();
		}
		delete pstmt;

		/* Select in ascending order */
		pstmt = con->prepareStatement("SELECT id FROM test ORDER BY id ASC");
		res = pstmt->executeQuery();

		/* Fetch in reverse = descending order! */
		res->afterLast();
		while (res->previous())
			cout << "\t... MySQL counts: " << res->getInt("id") << endl;
		delete res;

		delete pstmt;
		delete con;

	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	cout << endl;
}

void Example1()
{
	cout << endl;
	cout << "Running 'SELECT 'Hello World!' AS _message'..." << endl;

	try {
		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;

		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3307", "lp", "123456");
		/* Connect to the MySQL test database */
		con->setSchema("test");

		stmt = con->createStatement();
		res = stmt->executeQuery("SELECT 'Hello World!' AS _message");
		while (res->next()) {
			cout << "\t... MySQL replies: ";
			/* Access column data by alias or column name */
			cout << res->getString("_message") << endl;
			cout << "\t... MySQL says it again: ";
			/* Access column data by numeric offset, 1 is the first column */
			cout << res->getString(1) << endl;
		}

		while (stmt->getMoreResults());

		delete res;
		delete stmt;
		delete con;

	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	catch (...) {

	}

	cout << endl;
}

void Example3()
{
	cout << endl;
	cout << "Running 'select * from account_list'" << endl;

	try {
		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;

		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3307", "lp", "123456");
		/* Connect to the MySQL test database */
		con->setSchema("lobbyplatformpve");

		stmt = con->createStatement();
		res = stmt->executeQuery("call TestMoreResult()");
		while (res->next()) {
			cout << res->getUInt(1) << endl;
		}

		cout << endl;

		stmt->execute("update test set author=\'test2\' where id=1");

		while (stmt->getMoreResults()) stmt->getResultSet();

		if (stmt->getMoreResults())
		{
			res = stmt->getResultSet();
			while (res->next()) {
				cout << res->getUInt(1) << endl;
			}
		}

		delete res;
		delete stmt;
		delete con;

	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	cout << endl;
}

void Example4()
{
	cout << endl;
	cout << "Running 'select * from account_list'" << endl;

	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::ResultSet *res = NULL;

	/* Create a connection */
	driver = get_driver_instance();
	con = driver->connect("tcp://192.168.19.64:3307", "lp", "123");
	/* Connect to the MySQL test database */
	con->setSchema("lobbyplatformpve");

	stmt = con->createStatement();

	try {
		stmt->execute("call AppendMail ( 0 , '' , 9030653 , '1' , 46 , 0 , '' , '\' , 1708181257 , 1505624249 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , @_last_id ) ");
		while (res->next()) {
			cout << res->getUInt(1) << endl;
		}

		cout << endl;

		while (stmt->getMoreResults()) stmt->getResultSet();

		if (stmt->getMoreResults())
		{
			res = stmt->getResultSet();
			while (res->next()) {
				cout << res->getUInt(1) << endl;
			}
		}
	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;

		while (stmt->getMoreResults()) stmt->getResultSet();
	}

	if(res) delete res;
	delete stmt;
	delete con;

	cout << endl;
}

void Example5()
{
	//cout << endl;
	//cout << "Running 'select * from account_list'" << endl;

	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::ResultSet *res = NULL;

	/* Create a connection */
	driver = get_driver_instance();
	con = driver->connect("tcp://127.0.0.1:3307", "lp", "123");
	/* Connect to the MySQL test database */
	con->setSchema("lobby_slg_qa1");

	stmt = con->createStatement();

	try {
		string query = "select 'hello world'";
		res = stmt->executeQuery(" select ''hello world'");
		cout << "Start result" << endl;
		while (res->next()) {
			cout << res->getUInt(1) << endl;
		}
		cout << "End result" << endl;
		cout << endl;

		while (stmt->getMoreResults()) stmt->getResultSet();

		if (stmt->getMoreResults())
		{
			res = stmt->getResultSet();
			while (res->next()) {
				cout << res->getUInt(1) << endl;
			}
		}
	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;

		while (stmt->getMoreResults()) stmt->getResultSet();
	}
	catch (std::exception& e)
	{
		cout << "Exception: " << e.what() << endl;
	}

	if (res) delete res;
	delete stmt;
	delete con;

	cout << endl;
}

inline string EscapeProcVar(const string& str)
{
	vector<char> escapeVec(str.length() * 2 + 1, 0);
	size_t idx = 0;
	for (size_t i = 0; i < str.length(); ++i) {
		switch (str[i])
		{
		case '\\':
		case '\'':
			escapeVec[idx++] = '\\';
		default:
			break;
		}
		escapeVec[idx++] = str[i];
	}
	return string(escapeVec.begin(), escapeVec.begin() + idx);
}

int main(void)
{
	vector<string> strs;
	strs.push_back("123");
	strs.push_back("'hello world");
	strs.push_back("\\hello world");
	for (auto it = strs.begin(); it != strs.end(); ++it) {
		auto& str = *it;

		string escapeStr = EscapeProcVar(str);
		cout << "Before: " << str << " " << str.length() << endl;
		cout << "After: " << escapeStr << " " << escapeStr.length() << endl;
	}

	//Example5();

	system("pause");

	return 0;
}
