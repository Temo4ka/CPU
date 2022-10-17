DEF_CMD(HALT, 0, 0, {
    cpu -> ip = cpu -> codeSize;
})

DEF_CMD(PUSH, 1, 1, {
    if ((cmd) & TypeRAM)
        argument = cpu -> RAM[argument];
    else if ((cmd) & TypeReg)
        argument = cpu -> Regs[argument];
    else
        argument *= PRECISION;
    err |= stackPush(&(cpu -> stack), argument);
})

DEF_CMD(POP , 2, 1, {
//TODO: OutOfArrayErr

    if ((cmd) & TypeRAM) {
        if (argument >= RAM_SIZE) return CPU_OUT_OF_RAM;
        cpu -> RAM[argument] = stackPop(&(cpu -> stack), &err);
    } else if ((cmd) & TypeReg) {
        if (argument >= REGS_SIZE) return CPU_OUT_OF_REGS;
        cpu -> Regs[argument] = stackPop(&(cpu -> stack), &err);
    }else
        stackPop(&(cpu -> stack), &err);
})

DEF_CMD(ADD , 3, 0, {
    err |=stackPush(&(cpu -> stack), stackPop(&(cpu -> stack), &err) + stackPop(&(cpu -> stack), &err));
})

DEF_CMD(OUT , 4, -1, {
    Elem_t value = stackPop(&(cpu -> stack), &err);
    fprintf(stream, "%d.%02d\n", value / PRECISION, abs(value % PRECISION));
    err |= stackPush(&(cpu -> stack), value);
})

DEF_CMD( IN , 5, 0, {
    scanf("%d", &argument);
    err |= stackPush(&(cpu -> stack), argument * PRECISION);
})

DEF_CMD(DIV , 6, -2, {
    err |= stackPush(&(cpu -> stack), (int) ((double) (stackPop(&(cpu -> stack), &err) * PRECISION) / (double) stackPop(&(cpu -> stack), &err)));
})

DEF_CMD(MULT, 7, -2, {
    err |= stackPush(&(cpu -> stack), stackPop(&(cpu -> stack), &err) * stackPop(&(cpu -> stack), &err) / PRECISION);
})

DEF_CMD(SQRT, 17, -1, {
    err |= stackPush(&(cpu -> stack), ((int) sqrt(stackPop(&(cpu -> stack), &err) * PRECISION)));
})

DEF_CMD(SUB, 18, -2, {
    err |=stackPush(&(cpu -> stack), stackPop(&(cpu -> stack), &err) - stackPop(&(cpu -> stack), &err));
})

DEF_CMD_JUMP(JA ,  9,  >)

DEF_CMD_JUMP(JAE, 10, >=)

DEF_CMD_JUMP(JB , 11,  <)

DEF_CMD_JUMP(JBE, 12, <=)

DEF_CMD_JUMP(JL , 13, ==)

DEF_CMD_JUMP(JM , 14, !=)

DEF_CMD_JUMP(CALL, 15, ==)

DEF_CMD(RET, 16, 0, {
    argument = stackPop(&(cpu -> calls), &err);
    cpu -> ip = argument;
    break;
})