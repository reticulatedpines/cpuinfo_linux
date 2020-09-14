#include <stdlib.h>
#include <stdint.h>
//#include "gui_mbox.h"
//#include "gui.h"
//#include "gui_lang.h"
//#include "lang.h"

struct mmuregs_s {
    unsigned    ttbr0;
    unsigned    ttbr1;
    unsigned    ttbcr;
};

struct l1tblentry_s {
    char *typ;
    char *pbit;
    unsigned domain;
    char physaddr[12];
    char l2addr[12];
    char *ngbit;
    char *sbit;
    char *accperm;
    char *caching;
    char *memtype;
    char *xnbit;
};

extern size_t cpuinfo_desc_pmsa_size;
extern size_t cpuinfo_desc_vmsa_size;

struct cpuinfo_bitfield_desc_s {
    unsigned bits;
    const char *name;
    const char *(*desc_fn)(unsigned val);
};

typedef struct cpuinfo_word_desc_s {
    const char *name;
    const struct cpuinfo_bitfield_desc_s *fields;
} cpuinfo_word_desc_s;

uint32_t get_num_cpuinfo_words();
void cpuinfo_write_file(uint32_t *cpuinfo);
void cpuinfo_finish(unsigned dummy);
