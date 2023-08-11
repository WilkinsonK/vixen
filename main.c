#include "include/vixen.h"

int main(void) {
    vxn_token_load_defs();
    printf("\n\n");
    vxn_token_dump_defs();
    return 0;
}
