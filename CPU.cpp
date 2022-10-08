#include "CPU.h"

const int BIN_SIGNATURE = 'C' * 256 + 'P';
const int  ASM_VERSION  =       3        ;
const int    CMD_MASK   =       15       ;
const int DUMP_ELEMENTS =       25       ;
const char *SIGNATURE   =      "CP"      ;

//#define DUMP_PAUSE

#define codeDump(cpu) { \
    cpuCodeDump((cpu), __LINE__, __PRETTY_FUNCTION__, FILENAME_, stderr);      \
}

void myDebugElemPrint(FILE *stream, const Elem_t element);

int BinFileCtor(BinFile *binFile, FILE *stream) {
    catchNullptr(binFile);
    if (binFile -> status == Constructed)
        return BinDestructedFile;

    int *buffer = (int *) calloc(3, sizeof(int));
    catchNullptr(buffer);

    if (fread(buffer, sizeof(int), 3, stream) != 3)
        return BinFreadErr;

    catchNullptr(buffer);

    binFile -> signature  = *buffer;
    ++buffer;
    binFile -> asmVersion = *buffer;
    ++buffer;
    binFile ->  fileSize  = *buffer - 3 * sizeof(int);
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

    cpu -> status = Constructed;

    return OK;
}

int cpuDtor(CPU *cpu) {
    catchNullptr(cpu);
    if (cpu -> status == Destructed)
        return CPU_Destructed;

    free(cpu -> code);
    free(cpu -> RAM );
    free(cpu -> Regs);

    cpu ->   code   = nullptr;
    cpu -> codeSize =   -1   ;

    int err = stackDtor(&(cpu -> stack));
    if (err)
        return err;

    stackLogClose();

    cpu -> status = Destructed;

    return OK;
}

int BinFileDtor(BinFile *commands) {
    catchNullptr(commands);
    if (commands -> status == Destructed)
        return BinDestructedFile;

    catchNullptr(commands -> file);
    fclose(      commands -> file);

    commands -> status = Destructed;

    return OK;
}

int doTexCommands(Lines *commandList, FILE *stream) {
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
            case PUSH:
                ++currentCommand;
                sscanf(commandList -> array[currentCommand].line, "%d", &value);
                err |=stackPush(&stack, value);
                break;
            case POP:
                stackPop(&stack, &err);
                break;
            case ADD:
                if (stack.size < 2)
                    fprintf(stderr, "Not enough Elements to add :_(\n");
                else
                    err |=stackPush(&stack, stackPop(&stack, &err) + stackPop(&stack, &err));
                break;
            case DIV:
                if (stack.size < 2)
                    fprintf(stderr, "Not enough Elements to div :_(\n");
                else
                    err |= stackPush(&stack, stackPop(&stack, &err) / stackPop(&stack, &err));
                break;
            case OUT:
                if (stack.size < 1)
                    fprintf(stderr, "Not enough Elements to Out :_(\n");
                else {
                    value = stackPop(&stack, &err);
                    fprintf(stream, "%d\n", value);
                    err |= stackPush(&stack, value);
                }
                break;
            case PRODUCT:
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
}

int doBinCommands(CPU *cpu, FILE *stream) {
    catchNullptr(cpu);
    catchNullptr(cpu -> code);
    catchNullptr(stream);
    if (cpu -> status == Destructed)
        return CPU_Destructed;

    int err = 0;

    while (cpu -> ip < cpu -> codeSize) {
        char     cmd    = 0;
        Elem_t argument = 0;
        codeDump(cpu);

        switch((cpu -> code[cpu -> ip] & CMD_MASK)) {
            case PUSH:
                    cmd  = cpu -> code[cpu -> ip];
                argument = 0;

                cpu -> ip += sizeof(char);

                if (cmd & TypeReg) {
                    argument  += cpu -> code[cpu -> ip];
                    cpu -> ip += sizeof(char);
                }
                if (cmd & TypeNum) {
                    argument  += *((Elem_t *) (cpu -> code + cpu -> ip));
                    cpu -> ip += sizeof(Elem_t);
                }
                if (cmd & TypeRAM)
                    argument = cpu -> RAM[argument];

                err |= stackPush(&(cpu -> stack), argument);
                break;

            case POP:
                cmd  = cpu -> code[cpu -> ip];
                argument = 0;

                cpu -> ip += sizeof(char);

                if (cmd & TypeReg) {
                    argument  += cpu -> code[cpu -> ip];
                    cpu -> ip += sizeof(char);
                }
                if (cmd & TypeNum) {
                    argument  += cpu -> code[cpu -> ip];
                    cpu -> ip += sizeof(Elem_t);
                }

                //TODO: OutOfArrayErr

                if (cmd & TypeRAM)
                    cpu -> RAM[argument]  = stackPop(&(cpu -> stack), &err);
                else if (cmd & TypeRAM)
                    cpu -> Regs[argument] = stackPop(&(cpu -> stack), &err);
                else
                    stackPop(&(cpu -> stack), &err);
                break;

            case ADD:
                if (cpu -> stack.size < 2)
                    fprintf(stderr, "Not enough Elements to add :_(\n");
                else {
                    err |=stackPush(&(cpu -> stack), stackPop(&(cpu -> stack), &err) + stackPop(&(cpu -> stack), &err));
                    cpu -> ip += sizeof(char);
                }
                break;

            case DIV:
                if (cpu -> stack.size < 2)
                    fprintf(stderr, "Not enough Elements to div :_(\n");
                else {
                    err |= stackPush(&(cpu -> stack), stackPop(&(cpu -> stack), &err) / stackPop(&(cpu -> stack), &err));
                    cpu -> ip += sizeof(char);
                }
                break;

            case OUT:
                if (cpu -> stack.size < 1)
                    fprintf(stderr, "Not enough Elements to Out :_(\n");
                else {
                    Elem_t value = stackPop(&(cpu -> stack), &err);
                    fprintf(stream, "%d\n", value);
                    err |= stackPush(&(cpu -> stack), value);

                    cpu -> ip += sizeof(char);
                }
                break;

            case PRODUCT:
                if (cpu -> stack.size < 2)
                    fprintf(stderr, "Not enough Elements to div :_(\n");
                else {
                    err |= stackPush(&(cpu -> stack), stackPop(&(cpu -> stack), &err) * stackPop(&(cpu -> stack), &err));
                    cpu -> ip += sizeof(char);
                }
                break;

            default:
                break;
        }
        if (err)
            return err;
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