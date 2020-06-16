int main(int argc, char **argv)
{
    int i = 0xff;
    char buf[64];

    snprintf(buf, sizeof(buf), "%100c%-2$n");
    printf("buf %s\n", buf);
    if (i == 65) {
        printf("GOOD\n");
    }
}