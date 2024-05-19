#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include <cjson/cJSON.h>

#define READ_MODE "r"
#define BUFFER_MAX 255
#define BUFFER_HOSTADDR 16
#define BUFFER_PORT 6

/*
	Author: Bartlomiej Barszczewski
	License: MIT
	compilation : sudo gcc -o pgclient pgclient.c -lpq -lcjson -Wall -std=c2x -ggdb
		run : ./build/pgclient		
*/

struct PGconfig {
	char hostaddr[BUFFER_HOSTADDR];
	char port[BUFFER_PORT];
	char dbname[BUFFER_MAX];
	char user[BUFFER_MAX];
	char password[BUFFER_MAX];
};

const char *PGkeywords[] = {
	"hostaddr",
	"port",
	"dbname",
	"user",
	"password",
	NULL
};

PGconn * PQconnect(char * hostaddr, char * port, char * dbname, char * user, char * password) {	

	const char *values[] = {
		hostaddr,
		port,
		dbname,
		user,
		password,
		NULL
	};
	int expand_dbname = 0;
	
	PGconn * conn = PQconnectdbParams( 
		(const char **) PGkeywords, 
		(const char **) values,
	 	expand_dbname
	);
	
	if(PQstatus(conn) != CONNECTION_OK) {
		fprintf(stderr, "Connection to database %s failed\n", dbname);
		PQfinish(conn);
		exit(1);
	}
	
	return conn;
}

int getStrArrayLength(const char * const * array) {
	
	return ((int) (sizeof(array) / sizeof(const char * const *)));
}

void cJSON_GetValueKey( cJSON * json,  const char * kname, char * buffer) {
	cJSON * key = cJSON_GetObjectItemCaseSensitive(json, kname);

	if( cJSON_IsString(key) && (key->valuestring != NULL ) ) {
		size_t size_buffer = strlen(key->valuestring);
		strncpy( buffer, key->valuestring, size_buffer);
		buffer[size_buffer] = '\0';
	}
}


unsigned long readConfigFile( FILE * fptr ) {	
	if(fptr == NULL) {
		fprintf(stderr, "Failed to open config file: \n");
		exit(4);
	}

	fseek(fptr, 0, SEEK_END);
	unsigned long length = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	
	return length;
}


void cJSON_CheckError(cJSON * json, char * json_buffer ) {
	if( json == NULL) {
		const char * error_ptr = cJSON_GetErrorPtr();
		
		if(error_ptr != NULL) {
			fprintf(stderr, "Error json read: %s\n", error_ptr);
		}

		free(json_buffer);
		cJSON_Delete(json);
		exit(5);
	}
}

struct PGconfig getConfig(char * fname) {

	FILE *fptr = fopen(fname, READ_MODE);
	unsigned long length = readConfigFile(fptr);

	char * json_buffer = malloc( length + 1 );

	int len = fread(json_buffer, 1, length, fptr);
	fclose(fptr);

	json_buffer[length] = '\0';

	if( len == 0 ) {
		printf("Error, config file is empty\n");
		exit(6);
	}

	cJSON * json = cJSON_Parse(json_buffer);
	
	cJSON_CheckError(json, json_buffer);
	
	struct PGconfig config;

	cJSON_GetValueKey(json, PGkeywords[0], config.hostaddr);
	cJSON_GetValueKey(json, PGkeywords[1], config.port);
	cJSON_GetValueKey(json, PGkeywords[2], config.dbname);
	cJSON_GetValueKey(json, PGkeywords[3], config.user);
	cJSON_GetValueKey(json, PGkeywords[4], config.password);

	cJSON_Delete(json);
	return config;	
}

int countParams( const int * arr ) {
	int nParams = 0;
	while(*arr == 0) {
		arr++;
		nParams++;
	}
	return nParams;
}


PGresult * execute(PGconn * conn, char * query) {
	
	if(strlen(query) == 0 || query == NULL) {
		fprintf(stderr, "Execute failed, query is empty!");
		exit(3);
	}

	PGresult * result = PQexec(conn, query);

	if(PQresultStatus(result) != PGRES_TUPLES_OK) {
		puts("Failed, did not get any data!\n");
		return NULL;
	}

	return result;
}

PGresult * executeParams(
	PGconn * conn, 
	const char * command, 
	const char * const * paramValues,
	const int * paramFormats
) {
	const int * formats = paramFormats;
	int nParams = countParams(formats);

	int paramLen[nParams];
	
	for(int x = 0; x < nParams; x += 1) {
		paramLen[x] = sizeof(paramValues[x]);
	}
	
	const int * paramLengths = (const int *) paramLen;
	
	int resultFormat = 0;

	PGresult * res = PQexecParams(
		conn,
		command,
		nParams,
		NULL,
		paramValues,
		paramLengths,
		paramFormats,
		resultFormat		
	);
	
	if(PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "[%d] %s -> PQexecParams failed! %s\n", 
			__LINE__, __FILE__, PQresultErrorMessage(res));
		exit(7);
	}
	
	return res;
}


int main() {
	struct PGconfig cfg = getConfig("pgconfig.json");
	
	PGconn * conn = PQconnect( 
		cfg.hostaddr, 
		cfg.port, 
		cfg.dbname, 
		cfg.user,
		cfg.password
	);

	int rows = 0;
	int cols = 0;
	int count_tuples = 0;
	int count_fields = 0;

	if(PQstatus(conn) == CONNECTION_BAD) {
		fprintf(stderr, "Connection to failed\n");
		exit(2);
	}
	
	char * SQL_SELECT = "SELECT * FROM students";
	PGresult * res = execute(conn, SQL_SELECT);
	
	count_tuples = PQntuples(res);
	count_fields = PQnfields(res);		

	for(rows = 0; rows < count_tuples; rows++) {
		for(cols = 0; cols < count_fields; cols++) {
			printf(" %8s\t|", PQgetvalue(res, rows, cols));
		}
		puts("\n----------------------------------------------");
	}


	const char * SQL_INSERT = "INSERT INTO students(firstname, lastname, rating) VALUES($1, $2, $3)";
	char firstname[] = "Marta";
	char lastname[] = "Oleg";
	char rating[] = "5";
	
	const char * const paramValues[] = {
		firstname,
		lastname,
		rating
	};

	const int paramFormats[] = { 0, 0, 0 };
	executeParams(
		conn, 
		SQL_INSERT,
		paramValues,
		paramFormats
	);
	
	PQclear(res);
	PQfinish(conn);
	
	return 0;
}
