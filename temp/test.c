#include<stdio.h>
#include<string.h>

int main()
{
    char * l = malloc(sizeof(char));
    strcpy(l,"mytest");
    puts(l);
}