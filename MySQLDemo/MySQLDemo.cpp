#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h> 
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <string>
using namespace std;

#pragma comment(lib, "libmysql.lib")

struct SDBInfo
{
	string user;
	string pwd;
	string host;
	string db;
	string fileName;
	UINT32 port;
};

void ChkHero(SDBInfo db)
{
	cout << "check hero: " << db.db << endl;

	MYSQL myCont;
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	int res;
	mysql_init(&myCont);

	map<UINT64, map<UINT64, UINT64> > countryHeroes;
	map<UINT64, map<UINT64, UINT64> > heroAmount;
	map<UINT64, UINT64> accountMap;
	map<UINT64, map<UINT64, UINT64> > realScore;
	vector< pair<UINT64, UINT64> > delHeroes;

	if (mysql_real_connect(&myCont, db.host.c_str(), db.user.c_str(), db.pwd.c_str(), db.db.c_str(), db.port, NULL, 0))
	{
		cout << "connect succeed!" << endl;
		mysql_query(&myCont, "SET NAMES GBK"); //设置编码格式,否则在cmd下无法显示中文

		cout << "load country_hero: " << endl;
		res = mysql_query(&myCont, "select account_id, land_id, score from country_hero");//查询
		result = mysql_store_result(&myCont);//保存查询到的数据到result
		UINT64 nCount = 0;
		UINT64 nTotal = mysql_num_rows(result);
		while ((sql_row = mysql_fetch_row(result)))//获取具体的数据
		{
			UINT64 accountId = _atoi64(sql_row[0]);
			UINT64 landId = _atoi64(sql_row[1]);
			countryHeroes[accountId][landId] += _atoi64(sql_row[2]);
			++heroAmount[accountId][landId];
			++nCount;
			if (nCount % 1000 == 0)
				cout << "\r" << nCount << "\\" << nTotal;
		}
		mysql_free_result(result);
		cout << endl;

		for (auto it = countryHeroes.begin(); it != countryHeroes.end(); ++it)
		{
			UINT64 accountId = it->first;
			auto& heroes = it->second;
			for (auto it2 = heroes.begin(); it2 != heroes.end(); )
			{
				UINT64 landId = it2->first;
				UINT64 score = it2->second;

				it2->second /= heroAmount[accountId][landId];
				if (it2->second < 3000)
					it2 = heroes.erase(it2);
				else
					++it2;
			}
		}

		cout << "load account_list:" << endl;
		res = mysql_query(&myCont, "select AccId, UserId from account_list");//查询
		result = mysql_store_result(&myCont);//保存查询到的数据到result
		nCount = 0;
		nTotal = mysql_num_rows(result);
		while ((sql_row = mysql_fetch_row(result)))//获取具体的数据
		{
			UINT64 accountId = _atoi64(sql_row[0]);
			if (countryHeroes.count(accountId))
			{
				UINT64 roleId = _atoi64(sql_row[1]);
				accountMap[roleId] = accountId;
				if (accountMap.size() == countryHeroes.size())
					break;
			}
			++nCount;
			if (nCount % 1000 == 0)
				cout << "\r" << nCount << "\\" << nTotal;
		}
		mysql_free_result(result);
		cout << endl;


		cout << "load dota_profiles: " << endl;
		res = mysql_query(&myCont, "select UserId, land_id, DH from dota_profiles where GameMode = 1116");//查询
		result = mysql_store_result(&myCont);//保存查询到的数据到result
		nCount = 0;
		nTotal = mysql_num_rows(result);
		while ((sql_row = mysql_fetch_row(result)))//获取具体的数据
		{
			UINT64 userId = _atoi64(sql_row[0]);
			UINT64 landId = _atoi64(sql_row[1]);
			UINT64 score = _atoi64(sql_row[2]);

			if (accountMap.count(userId))
			{
				UINT64 accountId = accountMap[userId];
				realScore[accountId][landId] = score;
			}

			++nCount;
			if (nCount % 1000 == 0)
				cout << "\r" << nCount << "\\" << nTotal;
		}
		mysql_free_result(result);
		cout << endl;
	}
	mysql_close(&myCont);//断开连接

	for (auto it = countryHeroes.begin(); it != countryHeroes.end(); ++it)
	{
		UINT64 accountId = it->first;
		auto& heroes = it->second;
		for (auto it2 = heroes.begin(); it2 != heroes.end(); ++it2)
		{
			UINT64 landId = it2->first;
			UINT64 score = it2->second;

			if (realScore.count(accountId) && realScore[accountId].count(landId) && score < realScore[accountId][landId] + 5000)
			{

			}
			else
			{
				delHeroes.push_back(make_pair(accountId, landId));
			}
		}
	}

	FILE* fp = NULL;
	fopen_s(&fp, db.fileName.c_str(), "w");
	for (auto it = delHeroes.begin(); it != delHeroes.end(); ++it)
	{
		auto& delPair = *it;
		fprintf(fp, "delete from country_hero where account_id = %I64d and land_id = %I64d;\n", delPair.first, delPair.second);

	}
	fclose(fp);
}

void ChkHero2(SDBInfo db)
{
	cout << "check hero: " << db.db << endl;

	MYSQL myCont;
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	int res;
	mysql_init(&myCont);

	struct SCountryMbr
	{
		UINT64 roleId;
		UINT64 areaId;
		UINT64 landId;
		UINT64 accountId;
		UINT64 score;
		UINT64 realScore;
		bool bDel;

		SCountryMbr() :roleId(0), areaId(0), landId(0), accountId(0), score(0), realScore(0), bDel(true) {}
	};

	map<UINT64, map<UINT64, SCountryMbr> > countryMbr;
	map<UINT64, UINT64> accountMap;
	map<UINT64, map<UINT64, UINT64> > realScore;
	vector< pair<UINT64, UINT64> > delHeroes;

	if (mysql_real_connect(&myCont, db.host.c_str(), db.user.c_str(), db.pwd.c_str(), db.db.c_str(), db.port, NULL, 0))
	{
		cout << "connect succeed!" << endl;
		mysql_query(&myCont, "SET NAMES GBK"); //设置编码格式,否则在cmd下无法显示中文

		cout << "load country_mbr: " << endl;
		res = mysql_query(&myCont, "select role_id, area_id, land_id, account_id, score from country_mbr");//查询
		result = mysql_store_result(&myCont);//保存查询到的数据到result
		UINT64 nCount = 0;
		UINT64 nTotal = mysql_num_rows(result);
		while ((sql_row = mysql_fetch_row(result)))//获取具体的数据
		{
			SCountryMbr mbr;
			mbr.roleId = _atoi64(sql_row[0]);
			mbr.areaId = _atoi64(sql_row[1]);
			mbr.landId = _atoi64(sql_row[2]);
			mbr.accountId = _atoi64(sql_row[3]);
			mbr.score = _atoi64(sql_row[4]);
			countryMbr[mbr.roleId][mbr.areaId] = mbr;

			++nCount;
			if (nCount % 1000 == 0 || nCount == nTotal)
				cout << "\r" << nCount << "\\" << nTotal;
		}
		mysql_free_result(result);
		cout << endl;

		cout << "load dota_profiles: " << endl;
		res = mysql_query(&myCont, "select UserId, area_id, DH from dota_profiles where GameMode = 1116");//查询
		result = mysql_store_result(&myCont);//保存查询到的数据到result
		nCount = 0;
		nTotal = mysql_num_rows(result);
		while ((sql_row = mysql_fetch_row(result)))//获取具体的数据
		{
			UINT64 userId = _atoi64(sql_row[0]);
			UINT64 areaId = _atoi64(sql_row[1]);
			UINT64 score = _atoi64(sql_row[2]);

			if (countryMbr.count(userId) && countryMbr[userId].count(areaId))
			{
				auto& mbr = countryMbr[userId][areaId];
				if (mbr.score < score + 3000)
					mbr.bDel = false;
			}

			++nCount;
			if (nCount % 1000 == 0 || nCount == nTotal)
				cout << "\r" << nCount << "\\" << nTotal;
		}
		mysql_free_result(result);
		cout << endl;
	}
	mysql_close(&myCont);//断开连接

	FILE* fp = NULL;
	fopen_s(&fp, db.fileName.c_str(), "w");
	for (auto it = countryMbr.begin(); it != countryMbr.end(); ++it)
	{
		auto& heroes = it->second;
		for (auto it2 = heroes.begin(); it2 != heroes.end(); ++it2)
		{
			auto& mbr = it2->second;

			if (mbr.score >= 3000 && mbr.bDel)
				fprintf(fp, "delete from country_hero where account_id = %I64d and land_id = %I64d; #role_id:%I64d areaId:%I64d\n", mbr.accountId, mbr.landId, mbr.roleId, mbr.areaId);
		}
	}
	fclose(fp);
}

//class Base
//{
//public:
//	void a(float x) { cout << "Base::a(float) " << x << endl; }
//	void b(float x) { cout << "Base::b(float) " << x << endl; }
//};
//class Derived : public Base
//{
//public:
//	void a(int x) { cout << "Derived::a(int) " << x << endl; }
//	void b(float x) { cout << "Derived::b(float) " << x << endl; }
//};

int sum(int a, int b)
{
	return a + b;
}

int main()
{
	//Derived d;
	//Base *pb = &d;
	//Derived *pd = &d;

	//pb->a(3.14f); // Base::a(float) 3.14
	//pd->a(3.14f); // Base::a(float) 3.14
	//pb->b(3.14f); // Base::b(float) 3.14
	//pd->b(3.14f); // Derived::b(float) 3.14

	const int a = 10;
	int *p = (int*)(&a);
	*p = 20;
	cout << "a: " << a << " *p:" << *p << endl;

	int(*func)(int, int) = sum;

	SDBInfo db;
	db.user = "lp";
	db.pwd = "123456";
	db.host = "localhost";
	db.port = 3307;

	db.db = "lobbyplatformpve";
	db.fileName = db.db + ".sql";
	ChkHero2(db);

	db.db = "nf2";
	db.fileName = db.db + ".sql";
	ChkHero2(db);

	db.db = "nf3";
	db.fileName = db.db + ".sql";
	ChkHero2(db);

	db.db = "bf1";
	db.fileName = db.db + ".sql";
	ChkHero2(db);

	db.db = "bf2";
	db.fileName = db.db + ".sql";
	ChkHero2(db);

	db.db = "7k";
	db.fileName = db.db + ".sql";
	ChkHero2(db);

	db.db = "sh";
	db.fileName = db.db + ".sql";
	ChkHero2(db);

	system("pause");
	return 0;
}