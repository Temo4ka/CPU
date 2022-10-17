#include "stack.h"
#include "asm.h"

enum BinFileErrors {
     BinSignatureErr  = 1,
    BinAsmVersionErr  = 2,
       BinFreadErr    = 3,
    BinDestructedFile = 4,
};

enum CPU_Errors {
    CPU_DoubleDestruction  = 1,
    CPU_DoubleConstruction = 2,
        CPU_OUT_OF_REGS    = 3,
        CPU_OUT_OF_RAM     = 4
};

struct BinFile {
    FILE   *file      =  nullptr  ;
    size_t fileSize   =     0     ;
    size_t asmVersion =     0     ;
    size_t signature  =     0     ;
    int    status     =     0     ;
};

const int   RAM_SIZE  =  400 ;
const int  REGS_SIZE  =   5  ;
const int SIZE_POISON =  -1  ;

struct CPU {
    Elem_t  *Regs   =  nullptr ;
    Elem_t  *RAM    =  nullptr ;
    char    *code   =  nullptr ;
    size_t codeSize =     0    ;
    size_t    ip    =     0    ;
    bool    status  = Destructed;

    Stack stack = {};

    Stack calls = {};
};

int binFileCtor(BinFile *binFile, FILE *stream);

int binFileDtor(BinFile *commands);

int cpuCtor(CPU *cpu, BinFile *binFile);

int cpuDtor(CPU *cpu);

int executeBinary(CPU *cpu, FILE *stream);

int doTexCommands(Lines *commandList, FILE *stream);

void cpuCodeDump(CPU *cpu, const unsigned line, const char *functionName, const char *fileName, FILE *stream);

void cpuRegsDump(CPU *cpu, const unsigned line, const char *functionName, const char *fileName, FILE *stream);