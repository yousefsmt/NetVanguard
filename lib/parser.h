#ifndef NETVANGUARD_LIB_PARSER_H_
#define NETVANGUARD_LIB_PARSER_H_

#include <signal.h>
#include "types.h"

#define VER_MAJ ( LINUX_VERSION_CODE >> 16 )
#define VER_MIN ( ( LINUX_VERSION_CODE - ( VER_MAJ << 16 ) ) >> 8 )
#define VER_PAT ( LINUX_VERSION_CODE - ( VER_MAJ << 16 ) - ( VER_MIN << 8 ) )

#define DEBUG( msg... )   pr_log( DEBUG, msg )
#define SUCCESS( msg... ) pr_log( SUCCESS, msg )
#define WARNING( msg... ) pr_log( WARNING, msg )
#define ERROR( msg... )   pr_log( ERROR, msg )

void handler_signal(int signal);
int cli_parser(struct van_cli_t* van_cli, int argc, char* argv[]);
void pr_log( enum van_log_t level, const char * fmt, ...);

#endif /* NETVANGUARD_LIB_PARSER_H_ */