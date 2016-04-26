#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sysexits.h>
#include <stdbool.h>

#ifdef INPUT_HEX
#define INPUT_BASE 16
#else
#define INPUT_BASE 10
#endif

const char * find_str_end(const char *str){
    while (*str) str++;
    return str;
}

int main(int argc, const char * argv[])
{
    int i;
    int maxLen = 0;
    if (argc == 1) {
        printf("No input\n");
        return 0;
    }

    for (i = 1; i < argc; ++i) {
        if (strlen(argv[i]) > maxLen) {
            maxLen = strlen(argv[i]);
        }
    }

    putchar('\n');
    for (i = 1; i < argc; ++i) {
        const char * endptr = find_str_end(argv[i]);
        char * afterNum;
        uint64_t val = strtoll(argv[i], &afterNum, INPUT_BASE);
        printf("%-*s => ", maxLen, argv[i]);
        if (endptr != afterNum) {
            printf("PARSE ERROR\n");
        }
        else if ((val == 0) && (errno)) {
            printf("RANGE ERROR\n");
            errno = 0;
        }
        else {
#ifdef INPUT_HEX
            printf("%*lld\n", maxLen+1, val);
#else
            printf("%#*llx\n", maxLen+1, val);
#endif

        }
    }
    putchar('\n');

    return 0;
}
