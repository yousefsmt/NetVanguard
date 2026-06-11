#include "types.h"
#include "parser.h"


int main( int argc, char *argv[] )
{
    int ret;

    /* handle options */
    ret = handler_options( argc, argv );
    if ( ret < 0 )
    {
        ERROR( "handle options procces failed!");
        return -1;
    }

    /* signal handler */
    signal( SIGINT,  handler_signal );
    signal( SIGKILL, handler_signal );
    signal( SIGTERM, handler_signal );
    signal( SIGSEGV, handler_signal );

    /* init strcuture */

    return 0;
}