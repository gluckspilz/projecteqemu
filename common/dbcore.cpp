#include "../common/debug.h"
#include "../common/files.h"

#ifdef _WINDOWS
#include <winsock2.h>
#endif

#include <iostream>
using namespace std;
#include <errmsg.h>
#include <mysqld_error.h>
#include <limits.h>
#include "dbcore.h"
#include <string.h>
#include "../common/MiscFunctions.h"
#include <cstdlib>

#ifdef _WINDOWS
	#define snprintf	_snprintf
	#define strncasecmp	_strnicmp
	#define strcasecmp	_stricmp
	#include <process.h>
#else
	#include "unix.h"
	#include <pthread.h>
#endif

#ifdef _EQDEBUG
	#define DEBUG_MYSQL_QUERIES 0
#else
	#define DEBUG_MYSQL_QUERIES 0
#endif

DBcore::DBcore() {
	mysql_init(&mysql);
	pHost = 0;
	pUser = 0;
	pPassword = 0;
	pDatabase = 0;
	pCompress = false;
	pSSL = false;
}

DBcore::~DBcore() {
	mysql_close(&mysql);
	safe_delete_array(pHost);
	safe_delete_array(pUser);
	safe_delete_array(pPassword);
	safe_delete_array(pDatabase);
}


bool DBcore::ReadDBINI(char *host, char *user, char *passwd, char *database, int32 &port, bool &compress, bool *items) {
	char buf[200], type[200];
	char linebuf[512];
	char cport[6]={0};

	FILE *f;
	
	if (!(f = fopen (DB_INI_FILE, "r")))
	{
		printf ("Couldn't open Database Config File '%s'.\n", DB_INI_FILE);
		return(false);
	}
	
	do
	{
		fgets (buf, 199, f);
		if (feof (f))
		{
			printf ("[Database] block not found in DB.INI.\n");
			fclose(f);
			return(false);
		}
	}
	while (strncasecmp (buf, "[Database]\n", 11) != 0 && strncasecmp (buf, "[Database]\r\n", 12) != 0);
	
	while (!feof (f))
	{
		if(fgets(linebuf, 512, f) == NULL)
			continue;
#ifdef _WINDOWS
		if (sscanf(linebuf, "%[^=]=%[^\n]\n", type, buf) != 2)
			continue;
#else	
		if (sscanf(linebuf, "%[^=]=%[^\r\n]\n", type, buf) != 2)
			continue;
#endif
			{
				if (!strncasecmp (type, "host", 4))
				{
					strn0cpy (host, buf, 199);
					items[0] = true;
				}
				if (!strncasecmp (type, "user", 4))
				{
					strn0cpy (user, buf, 199);
					items[1] = true;
				}
				if (!strncasecmp (type, "pass", 4))
				{
					strn0cpy (passwd, buf, 199);
					items[2] = true;
				}
				if (!strncasecmp (type, "data", 4))
				{
					strn0cpy (database, buf, 199);
					items[3] = true;
				}
				if (!strncasecmp (type, "port", 4))
				{
					strn0cpy(cport,buf,5);
					port=atoi(cport);
					items[4] = true;
				}
				if (!strncasecmp (type, "compression", 11)) {
					if (strcasecmp(buf, "on") == 0) {
						items[5] = true;
						compress = true;
						printf("DB Compression on.\n");
					}
					else if (strcasecmp(buf, "off") != 0)
						printf("Unknown 'compression' setting in db.ini. Expected 'on' or 'off'.\n");;
				}
			}
	}
	
	fclose (f);
	return(true);
}


// Sends the MySQL server a keepalive
void DBcore::ping() {
	if (!MDatabase.trylock()) {
		// well, if's it's locked, someone's using it. If someone's using it, it doesnt need a keepalive
		return;
	}
	mysql_ping(&mysql);
	MDatabase.unlock();
}

bool DBcore::RunQuery(const char* query, int32 querylen, char* errbuf, MYSQL_RES** result, int32* affected_rows, int32* last_insert_id, int32* errnum, bool retry) {
	_CP(DBcore_RunQuery);
	if (errnum)
		*errnum = 0;
	if (errbuf)
		errbuf[0] = 0;
	bool ret = false;
	LockMutex lock(&MDatabase);
	if (pStatus != Connected)
		Open();
#if DEBUG_MYSQL_QUERIES >= 1
	char tmp[120];
	strn0cpy(tmp, query, sizeof(tmp));
	cout << "QUERY: " << tmp << endl;
#endif
	if (mysql_real_query(&mysql, query, querylen)) {
		if (mysql_errno(&mysql) == CR_SERVER_GONE_ERROR)
			pStatus = Error;
		if (mysql_errno(&mysql) == CR_SERVER_LOST || mysql_errno(&mysql) == CR_SERVER_GONE_ERROR) {
			if (retry) {
				cout << "Database Error: Lost connection, attempting to recover...." << endl;
				ret = RunQuery(query, querylen, errbuf, result, affected_rows, last_insert_id, errnum, false);
				if (ret)
					cout << "Reconnection to database successful." << endl;
			}
			else {
				pStatus = Error;
				if (errnum)
					*errnum = mysql_errno(&mysql);
				if (errbuf)
					snprintf(errbuf, MYSQL_ERRMSG_SIZE, "#%i: %s", mysql_errno(&mysql), mysql_error(&mysql));
				cout << "DB Query Error #" << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
				ret = false;
			}
		}
		else {
			if (errnum)
				*errnum = mysql_errno(&mysql);
			if (errbuf)
				snprintf(errbuf, MYSQL_ERRMSG_SIZE, "#%i: %s", mysql_errno(&mysql), mysql_error(&mysql));
#ifdef _EQDEBUG
			cout << "DB Query Error #" << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
#endif
			ret = false;
		}
	}
	else {
		if (result && mysql_field_count(&mysql)) {
			*result = mysql_store_result(&mysql);
#ifdef _EQDEBUG
			DBMemLeak::Alloc(*result, query);
#endif
		}
		else if (result)
			*result = 0;
		if (affected_rows)
			*affected_rows = mysql_affected_rows(&mysql);
		if (last_insert_id)
			*last_insert_id = mysql_insert_id(&mysql);
		if (result) {
			if (*result) {
				ret = true;
			}
			else {
#ifdef _EQDEBUG
				cout << "DB Query Error: No Result" << endl;
#endif
				if (errnum)
					*errnum = UINT_MAX;
				if (errbuf)
					strcpy(errbuf, "DBcore::RunQuery: No Result");
				ret = false;
			}
		}
		else {
			ret = true;
		}
	}
#if DEBUG_MYSQL_QUERIES >= 1
	if (ret) {
		cout << "query successful";
		if (result && (*result))
			cout << ", " << (int) mysql_num_rows(*result) << " rows returned";
		if (affected_rows)
			cout << ", " << (*affected_rows) << " rows affected";
		cout<< endl;
	}
	else {
		cout << "QUERY: query FAILED" << endl;
	}
#endif
	return ret;
}

int32 DBcore::DoEscapeString(char* tobuf, const char* frombuf, int32 fromlen) {
//	No good reason to lock the DB, we only need it in the first place to check char encoding.
//	LockMutex lock(&MDatabase);
	return mysql_real_escape_string(&mysql, tobuf, frombuf, fromlen);
}

bool DBcore::Open(const char* iHost, const char* iUser, const char* iPassword, const char* iDatabase,int32 iPort, int32* errnum, char* errbuf, bool iCompress, bool iSSL) {
	LockMutex lock(&MDatabase);
	safe_delete(pHost);
	safe_delete(pUser);
	safe_delete(pPassword);
	safe_delete(pDatabase);
	pHost = strcpy(new char[strlen(iHost) + 1], iHost);
	pUser = strcpy(new char[strlen(iUser) + 1], iUser);
	pPassword = strcpy(new char[strlen(iPassword) + 1], iPassword);
	pDatabase = strcpy(new char[strlen(iDatabase) + 1], iDatabase);
	pCompress = iCompress;
	pPort = iPort;
	pSSL = iSSL;
	return Open(errnum, errbuf);
}

bool DBcore::Open(int32* errnum, char* errbuf) {
	if (errbuf)
		errbuf[0] = 0;
	LockMutex lock(&MDatabase);
	if (GetStatus() == Connected)
		return true;
	if (GetStatus() == Error)
		mysql_close(&mysql);
		mysql_init(&mysql);		// Initialize structure again
	if (!pHost)
		return false;
	/*
	Added CLIENT_FOUND_ROWS flag to the connect
	otherwise DB update calls would say 0 rows affected when the value already equalled
	what the function was tring to set it to, therefore the function would think it failed 
	*/
	int32 flags = CLIENT_FOUND_ROWS;
	if (pCompress)
		flags |= CLIENT_COMPRESS;
	if (pSSL)
		flags |= CLIENT_SSL;
	if (mysql_real_connect(&mysql, pHost, pUser, pPassword, pDatabase, pPort, 0, flags)) {
		pStatus = Connected;
		return true;
	}
	else {
		if (errnum)
			*errnum = mysql_errno(&mysql);
		if (errbuf)
			snprintf(errbuf, MYSQL_ERRMSG_SIZE, "#%i: %s", mysql_errno(&mysql), mysql_error(&mysql));
		pStatus = Error;
		return false;
	}
}






