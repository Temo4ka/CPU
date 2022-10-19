#define _PUSH   stackPush
#define _POP    stackPop
#define  IP   cpu ->  ip
#define CODE  cpu -> code
#define _RAM  cpu -> RAM
#define _REGS cpu -> Regs
#define STACK cpu -> stack
#define  REC  cpu -> calls


DEF_CMD(HALT, 0, 0, {
    IP = cpu -> codeSize;
})

DEF_CMD(PUSH, 1, 1, {
    if ((cmd) & TypeRAM)
        argument = _RAM[argument];
    else if ((cmd) & TypeReg)
        argument = _REGS[argument];
    else
        argument *= PRECISION;
    err |= _PUSH(&(STACK), argument);
})

DEF_CMD(POP , 2, 1, {

    if ((cmd) & TypeRAM) {
        if (argument >= RAM_SIZE) return CPU_OUT_OF_RAM;
        _RAM[argument] = _POP(&(cpu -> stack), &err);
    } else if ((cmd) & TypeReg) {
        if (argument >= REGS_SIZE) return CPU_OUT_OF_REGS;
        _REGS[argument] = _POP(&(cpu -> stack), &err);
    } else
        _POP(&(cpu -> stack), &err);
})

DEF_CMD(ADD , 3, 0, {
    err |= _PUSH(&(STACK), _POP(&(STACK), &err) + _POP(&(STACK), &err));
})

DEF_CMD(OUT , 4, -1, {
    Elem_t value = _POP(&(STACK), &err);
    fprintf(stream, "%d.%02d\n", value / PRECISION, abs(value % PRECISION));
    err |= _PUSH(&(STACK), value);
})

DEF_CMD( IN , 5, 0, {
    scanf("%d", &argument);
    err |= _PUSH(&(STACK), argument * PRECISION);
})

DEF_CMD(DIV , 6, -2, {
    err |= _PUSH(&(STACK), (int) ((double) (_POP(&(STACK), &err) * PRECISION) / (double) _POP(&(STACK), &err)));
})

DEF_CMD(MULT, 7, -2, {
    err |= _PUSH(&(STACK), _POP(&(STACK), &err) * _POP(&(STACK), &err) / PRECISION);
})

DEF_CMD(SQRT, 17, -1, {
    err |= _PUSH(&(STACK), ((int) sqrt(_POP(&(STACK), &err) * PRECISION)));
})

DEF_CMD(SUB, 18, -2, {
    err |= _PUSH(&(STACK), _POP(&(STACK), &err) - _POP(&(STACK), &err));
})

DEF_CMD_JUMP(JA ,  9,  >)

DEF_CMD_JUMP(JAE, 10, >=)

DEF_CMD_JUMP(JB , 11,  <)

DEF_CMD_JUMP(JBE, 12, <=)

DEF_CMD_JUMP(JL , 13, ==)

DEF_CMD_JUMP(JM , 14, !=)

DEF_CMD_JUMP(CALL, 15, ==, {
    IP += sizeof(char);
    err |= _PUSH(&(REC), IP + sizeof(int));
    IP = *((int *) ((char *) CODE + IP));
    break;
})

DEF_CMD(RET, 16, 0, {
    argument = _POP(&(REC), &err);
    IP = argument;
    break;
})

#undef _PUSH
#undef _POP
#undef  IP
#undef CODE
#undef _RAM
#undef _REGS
#undef STACK
#undef  REC