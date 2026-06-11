#ifndef NETVANGUARD_CONFIG_TYPES_H_
#define NETVANGUARD_CONFIG_TYPES_H_

enum van_log_t
{
    DEBUG,
    SUCCESS,
    WARNING,
    ERROR,
    __LOG_MAX
};

#define LOG_MAX ( __LOG_MAX - 1 )

#ifdef __EMSCRIPTEN
    #define NO_ANSI
#endif

#endif /* NETVANGUARD_CONFIG_TYPES_H_ */