#include <stdlib.h>
#include <time.h>

extern void graphics_init(void);
extern void graphics_run(void);

int main(void)
{
    srand((unsigned int)time(NULL));
    graphics_init();
    graphics_run();
    return 0;
}
