#ifndef functionList
#include "functionList.h"
#endif
#ifdef ASMBLER_CP
#include "labels.h"
#endif

#ifndef catchNullptr
#define catchNullptr(a) { \
    if ((a) == nullptr) {         \
        fprintf(stderr, "Nullptr caught.\nVariable: %s,\nFile: %s,\nLine: %d,\nFunction: %s\n", #a, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
    return NULLCAUGTH;         \
    }\
}
#endif

enum ArgType {
    TypeNum = (1 << 4),
    TypeReg = (1 << 5),
    TypeRAM = (1 << 6)
};

enum AsmErrors {
    UndefinedCmd = 1,
};

#define DEF_CMD(name, num, ...)      \
    CMD_##name = (num),

#define DEF_CMD_JUMP(name, num, ...) \
    CMD_##name = (num),

enum StackCommands {
    #include "cmd.h"
};

#undef DEF_CMD

#undef DEF_CMD_JUMP

#ifdef ASMBLER_CP
int stackAsmTex(Lines *commandList, FILE *outStream);

int stackAsmBin(Lines *commandList, Label **labels, size_t *labelsNum, FILE *outStream);
#endif