#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <cstdint>
#include "functionList.h"

void swap(void *aParam, void *bParam, size_t sizeOfElement) {
    assert(aParam != nullptr || bParam != nullptr);

    char *a = (char *) aParam;
    char *b = (char *) bParam;

    while (sizeOfElement >= sizeof(uint64_t)) {
        uint64_t temp64 = *(uint64_t *)a;
        *(uint64_t *) a = *(uint64_t *)b;
        *(uint64_t *) b = temp64;

        a += sizeof(uint64_t);
        b += sizeof(uint64_t);
        sizeOfElement -= sizeof(uint64_t);
    }

    while (sizeOfElement >= sizeof(uint32_t)) {
        uint32_t temp32 = *(uint32_t *)a;
        *(uint32_t *) a = *(uint32_t *)b;
        *(uint32_t *) b = temp32;

        a += sizeof(uint32_t);
        b += sizeof(uint32_t);
        sizeOfElement -= sizeof(uint32_t);
    }
    while (sizeOfElement >= sizeof(uint16_t)) {
        uint16_t temp16 = *(uint16_t *)a;
        *(uint16_t *) a = *(uint16_t *)b;
        *(uint16_t *) b = temp16;

        a += sizeof(uint16_t);
        b += sizeof(uint16_t);
        sizeOfElement -= sizeof(uint16_t);
    }
    while (sizeOfElement >= sizeof(uint8_t)) {
        uint8_t temp8 = *(uint8_t *)a;
        *(uint8_t *) a = *(uint8_t *)b;
        *(uint8_t *) b = temp8;

        a += sizeof(uint8_t);
        b += sizeof(uint8_t);
        sizeOfElement -= sizeof(uint8_t);
    }
}

void linesDestructor(Lines *strings){
    free(strings -> array);
    strings ->      array    = nullptr;
    strings -> numberOfLines =   -1   ;
}

void lineConstructor(Line *line, char *string, size_t numberOfElements) {
    assert( line  != nullptr);
    assert(string != nullptr);

    line ->  line  =      string     ;
    line -> length = numberOfElements;
}

void linesConstructor(Lines *strings, size_t num) {
    if (strings == nullptr)
        return;

    strings -> numberOfLines =                  num               ;
    strings ->     array     = (Line *) calloc (num, sizeof(Line));
}

void myPuts(const char *string) {
    while (*string != '\0'){
        putchar(*string);
        string++;
    }
    putchar('\n');
}

const char *myStrchr(const char *string, int symbol) {
    assert(string != nullptr);

    const char *currentSymbol = string;

    while (*currentSymbol != '\0') {
        if (*currentSymbol == symbol)
            return currentSymbol;
        currentSymbol++;
    }

    return nullptr;
}

size_t myStrlen(const char *string) {
    const char *currentSymbol = string;

    while (*currentSymbol != '\0')
        currentSymbol++;

    return currentSymbol - string;
}

char *myStrcpy(char *resultOfCopy, const char *origin) {
    size_t currentIndex = 0;

    for ( ; origin[currentIndex] != '\0'; currentIndex++) {
        resultOfCopy[currentIndex] = origin[currentIndex];
    }
    resultOfCopy[currentIndex] = '\0';

    return resultOfCopy;
}

char *myStrncpy(char *resultOfCopy, const char *origin, size_t num) {
    size_t currentIndex = 0;

    while (currentIndex < num && origin[currentIndex] != '0'){
        resultOfCopy[currentIndex] = origin[currentIndex];
        currentIndex++;
    }
    resultOfCopy[currentIndex] = '\0';

    return resultOfCopy;
}

char *myStrcat(char *destptr, const char *srcptr) {
    char *result = destptr;
    size_t resultIndex = myStrlen(result);

    for (size_t srcptrIndex = 0; srcptr[srcptrIndex] != '\0'; srcptrIndex++) {
        result[resultIndex] = srcptr[srcptrIndex];
        resultIndex++;
    }
    result[resultIndex] = '\0';

    return result;
}

char *myStrncat(char *destptr, const char *srcptr, size_t num) {
    char *result = destptr;
    size_t resultIndex = myStrlen(result);

    for (size_t srcptrIndex = 0; srcptr[srcptrIndex] != '\0' && srcptrIndex < num; srcptrIndex++) {
        result[resultIndex] = srcptr[srcptrIndex];
        resultIndex++;
    }
    result[resultIndex] = '\0';

    return result;
}

char *myStrstr(char *str, const char *substr) {
    char *result = str;

    if (strlen(str) < strlen(substr))
        return nullptr;

    while (*result != '\0'){
        bool equality = true;
        for (size_t numberOfCheckedSymbols = 0; substr[numberOfCheckedSymbols] != '\0'; numberOfCheckedSymbols++)
            if (substr[numberOfCheckedSymbols] != *(result + numberOfCheckedSymbols)) {
                equality = false;
                break;
            }
        if (equality)
            return result;
        result++;
    }

    return nullptr;
}

char *myStrtok(char *string, const char *delimiter) {
    static char *input = nullptr;

    if (string != nullptr)
        input = string;

    if (input == nullptr)
        return nullptr;

    size_t passedSymbols = 0;

    while (myStrchr(delimiter, input[passedSymbols]) != nullptr)
        input++;

    while (input[passedSymbols] != '\0'){
        if (myStrchr(delimiter, input[passedSymbols]) != nullptr)
            break;
        //result[passedSymbols] = input[passedSymbols];
        passedSymbols++;
    }

    char *result = (char*) calloc(passedSymbols, sizeof(char));
    for (size_t currentSymbol = 0; currentSymbol < passedSymbols; currentSymbol++)
        result[currentSymbol] = input[currentSymbol];

    input += passedSymbols;

    if (*result == '\0') {
        input = nullptr;
        result = nullptr;
    }

    return result;
}
