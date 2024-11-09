#include "stdlib.h"

struct Person
{
    char *cdate_of_birth;
    char *cname;
};

uint8_t cmain()
{
    struct Person *cperson = malloc(sizeof(struct Person));

    cperson->cdate_of_birth = "123";
    cperson->cname = "Chal";
    int32_t cfoo = 10;
    int32_t cbar = 10;
    return cfoo + cbar;
}

int main()
{
    cmain();
}
