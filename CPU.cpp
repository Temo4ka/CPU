#include "CPU.h"

const int BIN_SIGNATURE = ('C' << 8) + 'P';
const int  ASM_VERSION  =         3       ;
const int    CMD_MASK   =   (1 << 5) - 1  ;
const int DUMP_ELEMENTS =        25       ;

const char SETRED[]   =  "\033[0;31m";
const char SETNONE[]  =  "\033[0;0m" ;
const char SETGREEN[] =  "\033[0;32m";

//const char *SIGNATURE   =       "CP"      ;

//#define CODE_DUMP_ON

//#define REGS_DUMP_ON

//#define STACK_DUMP_ON

#define SHOW_RAM_IMAGE

//#define DUMP_PAUSE

#define codeDump(cpu) { \
    cpuCodeDump((cpu), __LINE__, __PRETTY_FUNCTION__, FILENAME_, stderr);      \
}

#define regsDump(cpu) { \
    cpuRegsDump((cpu), __LINE__, __PRETTY_FUNCTION__, FILENAME_, stderr);      \
}

void myDebugElemPrint(FILE *stream, const Elem_t element);

int showRamImage(CPU *cpu);

int binFileCtor(BinFile *binFile, FILE *stream) {
    catchNullptr(binFile);
    if (binFile -> status == Constructed)
        return BinDestructedFile;

    Header *buffer = (Header *) calloc(1, sizeof(Header));
    catchNullptr(buffer);

    if (fread(buffer, sizeof(Header), 1, stream) != 1)
        return BinFreadErr;

    binFile -> signature  = buffer ->         signature        ;
    binFile -> asmVersion = buffer ->          version         ;
    binFile ->  fileSize  = buffer -> dataSize - sizeof(Header);
    binFile ->    file    =  stream;

    if (binFile -> signature != BIN_SIGNATURE)
        return BinSignatureErr;
    if (binFile -> asmVersion != ASM_VERSION)
        return BinAsmVersionErr;

    binFile ->   status   = Constructed;

    return OK;
}

int cpuCtor(CPU *cpu, BinFile *binFile) {
    catchNullptr(binFile);
    catchNullptr(cpu);
    catchNullptr(binFile -> file);
    if (cpu -> status == Constructed)
        return CPU_DoubleConstruction;

    cpu ->   code   = (char *) calloc(binFile -> fileSize, sizeof(char));
    cpu ->   RAM    = (int *)  calloc(      RAM_SIZE     , sizeof(int) );
    cpu ->   Regs   = (int *)  calloc(      REGS_SIZE    , sizeof(int) );
    cpu -> codeSize =               binFile -> fileSize                 ;

    catchNullptr(cpu -> code);
    catchNullptr(cpu -> RAM );
    catchNullptr(cpu -> Regs);

    if (fread(cpu -> code, sizeof(char), binFile -> fileSize, binFile -> file) != binFile -> fileSize)
        return BinFreadErr;

    stackCtor(&(cpu -> stack), myDebugElemPrint);
    stackCtor(&(cpu -> calls), myDebugElemPrint);

    cpu -> status = Constructed;

    return OK;
}

int cpuDtor(CPU *cpu) {
    catchNullptr(cpu);
    if (cpu -> status == Destructed)
        return CPU_DoubleDestruction;

    int err = 0;

    free(cpu -> code);
    cpu -> code = nullptr;

    free(cpu -> RAM);
    cpu -> RAM  = nullptr;

    free(cpu -> Regs);
    cpu -> Regs = nullptr;

    cpu -> codeSize = SIZE_POISON;

    err = stackDtor(&(cpu -> stack));
    if (err) return err;

    err = stackDtor(&(cpu -> calls));
    if (err) return err;

    stackLogClose();

    cpu -> status = Destructed;

    return OK;
}

int binFileDtor(BinFile *commands) {
    catchNullptr(commands);
    if (commands -> status == Destructed)
        return BinDestructedFile;

    catchNullptr(commands -> file);
    fclose(      commands -> file);

    commands -> status = Destructed;

    return OK;
}

int argDefinition(CPU *cpu, int cmd,  int *argument);

int executeBinary(CPU *cpu, FILE *stream) {
    catchNullptr(cpu);
    catchNullptr(cpu -> code);
    catchNullptr(stream);
    if (cpu -> status == Destructed)
        return CPU_DoubleDestruction;

    int err = 0;

    while (cpu -> ip < cpu -> codeSize) {
        unsigned char  cmd   = ((unsigned char) cpu -> code[cpu -> ip] & CMD_MASK);
        Elem_t      argument =                         0                          ;

#ifdef CODE_DUMP_ON
        codeDump(cpu);
#endif

#ifdef REGS_DUMP_ON
        regsDump(cpu);
#endif

#ifdef STACK_DUMP_ON
        stackDump(&cpu -> stack);
#endif

//#ifdef SHOW_RAM_IMAGE
//        showRamImage(cpu);
//#endif

#ifdef DUMP_PAUSE
        getchar();
#endif

        switch(cmd & CMD_MASK) {

#define DEF_CMD(name, num, arg, ...)                                                    \
        case CMD_##name:                                                                \
             if (arg > 0) {                                                             \
                   cmd   = (unsigned char) CODE[IP];                      \
                argument =                  0                    ;                      \
                                                                                        \
                IP += sizeof(char);                                              \
                err = argDefinition(cpu, cmd, &argument);                               \
                if (err) return err;                                                    \
                                                                                        \
                __VA_ARGS__                                                             \
             } else if (cpu -> stack.size < -arg)                                       \
                    fprintf(stderr, "Not enough Elements :_(\n");                       \
                else {                                                                  \
                    __VA_ARGS__                                                         \
                    IP += sizeof(char);                                          \
                }                                                                       \
             break;

#define DEF_CMD_JUMP(name, num, oper, ...)                                              \
        case CMD_##name:                                                                \
            __VA_ARGS__                                                                 \
            IP += sizeof(char);                                                  \
            if (_POP(&(STACK), &err) oper _POP(&(STACK), &err))   \
                IP = *((int *) ((char *) CODE + IP));              \
            else                                                                        \
                IP += sizeof(int);                                               \
            break;                                                                      \

#include "cmd.h"

#undef DEF_CMD_REC

#undef DEF_CMD_JUMP

#undef DEF_CMD

            default:
                cpu -> ip = cpu -> codeSize;
                break;
        }

        if (err) return err;
    }

#ifdef SHOW_RAM_IMAGE
    showRamImage(cpu);
#endif

    return OK;
}

int argDefinition(CPU *cpu, int cmd, int *argument) {
    catchNullptr(argument);
    catchNullptr(  cpu   );

    if (cmd & TypeReg) {
        *argument += cpu -> code[cpu -> ip];
        cpu -> ip +=      sizeof(char)     ;
    }
    if (cmd & TypeNum) {
        *argument += *((Elem_t *) (cpu -> code + cpu -> ip));
        cpu -> ip +=           sizeof(Elem_t)               ;
    }

    return OK;
}

void myDebugElemPrint(FILE *stream, const Elem_t element) {
    fprintf(stderr, "%d", element);
    fprintf(stream, "%d", element);
}

void cpuCodeDump(CPU *cpu, const unsigned line, const char *functionName, const char *fileName, FILE *stream) {
    fprintf(stream, "-------------------------------CodeDump-------------------------------\n");
    fprintf(stream, "CPU_Code:%08X dumped at %s at %s(%d)\n", cpu -> code, functionName, fileName, line);
    for (size_t current = 0; current < DUMP_ELEMENTS; ++current) {
        fprintf(stream, "%03zu ", current);
    }
    fprintf(stream, "\n");
    for (size_t current = 0; current < DUMP_ELEMENTS; ++current) {
        fprintf(stream, "%03d ", cpu -> code[current]);
    }
    fprintf(stream, "\n");
    for (size_t current = 0; current < DUMP_ELEMENTS; ++current) {
        if (cpu -> ip > current)
            fprintf(stream, "    ");
        else if (cpu -> ip == current)
            fprintf(stream, "^^^\n");
        else
            break;
    }
    for (size_t current = 0; current < DUMP_ELEMENTS; ++current) {
        if (cpu -> ip > current)
            fprintf(stream, "    ");
        else if (cpu -> ip == current)
            fprintf(stream, " ip\n");
        else
            break;
    }
    fprintf(stream, "\n");

#ifdef DUMP_PAUSE
    if (stream == stderr)
        getchar();
#endif
}

void printGreenStar();

void printRedStar();

int showRamImage(CPU *cpu) {
    catchNullptr(cpu       );
    catchNullptr(cpu -> RAM);

    for (size_t curElem = 0; curElem < RAM_SIZE; ++curElem) {
        if (curElem && (curElem % (int) sqrt(RAM_SIZE) == 0))
            printf("\n");
        if (cpu -> RAM[curElem])
            printGreenStar();
        else
            printRedStar();
    }
    printf("\n");
    printf("\n");

    return OK;
}

void printGreenStar() {
    printf("%s*%s", SETGREEN, SETNONE);
}

void printRedStar() {
    printf("%s*%s", SETRED, SETNONE);
}

void cpuRegsDump(CPU *cpu, const unsigned line, const char *functionName, const char *fileName, FILE *stream) {
    fprintf(stream, "-------------------------------RegsDump-------------------------------\n");
    fprintf(stream, "CPU_Regs:%08X dumped at %s at %s(%d)\n", cpu -> Regs, functionName, fileName, line);
    for (size_t current = 0; current < REGS_SIZE; ++current) {
        fprintf(stream, "%03zu ", current);
    }
    fprintf(stream, "\n");
    for (size_t current = 0; current < REGS_SIZE; ++current) {
        fprintf(stream, "%03d ", cpu -> Regs[current]);
    }
    fprintf(stream, "\n");

#ifdef DUMP_PAUSE
    if (stream == stderr)
        getchar();
#endif
}

/*
int doTexCommands(Lines *commandList, FILE *stream) { // TODO: pohavay cringa
    catchNullptr(commandList);
    catchNullptr(stream);

    char  signature[9] =   ""   ;
    int   asmVersion   =    0   ;
    int  numOfCommands =    0   ;
    int       cmd      =    0   ;
    int       err      =    0   ;
    Elem_t   value     =    0   ;

    Stack stack = {};
    stackCtor(&stack, myDebugElemPrint);

    sscanf(commandList->array[0].line, "%s",    signature  );
    sscanf(commandList -> array[1].line, "%d",  &asmVersion  );
    sscanf(commandList -> array[2].line, "%d", &numOfCommands);
    if (strcmp(signature, SIGNATURE) || asmVersion != ASM_VERSION) {
        fprintf(stderr, "Wrong version of input file!\n");
        return FileIN;
    }
    for (size_t currentCommand = 3; currentCommand < numOfCommands; ++currentCommand) {
        sscanf(commandList -> array[currentCommand].line, "%d", &cmd);
        switch(cmd) {
            case CMD_PUSH:
                ++currentCommand;
                sscanf(commandList -> array[currentCommand].line, "%d", &value);
                err |=stackPush(&stack, value);
                break;
            case CMD_POP:
                stackPop(&stack, &err);
                break;
            case CMD_ADD:
                if (stack.size < 2)
                    fprintf(stderr, "Not enough Elements to add :_(\n");
                else
                    err |=stackPush(&stack, stackPop(&stack, &err) + stackPop(&stack, &err));
                break;
            case CMD_DIV:
                if (stack.size < 2)
                    fprintf(stderr, "Not enough Elements to div :_(\n");
                else
                    err |= stackPush(&stack, stackPop(&stack, &err) / stackPop(&stack, &err));
                break;
            case CMD_OUT:
                if (stack.size < 1)
                    fprintf(stderr, "Not enough Elements to Out :_(\n");
                else {
                    value = stackPop(&stack, &err);
                    fprintf(stream, "%d\n", value);
                    err |= stackPush(&stack, value);
                }
                break;
            case CMD_MULT:
                if (stack.size < 2)
                    fprintf(stderr, "Not enough Elements to div :_(\n");
                else
                    err |= stackPush(&stack, stackPop(&stack, &err) * stackPop(&stack, &err));
                break;
            default:
                break;
        }
        if (err)
            return err;
    }

    stackLogClose();

    return OK;
}*/