#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include <cjson/cJSON.h>


struct PGconfig {
	char hostaddr[16];
	char port[6];
	char dbname[255];
	char user[255];
	char password[255];
};


PGconn * pg_connect(char * hostaddr, char * port, char * dbname, char * user, char * password) {	
	const char *keywords[] = {
		"hostaddr",
		"port",
		"dbname",
		"user",
		"password",
		NULL
	};

	const char *values[] = {
		hostaddr,
		port,
		dbname,
		user,
		password,
		NULL
	};
	
	size_t z = 0;
	PGconn * conn = PQconnectdbParams( (const char **) keywords, (const char **) values, z);
	

	if(PQstatus(conn) != CONNECTION_OK) {
		fprintf(stderr, "Connection to database %s failed\n", dbname);
		PQfinish(conn);
		exit(1);
	}

	printf("Connection to database %s SUCCESS\n", dbname);
	
	return conn;
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

struct PGconfig load_config(char * fname) {
	const char * READ_MODE = "r";
	FILE *fptr = fopen(fname, READ_MODE);
	
	if(fptr == NULL) {
		fprintf(stderr, "Failed to open file: %s\n", fname);
		exit(4);
	}

	fseek(fptr, 0, SEEK_END);
	long length = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	
	char * json_buffer = malloc( length + 1);

	int len = fread(json_buffer, 1, length, fptr);
	fclose(fptr);

	json_buffer[length] = '\0';

	if( len == 0 ) {
		printf("Error, config file is empty\n");
		exit(6);
	}

	cJSON * json = cJSON_Parse(json_buffer);
	
	if( json == NULL) {
		const char * error_ptr = cJSON_GetErrorPtr();
		
		if(error_ptr != NULL) {
			fprintf(stderr, "Error json read: %s\n", error_ptr);
		}

		free(json_buffer);
		cJSON_Delete(json);
		exit(5);
	}

	struct PGconfig config;

	cJSON * hostaddr = cJSON_GetObjectItemCaseSensitive(json, "hostaddr");

	if(cJSON_IsString(hostaddr) && ( hostaddr->valuestring != NULL )) {
		strncpy(config.hostaddr, hostaddr->valuestring, strlen(hostaddr->valuestring));
		config.hostaddr[ strlen(hostaddr->valuestring) ] = '\0';	
	}

	cJSON * port = cJSON_GetObjectItemCaseSensitive(json, "port");
	
	if(cJSON_IsString(port) && ( port->valuestring != NULL)) {
		strncpy(config.port,port->valuestring, strlen(port->valuestring));
		config.port[strlen(port->valuestring)] = '\0';
	}
	
	cJSON * dbname = cJSON_GetObjectItemCaseSensitive(json, "dbname");
	
	if(cJSON_IsString(dbname) && ( dbname->valuestring != NULL)) {
		strncpy(config.dbname,dbname->valuestring, strlen(dbname->valuestring));
		config.dbname[strlen(dbname->valuestring)] = '\0';
	}

	cJSON * user = cJSON_GetObjectItemCaseSensitive(json, "user");
	
	if(cJSON_IsString(user) && ( user->valuestring != NULL)) {
		strncpy(config.user, user->valuestring, strlen(user->valuestring) );
		config.user[ strlen(user->valuestring)] = '\0';
	}
	
	cJSON * password = cJSON_GetObjectItemCaseSensitive(json, "password");
	
	if(cJSON_IsString(password) && ( password->valuestring != NULL )) {
		strncpy(config.password, password->valuestring, strlen(password->valuestring));
		config.password[strlen(password->valuestring)] = '\0';
	}

	printf("hostaddr = %s\n", config.hostaddr);
	printf("user = %s\n", config.user);

	cJSON_Delete(json);
	return config;	
}

int main() {
	struct PGconfig cfg = load_config("pgconfig.json");
	
	PGconn * conn = pg_connect( 
		cfg.hostaddr, 
		cfg.port, 
		cfg.dbname, 
		cfg.user,
		cfg.password
	);
	printf("%p \n", conn);
	int rows = 0;
	int cols = 0;
	int count_tuples = 0;
	int count_fields = 0;


	if(PQstatus(conn) == CONNECTION_BAD) {
		fprintf(stderr, "Connection to failed\n");
		exit(2);
	}
	
	PGresult * res = execute(conn, "select * from students");
	
	count_tuples = PQntuples(res);
	count_fields = PQnfields(res);		

	for(rows = 0; rows < count_tuples; rows++) {
		for(cols = 0; cols < count_fields; cols++) {
			printf("%s\t\t", PQgetvalue(res, rows, cols));
		}
		puts("");
	}
	
	PQclear(res);
	PQfinish(conn);
	
	return 0;
}

