unsigned int func_800046A8(const char *string) {
    const char *end;

    end = string;
    while (*end++ != '\0') {}
    return end - string;
}
