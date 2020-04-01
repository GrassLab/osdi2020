#ifndef _MATH_H_
#define _MATH_H_

double my_pow (double x, int y)
{
    double temp;
    if (y == 0)
    return 1;
    temp = my_pow (x, y / 2);
    if ((y % 2) == 0) {
        return temp * temp;
    } else {
        if (y > 0)
            return x * temp * temp;
        else
            return (temp * temp) / x;
    }
}


#endif
