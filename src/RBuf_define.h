struct RBuf_struct{
    char buf[MAX_ARRAYSIZE][MAX_TEXT];   /* buf */
    buftype type[MAX_ARRAYSIZE];
    char thispointer_str[MAX_TEXT];
    char thisclass_str[MAX_TEXT];
    boolean isVerbose;
    int first;
    int last;
};
