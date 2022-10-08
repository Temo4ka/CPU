//#include "stack.h"
#include "CPU.h"

#define BINARY_LIST

const char *INPUT_FILE_NAME     =  "commandnumb.txt";
const char *INPUT_BIN_FILE_NAME =  "bincommand.bin" ;
const char *OUTPUT_FILE_NAME    = "resultOfCalc.txt";

int main(int argc, char *argv[]) {
    char *fileTexIn = nullptr;
    char *fileBinIn = nullptr;
    char * fileOut  = nullptr;

    int err = 0;

    switch (argc) {
        case 4: fileTexIn = argv[3];
                fileBinIn = argv[2];
                fileOut   = argv[1];
                break;

        case 3: fileTexIn = argv[2];
                fileBinIn = argv[1];
                fileOut   = (char *) OUTPUT_FILE_NAME;
                break;

        case 2: fileTexIn =           argv[1]           ;
                fileBinIn = (char *) INPUT_BIN_FILE_NAME;
                fileOut   = (char *) OUTPUT_FILE_NAME   ;
                break;

        default: fileTexIn = (char *)  INPUT_FILE_NAME;
                 fileBinIn = (char *) INPUT_BIN_FILE_NAME;
                 fileOut   = (char *) OUTPUT_FILE_NAME;
                 break;
    }

    FILE *ouputStream = fopen(fileOut, "w");

#ifdef BINARY_LIST
    FILE   *binInput  = fopen(fileBinIn, "rb");

    BinFile binList = {};
    err = BinFileCtor(&binList, binInput);
    if (err)
        return err;

    CPU cpu = {};
    err = cpuCtor(&cpu, &binList);
    if (err)
        return err;

    err = doBinCommands(&cpu, ouputStream);
    if (err)
        return err;

    err = BinFileDtor(&binList);
    if (err)
        return err;
    err = cpuDtor(&cpu);
    if (err)
        return err;
#else
    Text commandTxt = {};
    err = TEXTConstructor(&commandTxt, fileTexIn);
    if (err)
        return err;

    Lines commandList = {};
    err = getArrayOfStrings(&commandList, &commandTxt);
    if (err)
        return err;

    err = doTexCommands(&commandList, ouputStream);
    if (err)
        return err;

    err = textDestructor(&commandTxt);
    if (err)
        return err;
#endif

    fclose(ouputStream);
    logClose();
    return 0;
}
