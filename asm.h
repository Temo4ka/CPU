#define catchNullptr(a) { \
    if ((a) == nullptr) {         \
        fprintf(stderr, "Nullptr caught.\nVariable: %s,\nFile: %s,\nLine: %d,\nFunction: %s\n", #a, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
    return NULLCAUGTH;         \
    }\
}

enum StackCommands {
      HALT  = 0,
      PUSH  = 1,
      POP   = 2,
      ADD   = 3,
      OUT   = 4,
      IN    = 5,
      DIV   = 6,
    PRODUCT = 7,
};