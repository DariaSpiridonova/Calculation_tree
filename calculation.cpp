#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

const char *s = "10*(30+20*10)+13 ";

int GetG();
int GetE();
int GetT();
int GetP();
int GetN();

void SyntaxError();

int main()
{
    printf("%d\n", GetG());

    return 0;
}

int GetG()
{
    int val = GetE();
    if (*s != '$') 
        SyntaxError();
    s++;
    return val;
}

int GetE()
{
    int val = GetT();
    while (*s == '+' || *s == '-')
    {
        int op = *s;
        s++;
        int val2 = GetT();
        if (op = '+')
            val += val2;
        else    
            val -= val2;
    }

    return val;
}

int GetT()
{
    int val = GetP();
    while (*s == '*' || *s == '/')
    {
        int op = *s;
        s++;
        int val2 = GetP();
        if (op = '*')
            val *= val2;
        else    
            val /= val2;
    }

    return val;
}

int GetP()
{
    if (*s == '(')
    {
        s++;
        int val = GetE();
        if (*s != ')') SyntaxError();
        else s++;
        return val;
    }

    else 
        return GetN();
}

int GetN()
{
    int val = 0;
    bool was_shift = false;
    while ('0' <= *s && *s <= '9')
    {
        val = val * 10 + (int)(*s - '0');
        s++;
        was_shift = true;
    }

    if (!was_shift) SyntaxError();
    return val;
}

void SyntaxError()
{
    printf("Syntax Error\n");
    exit(1);
}