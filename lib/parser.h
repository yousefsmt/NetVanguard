#ifndef NETVANGUARD_LIB_PARSER_H_
#define NETVANGUARD_LIB_PARSER_H_

#include <signal.h>
#include "types.h"

#define DEBUG( msg... )   debug_msg(DEBUG, msg )
#define SUCCESS( msg... ) debug_msg(SUCCESS, msg )
#define WARNING( msg... ) debug_msg(WARNING, msg )
#define ERROR( msg... )   debug_msg(ERROR, msg )

int  handler_options( int argc, char *argv[] );
void handler_signal( int signal );

void debug_msg( enum van_log_t level, const char * fmt, ...);

#endif /* NETVANGUARD_LIB_PARSER_H_ */