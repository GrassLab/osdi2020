int abs(int n) {
    return (n < 0) ? - n : n;
}

float fabs(float n) {
    float abs = n;
    //VFP disabled? set FPEXC32_EL2[30]
    //asm volatile("VABS %0 %1" : "=r" (abs) : "r" (n));
    return abs;
}

int ceil(float n) {
    int a = n;
    return ((float)a != n) ? n + 1 : n;
}

int floor(float n) {
    int a = n;
    return ((float)a != n) ? n - 1 : n;
}

float pow(int n, int p) {
    if (!p) return 1;
    else if (p < 0) return pow(n, p + 1) / n;
    else return n * pow(n, p - 1);
}

float sqrt(float x) {
    float xhalf = 0.5f*x;
    union {
        float x;
        int i;
    } u;
    u.x = x;
    u.i = 0x5f375a86 - (u.i >> 1);
    /* The next line can be repeated any number of times to increase accuracy */
    u.x = u.x * (1.5f - xhalf * u.x * u.x);
    return u.x;
}
