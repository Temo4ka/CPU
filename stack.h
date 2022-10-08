#ifndef Elem_t
    #include "stackType.h"
#endif
#include <string.h>
#include <cassert>
#include <cstdio>
#include <stdint.h>
#include <stdlib.h>
#include <cstdarg>

#ifndef PROTECTION
    #define PROTECTION 7
#endif

#define STACK_LOG_INFO 1
#define STACK_CANARY_PROTECTION 2
#define STACK_HASH_PROTECTION 4

enum Status {
    Destructed  = 0,
    Constructed = 1,
};

enum StackErrors {
    StackOk                      =       0,
    StackIsNull                  = 1 <<  0,
    StackNullInData              = 1 <<  1,
    StackCapacityOverflow        = 1 <<  2,
    StackSizeOverflow            = 1 <<  3,
    StackSizeOutOfCapacity       = 1 <<  4,
    StackSizeBelowZero           = 1 <<  5,
    StackDoubleConstruction      = 1 <<  6,
    StackDoubleDestruction       = 1 <<  7,
    StackZeroSizePop             = 1 <<  8,
    StackIsInActive              = 1 <<  9,
    StackLftCanBirdIsDamaged     = 1 << 10,
    StackRgtCanBirdIsDamaged     = 1 << 11,
    StackDataIsDamaged           = 1 << 12,
    StackStructIsDamaged         = 1 << 13

};

struct StackInfo {
    const void*    pointerInfo      =   nullptr  ;
    const char*    declarationPlace =   nullptr  ;
    const char*    declarationFunc  =   nullptr  ;
    const char*    variableName     =   nullptr  ;
    unsigned       line             =      0     ;
};

struct Stack {
    Elem_t *data     =  nullptr  ;
    size_t  size     =     0     ;
    size_t  capacity =     1     ;
    int     koef     =     2     ;
    bool    status   = Destructed;

    #if (PROTECTION & STACK_CANARY_PROTECTION)
        uint64_t LftVictimToGods = 0xDED33DEAD;
        uint64_t RgtVictimToGods = 0xBAD99F00D;
    #endif

    void (*printElem_t) (FILE *stream, const Elem_t stackElement) = nullptr;

    #if (PROTECTION & STACK_LOG_INFO)
        StackInfo info = {}; // can be optional, new debug lvl
    #endif

    #if (PROTECTION & STACK_HASH_PROTECTION)
        uint64_t dataGnuHash   = 0;
        uint64_t structGnuHash = 0;
    #endif
};

const size_t SIZE_LIMIT = 1e6;

const int dumpFree = 1;

#ifndef POISON
    const Elem_t POISON = 1e9 + 17;
#endif

#if (PROTECTION & STACK_LOG_INFO)
    #define stackDump(stack) { stackDump_((stack), __PRETTY_FUNCTION__, FILENAME_, __LINE__);}
#endif

#define FILENAME_ (strrchr(__FILE__, '/') + 1)
#define stackCtor(stack, printElem_t) { stackConstructor((stack),  #stack, __PRETTY_FUNCTION__, FILENAME_, __LINE__, (printElem_t)); }

int stackConstructor(Stack *stack, const char* variableName, const char* declarationFunc, const char* declarationPlace,
                     const unsigned line, void (*printElem_t)(FILE *stream, const Elem_t stackElement) = nullptr);

int stackDtor(Stack *stack);

int stackPush(Stack *stack, Elem_t value);

Elem_t stackPop(Stack *stack, int *er = nullptr);

int stackResize(Stack *stack, int *er = nullptr);

void *recalloc(Elem_t *memPointer, size_t numberOfElements, size_t needNumOfElements, size_t sizeOfElement);

void fillPoison(Elem_t *memPointer, size_t size);

void stackLogClose();


#if (PROTECTION & STACK_CANARY_PROTECTION)
void stackPrepareVictims(Stack *stack);

bool LftCanBirdError(Stack *stack);

bool RgtCanBirdError(Stack *stack);
#endif

#if (PROTECTION & STACK_HASH_PROTECTION)
bool structGnuHashError(Stack *stack);

bool dataGnuHashError(Stack *stack);

void stackMakeHash(Stack *stack);

uint64_t getGnuHash(const void *memPointer, size_t totalBytes);
#endif

#if (PROTECTION & STACK_LOG_INFO)
int stackError(Stack *stack);

void stackDump_(Stack *stack, const char* functionName, const char *fileName, unsigned line);

void printErrorMessage(int error);
#endif