#include "db.h"
#include "parser.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
	(void)NotUsed;
	int i;
	for(i=0; i<argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

int sql_init(sqlite3 **db) {
    const char *filename = "vanguard.db";
	/*
    const char *sql = "CREATE TABLE IF NOT EXISTS VANGUARD("  \
                        "IP             CHAR(15)  NOT NULL," \
                        "PORT           INT       NOT NULL," \
                        "SIDE           CHAR(7)," \
                        "HOOK           CHAR(12)," \
                        "RULE           CHAR(7));";
	*/
	const char *sql = "CREATE TABLE VANGUARD (" \
						"ID	INTEGER PRIMARY KEY," \
						"IP	CHAR(15)," \
						"PORT	INT," \
						"SIDE	CHAR(7)," \
						"HOOK	CHAR(12)," \
						"RULE	CHAR(7)" \
						");";
    int err;
    char *zErrMsg = 0;

    err = sqlite3_open(filename, db);
    if(err != SQLITE_OK) {
		ERROR("Can't open database: %s\n", sqlite3_errmsg(*db));
		sqlite3_close(*db);
		return -1;
	}

	err = sqlite3_db_config(*db, SQLITE_DBCONFIG_RESET_DATABASE, 1, 0);
	if(err != SQLITE_OK) {
		ERROR("SQL error: %s\n", sqlite3_errmsg(*db));
		sqlite3_close(*db);
        return -1;
	}
	err = sqlite3_exec(*db, "VACUUM", 0, 0, 0);
	if(err != SQLITE_OK) {
		ERROR("SQL error: %s\n", sqlite3_errmsg(*db));
		sqlite3_close(*db);
        return -1;
	}
	err = sqlite3_db_config(*db, SQLITE_DBCONFIG_RESET_DATABASE, 0, 0);
	if(err != SQLITE_OK) {
		ERROR("SQL error: %s\n", sqlite3_errmsg(*db));
		sqlite3_close(*db);
        return -1;
	}

    err = sqlite3_exec(*db, sql, callback, 0, &zErrMsg);
	if(err != SQLITE_OK) {
		ERROR("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(*db);
        return -1;
	}

	return 0;
}

int sql_add_rule(sqlite3 **db, struct van_str_rule_t *rule) {
	char buffer[16];
	int err;

	if(*db) {
		sqlite3_stmt *sql;
		snprintf(buffer, 16, "%u.%u.%u.%u", (rule->ip & 0x000000ff),
											(rule->ip & 0x0000ff00) >> 8,
											(rule->ip & 0x00ff0000) >> 16,
											(rule->ip & 0xff000000) >> 24);
		const char *zsql = "INSERT INTO VANGUARD (IP, PORT, SIDE, HOOK, RULE) VALUES(?, ?, ?, ?, ?)";
		err = sqlite3_prepare_v2(*db, zsql, 73, &sql, NULL);
		if (err != SQLITE_OK) {
			ERROR("Prepare failed: %s\n",sqlite3_errmsg(*db));
			sqlite3_close(*db);
			return -1;
		}

		sqlite3_bind_text(sql, 1, buffer, -1, SQLITE_STATIC);
		sqlite3_bind_int(sql, 2, rule->port);
		sqlite3_bind_text(sql, 3, (GET_SIDE(rule->flags) == INPUT) ? "INPUT" : "OUTPUT", -1, SQLITE_STATIC);
		sqlite3_bind_text(sql, 4, (GET_HOOK_TYPE(rule->flags) == SOURCE) ? "SOURCE" : "DESTINATION", -1, SQLITE_STATIC);
		sqlite3_bind_text(sql, 5, (GET_RULE_TYPE(rule->flags) == ACCEPT) ? "ACCEPT" : (GET_RULE_TYPE(rule->flags) == BLOCK) ? "BLOCK" : "REJECT", -1, SQLITE_STATIC);

		err = sqlite3_step(sql);
		if (err == SQLITE_ERROR || err == SQLITE_BUSY) {
			ERROR("Step failed: %s\n",sqlite3_errmsg(*db));
			sqlite3_close(*db);
			return -1;
		}

		err = sqlite3_finalize(sql);
		if (err != SQLITE_OK) {
			ERROR("Prepare finalize failed: %s\n",sqlite3_errmsg(*db));
			sqlite3_close(*db);
			return -1;
		}
	} else {
		ERROR("database error!");
		return -1;
	}
	return 0;
}

int sql_close(sqlite3 **db) {
	int err;
	if(*db) {
		err = sqlite3_close(*db);
		if(err < 0) {
			ERROR("init database failed!");
			return -1;
		}
	} else {
		ERROR("database invalid!");
		return -1;
	}

	return 0;
}