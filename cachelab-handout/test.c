#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int opt;

    while ((opt = getopt(argc, argv, "s:E:b:t:")) != -1) {
        printf("option = %c, value = %s\n", opt, optarg);
    }

    return 0;
}