int func_8001101C(int status)
{
    int result;

    result = 0;
    if (status != 0) {
        if (status == 4) {
            goto result_four;
        }
        if (status < 5) {
            if (status == 1) {
                goto result_three;
            }
            result = status + 100;
            goto done;
        }
        if (status == 5) {
            goto result_one;
        }
        if (status == 11) {
            goto result_five;
        }
        result = status + 100;
        goto done;
    }
    goto done;

result_one:
    result = 1;
    goto done;
result_three:
    result = 3;
    goto done;
result_four:
    result = 4;
    goto done;
result_five:
    result = 5;
done:
    return result;
}
