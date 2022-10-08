#include "functionList.h"
#include <sys/types.h>
#include <sys/stat.h>

static const char *OUTPUT_FILENAME = "SortedStrings.txt";
static const char *INPUT_FILENAME  =          "poem.txt";

FILE *logFile = fopen("errors.txt", "w");

#define catchNullptr(a) { \
    if ((a) == nullptr) {         \
        fprintf(logFile, "Nullptr caught.\nVariable: %s,\nFile: %s,\nLine: %d,\nFunction: %s\n", #a, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
    return NULLCAUGTH;         \
    }\
}

#define logPrint(s) {\
    fprintf(logFile, "%s.\nFile: %s,\nLine: %d,\nFunction: %s\n", (s), __FILE__, __LINE__, __PRETTY_FUNCTION__);\
}

int TEXTConstructor(Text *txt, const char *fileName) {

    catchNullptr(fileName);
    catchNullptr(txt);

    txt -> file     = fopen(fileName, "r");
    txt -> fileName = fileName;

    catchNullptr(txt -> file);

    txt -> fileSize = getFileSize(txt -> fileName) ;
    if (txt -> fileSize < 0)
        return FileIN;

    txt -> buffer = (char *) calloc(txt -> fileSize + 1, sizeof(char));
    size_t gotSymbols = fread(txt -> buffer, sizeof(char), txt -> fileSize, txt -> file);
    txt -> buffer[gotSymbols] = '\0';

    if (!feof(txt -> file)) {
        logPrint("An Error in reading file occured");
        return FileIN;
    }

    return OK;
}

int getArrayOfStrings(Lines *arrayOfStrings, Text *poemText) {
    catchNullptr(arrayOfStrings);
    catchNullptr(poemText);

    size_t numberOfStrings = 0;

    for (size_t index = 0; poemText -> buffer[index] != '\0'; ++index)
        if (poemText -> buffer[index] == '\n' || poemText -> buffer[index] == ' ')
            ++numberOfStrings;
    linesConstructor(arrayOfStrings, numberOfStrings);

    catchNullptr(arrayOfStrings -> array);

    size_t stringIndex = 0;
    char *firstBufferPointer  = poemText -> buffer;
    char *secondBufferPointer = poemText -> buffer;
    while (*secondBufferPointer != '\0') {
        if (*secondBufferPointer == '\n' || *secondBufferPointer == ' ') {
            lineConstructor(&(arrayOfStrings -> array[stringIndex]), firstBufferPointer, secondBufferPointer - firstBufferPointer);

            catchNullptr(arrayOfStrings -> array[stringIndex].line);

            *secondBufferPointer = '\0';
            ++secondBufferPointer;
            ++stringIndex;
            firstBufferPointer = secondBufferPointer;
        }
        ++secondBufferPointer;
    }

    return OK;
}

int textDestructor(Text *txt) {
    catchNullptr(txt -> file);

    fclose(txt ->   file);
    free  (txt -> buffer);

    txt -> buffer = nullptr;

    return OK;
}

long getFileSize(const char *fileName) {
    catchNullptr(fileName);

    /*fseek(fileName , 0 , SEEK_END);
    long totalLength = ftell(fileName);
    rewind(fileName);*/

    struct stat buf = {};
    if (stat(fileName, &buf)) {
        logPrint("An Error in reading file occured");
        return FileIN;
    }

    return buf.st_size;
}

int print(Lines arrayOfStrings, FILE *outputFile) {
    catchNullptr(outputFile);

    for (size_t currentString = 0; currentString < arrayOfStrings.numberOfLines; ++currentString) {
        fputs(arrayOfStrings.array[currentString].line, outputFile);
        fprintf(outputFile, "\n");
    }
    fputs("---------------------------------------------------\n", outputFile);

    return OK;
}

int printOrigin(char *buffer, size_t numberOfStrings, FILE *outputFile) {
    catchNullptr(outputFile);

    size_t currentString = 0;
    while (currentString < numberOfStrings) {
        if (*buffer == '\0') {
            fputc('\n', outputFile);
            currentString++;
        } else
            fputc(*buffer, outputFile);
        buffer++;
    }

    return OK;
}

void getFileNames(int args, char **argv, char **inFileName, char **outFileName) {
    switch (args) {
        case 3:
            *inFileName  = argv[1];
            *outFileName = argv[2];
            break;
        case 2:
            *inFileName  = argv[1];
            *outFileName = (char *) OUTPUT_FILENAME;
            break;
        default:
            *inFileName  = (char *)  INPUT_FILENAME;
            *outFileName = (char *) OUTPUT_FILENAME;
            break;
    }
}

void logClose() {
    if (logFile == nullptr)
        return;

    fclose(logFile);
}