#include <stdio.h>
#include <stdint.h>

#include "cpuinfo.h"

void print_usage(void)
{
    printf("Typical usage: ./parser cpuinfo_r6.dat\n");
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        print_usage();
        return -1;
    }

    // get saved info dumped from cam, typically CPUINFO.DAT
    FILE *fp = NULL;
    const size_t num_cpuinfo_words = get_num_cpuinfo_words();
    uint32_t cpuinfo[num_cpuinfo_words];
    fp = fopen(argv[1], "rb");
    if (fp == NULL)
        return -1;
    fread(cpuinfo, sizeof(uint32_t), num_cpuinfo_words, fp);

    // write out description
    cpuinfo_write_file(cpuinfo);

    return 0;
}
