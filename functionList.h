#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

enum Errors {
    OK            =  0,
    FileIN        = -1,
    FileOUT       = -2,
    BUFFER        = -3,
    DYNAMICMEMORY = -4,
    NULLCAUGTH    = -5
};

enum Compare {
    Less   = -1,
    Equal  =  0,
    Bigger =  1
};

struct Error {
    char const *errorMessage = nullptr;
    char const *functionName = nullptr;
    char const *fileName     = nullptr;
    unsigned        line     =       0;
};

struct Line {
    char  *line   = nullptr;
    size_t length = 0;
};

struct Lines {
    Line  *array         = nullptr;
    size_t numberOfLines =    0   ;
};

struct Text {
    const char *fileName = nullptr;
    FILE       *file     = nullptr;
    char       *buffer   = nullptr;
    long        fileSize =    0   ;
};

void linesDestructor(Lines *strings);

int textDestructor(Text *txt);

///--------------------------------------------
//! \brief This procedure prints string
//! \param [in] string
//!
///--------------------------------------------
void myPuts(const char *string);

///--------------------------------------------------------------------------
//! \brief This function returns the first occurrence of a symbol in a string
//! \param [in] string, symbol
//! \returns pointer to the symbol
//!
///--------------------------------------------------------------------------
const char *myStrchr(const char *string, int symbol);

///--------------------------------------------------
//! \brief This function gives the length of a string
//! \param [in] string
//! \returns size of a string
//!
///--------------------------------------------------
size_t myStrlen(const char *string);

///--------------------------------------------------
//! \brief This function copies strings
//! \param [in] string
//! \returns copy of a string
//!
///--------------------------------------------------
char *myStrcpy(char *resultOfCopy, const char *origin);

///------------------------------------------------------
//! \brief This function copies first symbols of a string
//! \param [in] string
//! \returns size of a string
//!
///------------------------------------------------------
char *myStrncpy(char *resultOfCopy, const char *origin, size_t num);

///--------------------------------------------------
//! \brief This function concats two strings
//! \param [in] destptr, srcptr
//! \returns concatenated strings
//!
///--------------------------------------------------
char *myStrcat(char *destptr, const char *srcptr);

///----------------------------------------------------------------------------
//! \brief This function concats some first symbols of one string with another
//! \param [in] destptr, srcptr
//! \returns concatenated strings
//!
///----------------------------------------------------------------------------
char *myStrncat(char *destptr, const char *srcptr, size_t num);

///----------------------------------------------------------------------------
//! \brief This function concats some first symbols of one string with another
//! \param [in] destptr, srcptr
//! \returns concatenated strings
//!
///----------------------------------------------------------------------------
char *myStrstr(char *str, const char *substr);

///----------------------------------------------------------------------------
//! \brief This function separates string in case of finding special symbols
//! \param [in] string, delimiter
//! \returns a piece of a string
//!
///----------------------------------------------------------------------------
char *myStrtok(char *string, const char *delimiter);

///----------------------------------------------------------------------------
//! \brief This procedures gets an array of strings from opened file
//! \param [in] arrayOfStrings
//!
///----------------------------------------------------------------------------
int getArrayOfStrings(Lines *arrayOfStrings, Text *poemText);

///----------------------------------------------------------------------------
//! \brief This procedure sorts an array of strings
//! \param [in] arrayOfStrings, left, right
//!
///----------------------------------------------------------------------------
void mergeSort(Lines *arrayOfStrings, size_t left, size_t right, int (*cmp)(const void *aParam, const void *bParam));

///----------------------------------------------------------------------------
//! \brief This procedure sorts an array of strings
//! \param [in] arrayOfStrings, left, right
//!
///----------------------------------------------------------------------------
void quickSort(void *arrayOfStrings, size_t sizeOfElement, size_t left, size_t right, int (*cmp)(const void *aParam, const void *bParam));

///----------------------------------------------------------------------------
//! \brief It's a comparator for mergesort
//!
///----------------------------------------------------------------------------
int comparatorForMergeSort(const void *aParam, const void *bParam);

///----------------------------------------------------------------------------
//! \brief It's a comparator for quick sort
//!
///----------------------------------------------------------------------------
int comparatorForQsort(const void *aParam, const void *bParam);

///----------------------------------------------------------------------------
//! \brief It's a comparator for quick sort
//!
///----------------------------------------------------------------------------
int revComparator(const void *aParam, const void *bParam);

///----------------------------------------------------------------------------
//! \brief This function prints array of strings
//! \param [in] arrayOfStrings, outputFile
//!
///----------------------------------------------------------------------------
int print(Lines arrayOfStrings, FILE *outputFile);

///----------------------------------------------------------------------------
//! \brief This procedure swaps lines
//!
///----------------------------------------------------------------------------
void swap(void *a, void *b, size_t sizeOfElement);

///----------------------------------------------------------------------------
//! \brief This function prints original text
//! \param [in] buffer, numberOfStrings, outputFile
//!
///----------------------------------------------------------------------------
int printOrigin(char *buffer, size_t numberOfStrings, FILE *outputFile);

///----------------------------------------------------------------------------
//! \brief This procedure seeks for errors
//! \param [in] error
//!
///----------------------------------------------------------------------------
int checkForErrors(int error);

///----------------------------------------------------------------------------
//! \brief This function constructs Text structure
//! \returns code of an error
//!
///----------------------------------------------------------------------------
int TEXTConstructor(Text *txt, const char *fileName);

///----------------------------------------------------------------------------
//! \brief This procedure constructs Lines structure
//!
///----------------------------------------------------------------------------
void linesConstructor(Lines *strings, size_t num);

///----------------------------------------------------------------------------
//! \brief This procedure constructs Line structure
//!
///----------------------------------------------------------------------------
void lineConstructor(Line *line, char *string, size_t numberOfElements);

///----------------------------------------------------------------------------
//! \brief This function gets size of file
//! \returns size of file
//!
///----------------------------------------------------------------------------
long getFileSize(const char *poemFile);

///----------------------------------------------------------------------------
//! \brief This function is comparator for *
//! \param [in] aParam, bParam
//!
///----------------------------------------------------------------------------
int originComparator(const void *aParam, const void *bParam);

void getFileNames(int args, char **argv, char **inFileName, char **outFileName);

void logClose();