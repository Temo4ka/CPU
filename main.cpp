//#include "stack.h"
#include <string>
#include "CPU.h"
#include <windows.h>

#define BINARY_LIST

#include "defaultFilenames.h"

int main(int argc, char *argv[]) {
    system(("chcp " + std :: to_string(CP_UTF8)).c_str());

    const char *fileTexIn =   INPUT_FILE_NAME  ;
    const char *fileBinIn = INPUT_BIN_FILE_NAME;
    const char * fileOut  =   OUTPUT_FILE_NAME ;

    int err = 0;

    switch (argc) {
        case 4:
            fileBinIn = argv[1];
            fileOut   = argv[2];
            fileTexIn = argv[3];
            break;

        case 3:
            fileBinIn = argv[1];
            fileTexIn = argv[2];
            break;

        case 2:
            fileBinIn = argv[1];
            break;
    }

    FILE *outputStream = fopen(fileOut, "w");
    if (outputStream == nullptr) return EXIT_FAILURE;

#ifdef BINARY_LIST
    FILE *binInput = fopen(fileBinIn, "rb");
    if (binInput == nullptr) return EXIT_FAILURE;

    BinFile binList = {};
    err = binFileCtor(&binList, binInput);
    if (err) return EXIT_FAILURE;

    CPU cpu = {};
    err = cpuCtor(&cpu, &binList);
    if (err) return EXIT_FAILURE;

    err = executeBinary(&cpu, outputStream);
    if (err) return EXIT_FAILURE;

    err = binFileDtor(&binList);
    if (err) return EXIT_FAILURE;

    err = cpuDtor(&cpu);
    if (err) return EXIT_FAILURE;
#else
    Text commandTxt = {};
    err = TEXTConstructor(&commandTxt, fileTexIn); // TODO: kto slushaet, a kto net
    if (err) // TODO: can be one-line
             // TODO: dont return not standard value from main.
        return err;

    Lines commandList = {};
    err = getArrayOfStrings(&commandList, &commandTxt);
    if (err)
        return err;

    err = doTexCommands(&commandList, ouputStream); // TODO: tex
    if (err)
        return err;

    err = textDestructor(&commandTxt);
    if (err)
        return err;
#endif

    fclose(outputStream);
    stackLogClose();
    logClose();

    return 0;
}
