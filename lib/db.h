#ifndef NETVANGUARD_LIB_DB_H_
#define NETVANGUARD_LIB_DB_H_

#include <stdint.h>
#include <sqlite3.h>

#include "kernel.h"

int sql_init(sqlite3 **db);
int sql_add_rule(sqlite3 **db, struct van_str_rule_t *rule);
int sql_close(sqlite3 **db);


#endif /* NETVANGUARD_LIB_DB_H_ */