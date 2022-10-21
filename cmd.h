#define _PUSH(value)   stackPush(&(STACK), (value))
#define _POP            stackPop (&(STACK), &err)
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
        argument =  _RAM[argument];
    else if ((cmd) & TypeReg)
        argument = _REGS[argument];
    else
        argument *= PRECISION;
    err |= _PUSH(argument);
})

DEF_CMD(POP , 2, 1, {

    if (((cmd) & TypeRAM) && ((cmd) & TypeReg)) {
        if (argument >= REGS_SIZE || argument < 0) break;
        if (_REGS[argument] / PRECISION >= RAM_SIZE || _REGS[argument] / PRECISION < 0) break;

        _RAM[_REGS[argument] / PRECISION] = _POP;
    } else if ((cmd) & TypeRAM) {
        if (argument >= RAM_SIZE) return CPU_OUT_OF_RAM;
         _RAM[argument] = _POP;
    } else if ((cmd) & TypeReg) {
        if (argument >= REGS_SIZE) return CPU_OUT_OF_REGS;
        _REGS[argument] = _POP;
    } else
        _POP;
})

DEF_CMD(ADD , 3, 0, {
    err |= _PUSH(_POP + _POP);
})

DEF_CMD(OUT , 4, -1, {
    Elem_t value = _POP;
    fprintf(stream, "%d.%02d\n", value / PRECISION, abs(value % PRECISION));
    err |= _PUSH(value);
})

DEF_CMD( IN , 5, 0, {
    scanf("%d", &argument);
    err |= _PUSH(argument * PRECISION);
})

DEF_CMD(DIV , 6, -2, {
    err |= _PUSH((int) ((double) (_POP * PRECISION) / (double) _POP));
})

DEF_CMD(MULT, 7, -2, {
    err |= _PUSH((double) _POP / (double) PRECISION * _POP);
})

DEF_CMD(SQRT, 17, -1, {
    err |= _PUSH(((int) sqrt(_POP * PRECISION)));
})

DEF_CMD(SUB, 18, -2, {
    err |= _PUSH(_POP - _POP);
})

DEF_CMD(SIN, 19, -1, {
    double val = _POP;
    val = (double) val / (double) (PRECISION * PRECISION);
//    fprintf(stderr, "%d\n", (int) (sin(val) * (double) PRECISION));
    err |= _PUSH((int) (sin(val) * (double) PRECISION));
})

DEF_CMD(COS, 20, -1, {
    double val = _POP;
    val = (double) val / (double) (PRECISION * PRECISION);
//    fprintf(stderr, "%d\n", (int) (cos(val) * (double) PRECISION));
    err |= _PUSH((int) (cos(val) * (double) PRECISION));
})

DEF_CMD(INT, 21, -1, {
    err |= _PUSH(_POP / PRECISION * PRECISION);
})

DEF_CMD_JUMP(JA ,  9,  >)

DEF_CMD_JUMP(JAE, 10, >=)

DEF_CMD_JUMP(JB , 11,  <)

DEF_CMD_JUMP(JBE, 12, <=)

DEF_CMD_JUMP(JL , 13, ==)

DEF_CMD_JUMP(JM , 14, !=)

DEF_CMD_JUMP(CALL, 15, ==, {
    IP += sizeof(char);
    err |= stackPush(&(REC), IP + sizeof(int));
    IP = *((int *) ((char *) CODE + IP));
    break;
})

DEF_CMD(RET, 16, 0, {
    argument = stackPop(&(REC), &err);
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