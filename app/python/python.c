#include <stdio.h>
#include <types.h>

#ifdef __cplusplus
extern "C"
#endif
int main(int, char**);

int app_main()
{
    // Calling python main
    return main(0, NULL);
}
