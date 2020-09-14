#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "cpuinfo.h"

const struct cpuinfo_bitfield_desc_s cpuinf_id[] = {
    {4,"Revision"},
    {12,"Part"},
    {4,"ARM Arch"},
    {4,"Variant"},
    {8,"Implementor"},
    {}
};

static const char *reg_sizes[] = {
    "4K", "8K", "16K", "32K", "64K", "128K", "256K", "512K", 
    "1M", "2M", "4M", "8M", "16M", "32M", "64M", "128M", "256M", "512M",
    "1G", "2G", "4G",
};

static const char *regperm_str(unsigned val) {
    switch(val) {
        case 0: return "P:-- U:--";
        case 1: return "P:RW U:--";
        case 2: return "P:RW U:R-";
        case 3: return "P:RW U:RW";
        case 5: return "P:R- U:--";
        case 6: return "P:R- U:R-";
        default:
            return "P:?? U:??";
    }
}

static char linebuf[256]; // fixed buffer for outputting dynamic strings from interpreter functions

static const char *two_nth_str[] = {
"1", "2", "4", "8", "16", "32", "64", "128", "256", "512", "1K", "2K", "4K", "8K", "16K", "32K"
};

static const char *two_on_nth(unsigned val) {
    if (val < 16) {
        return two_nth_str[val];
    }
    return "invalid";
}

static const char *two_on_nth_granule(unsigned val) {
    if (val == 0) {
        return "no info";
    }
    else if (val > 9) {
        return "reserved";
    }
    else {
        return two_nth_str[val];
    }
    return "invalid";
}

const struct cpuinfo_bitfield_desc_s cpuinf_feat0[] = {
    {4,"ARM inst set"},
    {4,"Thumb inst set"},
    {4,"Jazelle inst set"},
    {4,"ThumbEE inst set"},
    {16,"-"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_feat1[] = {
    {4,"Programmers' model"},
    {4,"Security extensions"},
    {4,"Microcontr. prog model"},
    {4,"Virt. extensions"},
    {4,"Generic timer ext."},
    {12,"-"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_dbgfeat[] = {
    {4,"Coproc. dbg model"},
    {4,"Coproc. secure dbg model"},
    {4,"Memory-mapped dbg model"},
    {4,"Coproc. trace model"},
    {4,"Memory-mapped trace model"},
    {4,"Debug model M"},
    {4,"Perf. monitors"},
    {4,"-"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_mmfr0[] = {
    {4,"VMSA support"},
    {4,"PMSA support"},
    {4,"Cache coherence"},
    {4,"Outer shareable"},
    {4,"TCM support"},
    {4,"Auxiliary registers"},
    {4,"FCSE support"},
    {4,"-"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_mmfr1[] = {
    {4,"L1 Harvard cache VA"},
    {4,"L1 unified cache VA"},
    {4,"L1 Harvard cache s/w"},
    {4,"L1 unified cache s/w"},
    {4,"L1 Harvard cache"},
    {4,"L1 unified cache"},
    {4,"L1 cache test & clean"},
    {4,"Branch predictor"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_mmfr2[] = {
    {4,"L1 Harvard fg prefetch"},
    {4,"L1 Harvard bg prefetch"},
    {4,"L1 Harvard range"},
    {4,"Harvard TLB"},
    {4,"Unified TLB"},
    {4,"Mem barrier"},
    {4,"WFI stall"},
    {4,"HW access flag"},
    {}
};

static const char *mmfr3_cache(unsigned val) {
    switch (val) {
        case 0: return "Not supported";
        case 1: return "Supported";
    }
    return "(invalid)";
}

static const char *mmfr3_bp(unsigned val) {
    switch (val) {
        case 0: return "Not supported";
        case 1: return "Invalidate all";
        case 2: return "Invalidate all, invalidate by MVA";
    }
    return "(invalid)";
}

static const char *mmfr3_cms(unsigned val) {
    switch (val) {
        case 0: return "4 GByte";
        case 1: return "64 GByte";
        case 2: return "1 TByte";
    }
    return "(invalid)";
}

static const char *mmfr3_ss(unsigned val) {
    switch (val) {
        case 0: return "Supported";
        case 15: return "Not supported";
    }
    return "(invalid)";
}

const struct cpuinfo_bitfield_desc_s cpuinf_mmfr3[] = {
    {4,"Cache maintain MVA", mmfr3_cache},
    {4,"Cache maintain set/way", mmfr3_cache},
    {4,"Branch predictor maintenance", mmfr3_bp},
    {4,"Maintenance broadcast"},
    {4,"-"},
    {4,"Transl. table coherent walk"},
    {4,"Cached memory size", mmfr3_cms},
    {4,"Supersection support", mmfr3_ss},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_isar0[] = {
    {4,"Swap instrs"},
    {4,"Bitcount instrs"},
    {4,"Bitfield instrs"},
    {4,"CmpBranch instrs"},
    {4,"Coproc instrs"},
    {4,"Debug instrs"},
    {4,"Divide instrs"},
    {4,"-"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_isar1[] = {
    {4,"Endian instrs"},
    {4,"Exception instrs"},
    {4,"Exception AR instrs"},
    {4,"Extend instrs"},
    {4,"IfThen instrs"},
    {4,"Immediate instrs"},
    {4,"Interwork instrs"},
    {4,"Jazelle instrs"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_isar2[] = {
    {4,"LoadStore instrs"},
    {4,"Memhint instrs"},
    {4,"MultiAccess Interruptible instructions"},
    {4,"Mult instrs"},
    {4,"MultS instrs"},
    {4,"MultU instrs"},
    {4,"PSR AR instrs"},
    {4,"Reversal instrs"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_isar3[] = {
    {4,"Saturate instrs"},
    {4,"SIMD instrs"},
    {4,"SVC instrs"},
    {4,"SynchPrim instrs"},
    {4,"TabBranch instrs"},
    {4,"ThumbCopy instrs"},
    {4,"TrueNOP instrs"},
    {4,"T2 Exec Env instrs"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_isar4[] = {
    {4,"Unprivileged instrs"},
    {4,"WithShifts instrs"},
    {4,"Writeback instrs"},
    {4,"SMC instrs"},
    {4,"Barrier instrs"},
    {4,"SynchPrim_instrs_frac"},
    {4,"PSR_M instrs"},
    {4,"-"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_isar5[] = {
    {32,"-"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_ctr[] = {
    {4,"Icache min words/line", two_on_nth},
    {10,"(zero)"},
    {2,"L1 Icache policy"},
    {4,"Dcache min words/line", two_on_nth},
    {4,"Exclusives Reservation Granule", two_on_nth_granule},
    {4,"Cache Writeback Granule", two_on_nth_granule},
    {1,"(zero)"},
    {3,"(register format)"},
    {}
};

static const char *ctype_str(unsigned val) {
    switch (val) {
        case 0: return "no cache";
        case 1: return "Icache only";
        case 2: return "Dcache only";
        case 3: return "Separate Icache, Dcache";
        case 4: return "Unified cache";
    }
    return "-";
}

const struct cpuinfo_bitfield_desc_s cpuinf_clidr[] = {
    {3,"Cache type, level1", ctype_str},
    {3,"Cache type, level2", ctype_str},
    {3,"Cache type, level3", ctype_str},
    {3,"Cache type, level4", ctype_str},
    {3,"Cache type, level5", ctype_str},
    {3,"Cache type, level6", ctype_str},
    {3,"Cache type, level7", ctype_str},
    {3,"Cache type, level8", ctype_str}, // L8 mentioned only in Cortex R4 TRM
    {3,"Level of coherency"},
    {3,"Level of unification"},
    {2,"(zero)"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_csselr[] = {
    {1,"Instruction, not data"},
    {3,"Level"},
    {28,"(unknown)"},
    {}
};

static const char *ccsidr_linesize(unsigned val) {
    return two_nth_str[val+2];
}

static const char *ccsidr_plusone(unsigned val) {
    sprintf(linebuf,"%i",val+1);
    return linebuf;
}

const struct cpuinfo_bitfield_desc_s cpuinf_ccsidr[] = {
    {3,"Line size in words", ccsidr_linesize},
    {10,"Associativity", ccsidr_plusone},
    {15,"Number of sets", ccsidr_plusone},
    {1,"Write allocation"},
    {1,"Read allocation"},
    {1,"Write back"},
    {1,"Write through"},
    {}
};

static const char *cache_tcm_size_str(unsigned val) {
    if (val == 0) 
        return "0";
    if (val < 3 || val > 14)
        return "invalid";
    return reg_sizes[val-3];
}

static const char *cache_tcm_addr_str(unsigned val) {
    sprintf(linebuf,"0x%08x",val<<12);
    return linebuf;
}

const struct cpuinfo_bitfield_desc_s cpuinf_tcmreg[] = {
    {1,"Enabled"},
    {1,"-"},
    {5,"Size", cache_tcm_size_str},
    {5,"-"},
    {20,"Base address", cache_tcm_addr_str},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_mputype[] = {
    {1,"S"},
    {7,"-"},
    {8,"Num of MPU regions"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_mpubase[] = {
    {32,"Base address"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_sctlr[] = {
    {1,"MPU Enable"},
    {1,"Strict Align"},
    {1,"L1 DCache Enable"},
    {4,"- (SBO)"},
    {4,"- (SBZ)"},
    {1,"Branch Pred Enable"},
    {1,"L1 ICache Enable"},
    {1,"High Vector"},
    {1,"Round Robin"},
    {1,"- (SBZ)"},
    {1,"- (SBO)"},
    {1,"MPU background reg"},
    {1,"- (SBO)"},
    {1,"Div0 exception"},
    {1,"- (SBZ)"},
    {1,"FIQ Enable"},
    {2,"- (SBO)"},
    {1,"VIC"},
    {1,"CPSR E bit"},
    {1,"- (SBZ)"},
    {1,"NMFI"},
    {1,"TRE"},
    {1,"AFE"},
    {1,"Thumb exceptions"},
    {1,"Endian"},
    {}
};

static const char *mpu_region_size_str(unsigned val) {
    if (val < 4 || val > 31)
        return "invalid";
    if (val < 11)
        return two_nth_str[val+1];
    return reg_sizes[val-11];
}

static const char *bitfield8(unsigned val) {
    linebuf[8] = 0;
    int n;
    for (n=0; n<8; n++) {
        linebuf[7-n] = (val & (1<<n))?'1':'0';
    }
    return linebuf;
}

const struct cpuinfo_bitfield_desc_s cpuinf_mpusizeen[] = {
    {1,"Enabled"},
    {5,"Size", mpu_region_size_str},
    {2,"-"},
    {8,"Sub-regions disabled", bitfield8},
    {}
};

static const char *mpu_rattr(unsigned val) {
    char *s="";
    char *s2="";
    char *t;
    t = (val&4)?"Shared":"Non-shared";
    if (val&0x20) {
        switch (val&3) {
            case 0: s = "Inner Non-cacheable"; break;
            case 1: s = "Inner Write-back, write-allocate"; break;
            case 2: s = "Inner Write-through, no write-allocate"; break;
            case 3: s = "Inner Write-back, no write-allocate"; break;
        }
        switch ((val&0x18)>>3) {
            case 0: s2 = "Outer Non-cacheable"; break;
            case 1: s2 = "Outer Write-back, write-allocate"; break;
            case 2: s2 = "Outer Write-through, no write-allocate"; break;
            case 3: s2 = "Outer Write-back, no write-allocate"; break;
        }
        sprintf(linebuf,"%s; %s; %s",s, s2, t);
    }
    else {
        switch (val&0x1B) {
            case 0: s = "Strongly ordered, shareable"; t=""; break;
            case 1: s = "Shareable device"; t="Shareable"; break;
            case 2: s = "Outer and Inner write-through, no write-allocate"; break;
            case 3: s = "Outer and Inner write-back, no write-allocate"; break;
            case 8: s = "Outer and Inner Non-cacheable"; break;
            case 11: s = "Outer and Inner write-back, write-allocate"; break;
            case 16: s = "Non-shareable Device"; t=""; break;
            default: s = "(reserved)"; t="";
        }
        sprintf(linebuf,"%s; %s",s, t);
    }
    return linebuf;
}

const struct cpuinfo_bitfield_desc_s cpuinf_accesscontrol[] = {
    {6,"Region attributes", mpu_rattr},
    {2,"-"},
    {3,"Access permission", regperm_str},
    {1,"-"},
    {1,"Execute never"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_generic[] = {
    {32,"(raw value)"},
    {}
};

static const char * dbg_version(unsigned val) {
    switch(val) {
        case 0b0001: return "v6";
        case 0b0010: return "v6.1";
        case 0b0011: return "v7 full";
        case 0b0100: return "v7 basic";
        case 0b0101: return "v7.1";
        case 0b0110: return "v8";
        case 0b0111: return "v8.1";
        case 0b1000: return "v8.2";
    }
    return "???";
}

static const struct cpuinfo_bitfield_desc_s cpuinf_dbgdidr[] = {
    {4,"Revision"},
    {4,"Variant"},
    {8,"- (RAZ)"},
    {4,"Version",dbg_version},
    {4,"Context",ccsidr_plusone},
    {4,"BRP",ccsidr_plusone},
    {4,"WRP",ccsidr_plusone},
    {}
};

static const struct cpuinfo_bitfield_desc_s cpuinf_dbgd_address[] = {
    {2,"Valid"},
    {10,"- (UNK)"},
    {20,"Address",cache_tcm_addr_str},
    {}
};

static const struct cpuinfo_bitfield_desc_s cpuinf_dbgdscr[] = {
    {1,"HALTED"},
    {1,"RESTARTED"},
    {4,"MOE"},
    {1,"SDABORT_l"},
    {1,"ADABORT_l"},
    {1,"UND_l"},
    {1,"FS"},
    {1,"DBGack"},
    {1,"INTdis"},
    {1,"UDCCdis"},
    {1,"ITRen"},
    {1,"HDBGen"},
    {1,"MDBGen"},
    {1,"SPIDdis"},
    {1,"SPNIDdis"},
    {1,"NS"},
    {1,"ADAdiscard"},
    {2,"ExtDCCmode"},
    {2,"- (SBZ)"},
    {1,"InstrCompl_l"},
    {1,"PipeAdv"},
    {1,"TXfull_l"},
    {1,"RXfull_l"},
    {1,"- (SBZ)"},
    {1,"TXfull"},
    {1,"RXfull"},
    {1,"- (SBZ)"},
    {}
};

const struct cpuinfo_word_desc_s cpuinfo_desc_pmsa[]={
    {"ID", cpuinf_id },
    {"Cache type", cpuinf_ctr },
    {"TCM type", cpuinf_generic },
    {"MPU type", cpuinf_mputype },
    {"Multiprocessor ID", cpuinf_generic },
    {"Processor feature 0", cpuinf_feat0 },
    {"Processor feature 1", cpuinf_feat1 },
    {"Debug feature", cpuinf_dbgfeat },
    {"Aux feature", cpuinf_generic },
    {"Mem model feature 0", cpuinf_mmfr0 },
    {"Mem model feature 1", cpuinf_mmfr1 },
    {"Mem model feature 2", cpuinf_mmfr2 },
    {"Mem model feature 3", cpuinf_mmfr3 },
    {"ISA feature 0", cpuinf_isar0 },
    {"ISA feature 1", cpuinf_isar1 },
    {"ISA feature 2", cpuinf_isar2 },
    {"ISA feature 3", cpuinf_isar3 },
    {"ISA feature 4", cpuinf_isar4 },
    {"ISA feature 5", cpuinf_isar5 },
    {"Cache level ID", cpuinf_clidr },
    {"Cache size ID reg (data, level0)", cpuinf_ccsidr },
    {"Cache size ID reg (inst, level0)", cpuinf_ccsidr },
    {"SCTLR", cpuinf_sctlr },
    {"ACTLR", cpuinf_generic },
    {"ACTLR2", cpuinf_generic },
    {"CPACR", cpuinf_generic },
    {"Build options 1", cpuinf_generic },
    {"Build options 2", cpuinf_generic },
    {"ATCM region reg", cpuinf_tcmreg },
    {"BTCM region reg", cpuinf_tcmreg },
    {"MPU region 0 base", cpuinf_mpubase },
    {"MPU region 0 size & enable", cpuinf_mpusizeen },
    {"MPU region 0 access control", cpuinf_accesscontrol },
    {"MPU region 1 base", cpuinf_mpubase },
    {"MPU region 1 size & enable", cpuinf_mpusizeen },
    {"MPU region 1 access control", cpuinf_accesscontrol },
    {"MPU region 2 base", cpuinf_mpubase },
    {"MPU region 2 size & enable", cpuinf_mpusizeen },
    {"MPU region 2 access control", cpuinf_accesscontrol },
    {"MPU region 3 base", cpuinf_mpubase },
    {"MPU region 3 size & enable", cpuinf_mpusizeen },
    {"MPU region 3 access control", cpuinf_accesscontrol },
    {"MPU region 4 base", cpuinf_mpubase },
    {"MPU region 4 size & enable", cpuinf_mpusizeen },
    {"MPU region 4 access control", cpuinf_accesscontrol },
    {"MPU region 5 base", cpuinf_mpubase },
    {"MPU region 5 size & enable", cpuinf_mpusizeen },
    {"MPU region 5 access control", cpuinf_accesscontrol },
    {"MPU region 6 base", cpuinf_mpubase },
    {"MPU region 6 size & enable", cpuinf_mpusizeen },
    {"MPU region 6 access control", cpuinf_accesscontrol },
    {"MPU region 7 base", cpuinf_mpubase },
    {"MPU region 7 size & enable", cpuinf_mpusizeen },
    {"MPU region 7 access control", cpuinf_accesscontrol },
    {"DBGDIDR", cpuinf_dbgdidr },
    {"DBGDRAR", cpuinf_dbgd_address },
    {"DBGDSAR", cpuinf_dbgd_address },
    {"DBGDSCR", cpuinf_dbgdscr },
    //{"Floating Point System ID register", cpuinf_generic },
    //{"Media and VFP Feature Register 0", cpuinf_generic },
    //{"Media and VFP Feature Register 1", cpuinf_generic },
    {}
};
size_t cpuinfo_desc_pmsa_size = sizeof(cpuinfo_desc_pmsa);

static const char * tlb_unified(unsigned val) {
    switch(val) {
        case 0: return "Unified TLB";
        case 1: return "Separate data and instruction TLB";
    }
    return "???";
}

static const char * tlb_entries(unsigned val) {
    switch(val) {
        case 0: return "64";
        case 1: return "128";
        case 2: return "256";
        case 3: return "512";
    }
    return "???";
}

const struct cpuinfo_bitfield_desc_s cpuinf_tlbtype[] = {
    {1,"TLB", tlb_unified},
    {2,"TLB entries", tlb_entries},
    {5,"-"},
    {8,"Lockable unified or data entries"},
    {8,"Lockable instruction entries"},
    {8,"(zero)"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_sctlr_vmsa[] = {
    {1,"MMU Enable"},
    {1,"Strict Align"},
    {1,"Data or Unified Cache Enable"},
    {1,"CP15 Barrier Enable"},
    {3,"- (SBO)"},
    {3,"- (SBZ)"},
    {1,"SWP/SWPB Enable"},
    {1,"Branch Pred Enable"},
    {1,"ICache Enable"},
    {1,"High Vector"},
    {1,"Round Robin"},
    {1,"- (SBZ)"},
    {1,"- (SBO)"},
    {1,"HA flag"},
    {1,"- (SBO)"},
    {1,"WXN (virt. ext. only)"},
    {1,"UWXN (virt. ext. only)"},
    {1,"FIQ Enable"},
    {2,"- (SBO)"},
    {1,"VE"},
    {1,"CPSR E"},
    {1,"- (SBZ)"},
    {1,"NMFI"},
    {1,"TRE"},
    {1,"AFE"},
    {1,"Thumb exceptions"},
    {1,"- (SBZ)"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_clidr_vmsa[] = {
    {3,"Cache type, level1", ctype_str},
    {3,"Cache type, level2", ctype_str},
    {3,"Cache type, level3", ctype_str},
    {3,"Cache type, level4", ctype_str},
    {3,"Cache type, level5", ctype_str},
    {3,"Cache type, level6", ctype_str},
    {3,"Cache type, level7", ctype_str},
    {3,"Level of unification Inner Shareable"},
    {3,"Level of coherency"},
    {3,"Level of unification"},
    {2,"(zero)"},
    {}
};

static const char *ttbraddr0(unsigned val) {
    val <<= 7;
    sprintf(linebuf,"0x%08x",val);
    return linebuf;
}

static const char *ttbraddr1(unsigned val) {
    val <<= 7;
    sprintf(linebuf,"0x%08x",val);
    return linebuf;
}

static const char *ttbcr_n(unsigned val) {
    val = 128 << (7-val);
    sprintf(linebuf,"TTBR0 table size %u bytes",val);
    return linebuf;
}

const struct cpuinfo_bitfield_desc_s cpuinf_ttbcr[] = {
    {3,"N", ttbcr_n },
    {1,"(zero)" },
    {1,"TTBR0 walks disabled" },
    {1,"TTBR1 walks disabled" },
    {25,"(zero)" },
    {1,"Long descriptors" },
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_ttbr0[] = {
    {1,"IRGN[1]"},
    {1,"Shareable" },
    {1,"(impl. defined)" },
    {2,"RGN (Outer cacheability)" },
    {1,"NOS (Inner shareable)" },
    {1,"IRGN[0]" },
    {25,"Table address", ttbraddr0 },
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_ttbr1[] = {
    {1,"IRGN[1]"},
    {1,"Shareable" },
    {1,"(impl. defined)" },
    {2,"RGN (Outer cacheability)" },
    {1,"NOS (Inner shareable)" },
    {1,"IRGN[0]" },
    {25,"Table address", ttbraddr1 },
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_actlr_vmsa[] = {
    {1,"Cache & TLB maint. broadcast" },
    {1,"L2 prefetch enable" },
    {1,"L1 prefetch enable" },
    {1,"Write full line of zeroes" },
    {2,"(zero)"},
    {1,"SMP" },
    {1,"Exclusive cache" },
    {1,"Alloc in one way" },
    {1,"Parity on" },
    {22,"-"},
    {}
};

const struct cpuinfo_bitfield_desc_s cpuinf_cpacr_vmsa[] = {
    {20,"(zero)"},
    {2,"CP10 access permission" },
    {2,"CP11 access permission" },
    {6,"(zero)"},
    {1,"D32DIS" },
    {1,"ASEDIS" },
    {}
};

const struct cpuinfo_word_desc_s cpuinfo_desc_vmsa[]={
    {"ID", cpuinf_id },
    {"Cache type", cpuinf_ctr },
    {"TCM type", cpuinf_generic },
    {"TLB type", cpuinf_tlbtype },
    {"TTBCR", cpuinf_ttbcr },
    {"TTBR0", cpuinf_ttbr0 },
    {"TTBR1", cpuinf_ttbr1 },
    {"Multiprocessor ID", cpuinf_generic },
    {"Revision ID", cpuinf_generic },
    {"Processor feature 0", cpuinf_feat0 },
    {"Processor feature 1", cpuinf_feat1 },
    {"Debug feature", cpuinf_dbgfeat },
    {"Aux feature", cpuinf_generic },
    {"Mem model feature 0", cpuinf_mmfr0 },
    {"Mem model feature 1", cpuinf_mmfr1 },
    {"Mem model feature 2", cpuinf_mmfr2 },
    {"Mem model feature 3", cpuinf_mmfr3 },
    {"ISA feature 0", cpuinf_isar0 },
    {"ISA feature 1", cpuinf_isar1 },
    {"ISA feature 2", cpuinf_isar2 },
    {"ISA feature 3", cpuinf_isar3 },
    {"ISA feature 4", cpuinf_isar4 },
    {"ISA feature 5", cpuinf_isar5 },
    {"Cache level ID", cpuinf_clidr_vmsa },
    {"Cache size ID reg (data, level0)", cpuinf_ccsidr },
    {"Cache size ID reg (inst, level0)", cpuinf_ccsidr },
    {"SCTLR", cpuinf_sctlr_vmsa },
    {"ACTLR", cpuinf_actlr_vmsa },
    {"ACTLR2", cpuinf_generic },
    {"CPACR", cpuinf_cpacr_vmsa },
    {"DACR", cpuinf_generic },
    //{"ATCM region reg", cpuinf_tcmreg }, // not specified in Cortex A literature
    //{"BTCM region reg", cpuinf_tcmreg }, // not specified in Cortex A literature
    {"NSACR (sec. ext. only)", cpuinf_generic },
    {"DBGDIDR", cpuinf_dbgdidr },
    {"DBGDRAR", cpuinf_dbgd_address },
    {"DBGDSAR", cpuinf_dbgd_address },
    {"DBGDSCR", cpuinf_dbgdscr },
    //{"Floating Point System ID register", cpuinf_generic },
    //{"Media and VFP Feature Register 0", cpuinf_generic },
    //{"Media and VFP Feature Register 1", cpuinf_generic },
    {"Config base addr reg", cpuinf_generic },
    {"PLEIDR", cpuinf_generic },
    {"TLB lockdown reg", cpuinf_generic },
    {"PRRR", cpuinf_generic },
    {"NMRR", cpuinf_generic },
    {}
};
size_t cpuinfo_desc_vmsa_size = sizeof(cpuinfo_desc_vmsa);

char *cpolicies[] = {
    "Cached OUTER 0 INNER 0",
    "Cached OUTER 0 INNER 1",
    "Cached OUTER 0 INNER 2",
    "Cached OUTER 0 INNER 3",
    "Cached OUTER 1 INNER 0",
    "Cached OUTER 1 INNER 1",
    "Cached OUTER 1 INNER 2",
    "Cached OUTER 1 INNER 3",
    "Cached OUTER 2 INNER 0",
    "Cached OUTER 2 INNER 1",
    "Cached OUTER 2 INNER 2",
    "Cached OUTER 2 INNER 3",
    "Cached OUTER 3 INNER 0",
    "Cached OUTER 3 INNER 1",
    "Cached OUTER 3 INNER 2",
    "Cached OUTER 3 INNER 3",
};

unsigned interpret_l1_table_entry(unsigned e, char *buf) {
    unsigned ret = 0, l2a = 0;
    struct l1tblentry_s col;
    col.typ = "";
    col.pbit = "";
    col.ngbit = "";
    col.domain = 0;
    col.physaddr[0] = 0;
    col.l2addr[0] = 0;
    col.sbit = "";
    col.accperm = "";
    col.caching = "";
    col.memtype = "";
    col.xnbit = "";
    int typ = -1;
    switch (e & 3) {
        case 1: typ = 2;
                break;
        case 2: typ = (e & 0x40000)?1:0;
                break;
    }
    if (typ < 0) {
        sprintf(buf,"Fault,");
        return 2;
    }
    if (e & 0x200) {
        col.pbit = "P";
    }
    col.domain = (e >> 5) & 15;
    if (typ == 2) {
        col.typ = "L2 ref";
        l2a = e & 0xfffffc00;
        sprintf(col.l2addr,"0x%08x",l2a);
    }
    else {
        col.typ = typ==1?"Supersection":"Section";
        sprintf(col.physaddr,"0x%08x",e & 0xfff00000);
        col.ngbit = e&0x20000?"Nonglobal":"Global";
        col.sbit = e&0x10000?"Shareable":"";
        col.xnbit = e&0x10?"No exec":"";
        switch (e & 0x8C00) {
            case 0: col.accperm = "--/--"; break;
            case 0x400: col.accperm = "RW/--"; break;
            case 0x800: col.accperm = "RW/R-"; break;
            case 0xC00: col.accperm = "RW/RW"; break;
            case 0x8000: col.accperm = "rsrvd"; break;
            case 0x8400: col.accperm = "R-/--"; break;
            case 0x8800: col.accperm = "R-/R-"; break;
            case 0x8C00: col.accperm = "rsrvd"; break;
        }
        switch (e & 0x700c) {
          case 0: col.caching = "STR ORD"; col.memtype = "Strongly-ordered"; break;
          case 4: col.caching = "SHR DEV"; col.memtype = "Device"; break;
          case 8: col.caching = "WRTHR, NAW"; col.memtype = "Normal"; break;
          case 0xc: col.caching = "WRBCK, NAW"; col.memtype = "Normal"; break;
          case 0x1000: col.caching = "NON CACH"; col.memtype = "Normal"; break;
          case 0x2000: col.caching = "NONSHR DEV"; col.memtype = "Device"; break;
          default:
              if (e & 0x4000) {
                  unsigned i = ((e&0x3000)>>10)|((e&0xc)>>2);
                  col.caching = cpolicies[i];
                  col.memtype = "Normal";
              }
        }
    }
    sprintf(buf,"%s,%s,%s,%u,%s,%s,%s,%s,%s,%s,%s,",col.typ,col.pbit,
            col.ngbit,col.domain,col.physaddr,col.l2addr,col.sbit,col.accperm,
            col.caching,col.memtype,col.xnbit);
    if (typ==1) {
        ret = 1;
    }
    if (l2a!=0) {
        ret = l2a;
    }
    return ret;
}

unsigned interpret_l2_table_entry(unsigned e, char *buf) {
    unsigned ret = 0, f;
    struct l1tblentry_s col;
    col.typ = "";
    col.pbit = "";
    col.ngbit = "";
    col.domain = 0;
    col.physaddr[0] = 0;
    col.l2addr[0] = 0;
    col.sbit = "";
    col.accperm = "";
    col.caching = "";
    col.memtype = "";
    col.xnbit = "";
    int typ = -1;
    switch (e & 3) {
        case 1: typ = 1;
                col.typ = "Large page";
                sprintf(col.physaddr,"0x%08x",e & 0xffff0000);
                col.xnbit = e&0x8000?"No exec":"";
                f = e;
                break;
        case 2:
        case 3: typ = 0;
                col.typ = "Small page";
                sprintf(col.physaddr,"0x%08x",e & 0xfffff000);
                col.xnbit = e&1?"No exec":"";
                f = ((e<<6) & 0x7000) | (e & 0xc); // rearrange bits for evaluation below
                break;
    }
    if (typ < 0) {
        sprintf(buf,"Fault,");
        return 2;
    }
    col.ngbit = e&0x800?"Nonglobal":"Global";
    col.sbit = e&0x400?"Shareable":"";
    switch ((e<<6) & 0x8C00) {
        case 0: col.accperm = "--/--"; break;
        case 0x400: col.accperm = "RW/--"; break;
        case 0x800: col.accperm = "RW/R-"; break;
        case 0xC00: col.accperm = "RW/RW"; break;
        case 0x8000: col.accperm = "rsrvd"; break;
        case 0x8400: col.accperm = "R-/--"; break;
        case 0x8800: col.accperm = "R-/R-"; break;
        case 0x8C00: col.accperm = "rsrvd"; break;
    }
    switch (f & 0x700c) {
      case 0: col.caching = "STR ORD"; col.memtype = "Strongly-ordered"; break;
      case 4: col.caching = "SHR DEV"; col.memtype = "Device"; break;
      case 8: col.caching = "WRTHR, NAW"; col.memtype = "Normal"; break;
      case 0xc: col.caching = "WRBCK, NAW"; col.memtype = "Normal"; break;
      case 0x1000: col.caching = "NON CACH"; col.memtype = "Normal"; break;
      case 0x2000: col.caching = "NONSHR DEV"; col.memtype = "Device"; break;
      default:
          if (f & 0x4000) {
              unsigned i = ((f&0x3000)>>10)|((f&0xc)>>2);
              col.caching = cpolicies[i];
              col.memtype = "Normal";
          }
    }
    sprintf(buf,"%s,%s,%s,,%s,%s,%s,%s,%s,%s,%s,",col.typ,col.pbit,
            col.ngbit,col.physaddr,col.l2addr,col.sbit,col.accperm,
            col.caching,col.memtype,col.xnbit);
    if (typ==1) {
        ret = 1;
    }
    return ret;
}

const char *csvhead = "Virt.addr,Table,Type,P bit,NG bit,Domain,Phys.addr,L2 ref,S bit,Privileged/Nonpriv.,Caching,Memtype,XN bit\n";

// this uses get_mmuregs_vmsa(), which is inline asm.  Disabling for now,
// may not be needed at all.
/*
void memmapping_vmsa(FILE *f) {
    struct mmuregs_s mmuregs;
    get_mmuregs_vmsa(&mmuregs);
    unsigned tt0len = 128 << (7 - (mmuregs.ttbcr & 7));
    unsigned tt0adr = mmuregs.ttbr0 & 0xffffff80;
    unsigned tt1adr = mmuregs.ttbr1 & 0xffffff80;
    unsigned n, *e, r, pr=42, cycl, l1a;
    char *conclude;
    e = (unsigned*)tt0adr;
    cycl = tt0len/4;
    l1a = 0;
    unsigned remain = 2;
    fwrite(csvhead,1,strlen(csvhead),f);
    while (remain) {
        for (n=0; n<cycl; n++) {
            sprintf(linebuf,"0x%08X,L1,",l1a); // virtual address to be described by L1 entry
            fwrite(linebuf,1,strlen(linebuf),f);
            conclude = "\n";
            r = interpret_l1_table_entry(*e, linebuf);
            fwrite(linebuf,1,strlen(linebuf),f);
            if (r==1 && pr!=1) { // supersection begins
                if ((unsigned)e & 0x3f) {
                    conclude = "ERR: Unaligned supersection\n";
                }
                else {
                    unsigned m, v;
                    v = *e;
                    for (m=1; m<16; m++) {
                        if (v != *(e+m)) {
                            conclude = "ERR: Inconsistent supersection\n";
                            break;
                        }
                    }
                }
            }
            else if (r==2) {
                conclude = "\n";
            }
            fwrite(conclude,1,strlen(conclude),f);
            if (r>42) { // interpret L2 table
                unsigned nn, l2a, *ee, rr, prr=42;
                l2a = l1a;
                ee = (unsigned*) r;
                for (nn=0; nn<256; nn++) {
                    sprintf(linebuf,"0x%08X,L2,",l2a); // virtual address to be described by L2 entry
                    fwrite(linebuf,1,strlen(linebuf),f);
                    conclude = "\n";
                    rr = interpret_l2_table_entry(*ee, linebuf);
                    fwrite(linebuf,1,strlen(linebuf),f);
                    
                    if (rr==1 && prr!=1) { // large page begins
                        if ((unsigned)ee & 0x3f) {
                            conclude = "ERR: Unaligned large page\n";
                        }
                        else {
                            unsigned mm, vv;
                            vv = *ee;
                            for (mm=1; mm<16; mm++) {
                                if (vv != *(ee+mm)) {
                                    conclude = "ERR: Inconsistent large page\n";
                                    break;
                                }
                            }
                        }
                    }
                    else if (rr==2) {
                        conclude = "\n";
                    }
                    fwrite(conclude,1,strlen(conclude),f);
                    
                    ee++;
                    prr = rr;
                    l2a += 0x1000;
                }
            }
            e++;
            pr = r;
            l1a += 0x100000;
        }
        e = (unsigned*)(tt1adr+tt0len);
        cycl = (0x4000 - tt0len) / 4;
        remain--;
    }
    fclose(f);
}
*/

uint32_t get_num_cpuinfo_words()
{
    // FIXME - for now, we assume v7
    if (cpuinfo_desc_pmsa_size > cpuinfo_desc_vmsa_size)
    {
        return (cpuinfo_desc_pmsa_size / sizeof(cpuinfo_word_desc_s)) - 1;
    }
    else
    {
        return (cpuinfo_desc_vmsa_size / sizeof(cpuinfo_word_desc_s)) - 1;
    }

    // v5 version
    // sizeof(cpuinfo_desc) / sizeof(cpuinfo_desc[0])) - 1
}

void cpuinfo_write_file(uint32_t *cpuinfo) {
    int i,j;
    unsigned fieldval,wordval;
    unsigned mask,bits;
    char buf[100];
    char *p;

    // FIXME - currently we assume the passed in buffer
    // contains data from a v7 vmsa dump.  We need to 
    // either determine from the buffer, or, take options
    // to this function so we can do the right thing.
    struct cpuinfo_word_desc_s *cpuinfo_desc;
    cpuinfo_desc = (struct cpuinfo_word_desc_s*) cpuinfo_desc_vmsa;
/*
#ifdef THUMB_FW
    struct cpuinfo_word_desc_s *cpuinfo_desc;
    if (cpu_is_vmsa()) {
        FILE *f=fopen("A/MMU_MAP.CSV", "wb");
        memmapping_vmsa(f);
        cpuinfo_get_info_vmsa(cpuinfo);
        cpuinfo_desc = (struct cpuinfo_word_desc_s*) cpuinfo_desc_vmsa;
    }
    else {
        cpuinfo_get_info_pmsa(cpuinfo);
        cpuinfo_desc = (struct cpuinfo_word_desc_s*) cpuinfo_desc_pmsa;
    }
#else
    cpuinfo_get_info(cpuinfo);
#endif
*/
    for(i = 0; cpuinfo_desc[i].name; i++) {
        wordval = cpuinfo[i];
        fprintf(stdout, "%-10s 0x%08X\n", cpuinfo_desc[i].name, wordval);
        for(j=0; cpuinfo_desc[i].fields[j].name; j++) {
            p = buf;
            bits = cpuinfo_desc[i].fields[j].bits;
            mask = ~(0xFFFFFFFF << bits);
            fieldval = wordval & mask;
            p += sprintf(p,"  %-20s 0x%X %d", cpuinfo_desc[i].fields[j].name, fieldval, fieldval);
            if(cpuinfo_desc[i].fields[j].desc_fn) {
                p += sprintf(p, " [%s]", cpuinfo_desc[i].fields[j].desc_fn(fieldval));
            }
            strcat(p, "\n");
            fprintf(stdout, buf);
            wordval >>= bits;
        }
    }
//    sprintf(buf, lang_str(LANG_CPUINFO_WROTE), "A/CPUINFO.TXT");
//    gui_mbox_init(LANG_MENU_DEBUG_CPU_INFO,(int)buf,MBOX_FUNC_RESTORE|MBOX_TEXT_CENTER, cpuinfo_finish);
}

