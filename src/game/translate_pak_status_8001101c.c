int func_8001101C(int status)
{
    if (status == 0) {
        return 0;
    }
    if (status == 4) {
        return 1;
    }
    if (status < 5) {
        if (status == 1) {
            return 4;
        }
        return status + 100;
    }
    if (status == 5) {
        return 3;
    }
    if (status == 11) {
        return 5;
    }
    return status + 100;
}
