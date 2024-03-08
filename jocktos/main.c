#include "main.h"
// [WIP]
void JocktosMain(void)
{
    int x = 0;
    int y = 100;
    
    while(1)
    {
        x++;
        if (x >= 100) x = 0;
         y--;
        if (y <= 0) y = 100;
    };
}
