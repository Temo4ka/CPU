#include "functionList.h"
#include "asm.h"
#include "stack.h"


enum BinFileErrors {
     BinSignatureErr  = 1,
    BinAsmVersionErr  = 2,
       BinFreadErr    = 3,
    BinDestructedFile = 4,
};

enum CPU_Errors {
        CPU_Destructed     = 1,
    CPU_DoubleConstruction = 2,
};

struct BinFile {
    FILE   *file      =  nullptr  ;
    size_t fileSize   =     0     ;
    int    status     =     0     ;
    int   signature   =     0     ;
    int  asmVersion   =     0     ;
};

const int  RAM_SIZE = 10000;
const int REGS_SIZE = 5;

struct CPU {
    int     *Regs   =  nullptr ;
    int     *RAM    =  nullptr ;
    char    *code   =  nullptr ;
    size_t codeSize =     0    ;
    size_t    ip    =     0    ;
    bool    status  = Destructed;

    Stack stack = {};
};

int BinFileCtor(BinFile *commands, FILE *stream);

int BinFileDtor(BinFile *commands);

int cpuCtor(CPU *cpu, BinFile *binFile);

int cpuDtor(CPU *cpu);

int doBinCommands(CPU *cpu, FILE *stream);

int doTexCommands(Lines *commandList, FILE *stream);