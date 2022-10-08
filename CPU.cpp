#include "CPU.h"


const int BIN_SIGNATURE = 'C' * 256 + 'P';
const int  ASM_VERSION  =       3        ;
const int    CMD_MASK   =       31       ;
const char *SIGNATURE   =      "CP"      ;

void myDebugElemPrint(FILE *stream, const Elem_t element);

int BinFileCtor(BinFile *commands, FILE *stream) {
    catchNullptr(commands);
    if (commands -> status == Constructed)
        return BinDestructedFile;

    int *buffer = (int *) calloc(3, sizeof(int));
    catchNullptr(buffer);

    if (fread(buffer, sizeof(int), 3, stream) != 3)
        return BinFreadErr;

    catchNullptr(buffer);

    commands -> signature  = *buffer;
    ++buffer;
    commands -> asmVersion = *buffer;
    ++buffer;
    commands ->  fileSize  = *buffer;
    commands ->    file    =  stream;

    if (commands -> signature != BIN_SIGNATURE)
        return BinSignatureErr;
    if (commands -> asmVersion != ASM_VERSION)
        return BinAsmVersionErr;

    commands ->   status   = Constructed;

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
        switch(cpu -> code[cpu -> ip]) {
            case PUSH:
                cpu -> ip += sizeof(int);
                err |=stackPush(&(cpu -> stack), cpu -> code[cpu -> ip]);
                break;
            case POP:
                stackPop(&(cpu -> stack), &err);
                break;
            case ADD:
                if (cpu -> stack.size < 2)
                    fprintf(stderr, "Not enough Elements to add :_(\n");
                else
                    err |=stackPush(&(cpu -> stack), stackPop(&(cpu -> stack), &err) + stackPop(&(cpu -> stack), &err));
                break;
            case DIV:
                if (cpu -> stack.size < 2)
                    fprintf(stderr, "Not enough Elements to div :_(\n");
                else
                    err |= stackPush(&(cpu -> stack), stackPop(&(cpu -> stack), &err) / stackPop(&(cpu -> stack), &err));
                break;
            case OUT:
                if (cpu -> stack.size < 1)
                    fprintf(stderr, "Not enough Elements to Out :_(\n");
                else {
                    Elem_t value = stackPop(&(cpu -> stack), &err);
                    fprintf(stream, "%d\n", value);
                    err |= stackPush(&(cpu -> stack), value);
                }
                break;
            case PRODUCT:
                if (cpu -> stack.size < 2)
                    fprintf(stderr, "Not enough Elements to div :_(\n");
                else
                    err |= stackPush(&(cpu -> stack), stackPop(&(cpu -> stack), &err) * stackPop(&(cpu -> stack), &err));
                break;
            default:
                break;
        }
        if (err)
            return err;
        cpu -> ip += sizeof(int);
    }


    return OK;
}

void myDebugElemPrint(FILE *stream, const Elem_t element) {
    fprintf(stderr, "%d", element);
    fprintf(stream, "%d", element);
}