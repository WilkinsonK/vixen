#include "include/vixen.h"

int main(void) {
    VxnTokens_LoadDefs();
    VxnLexer_AnalyzeText("1 + 2");
    VxnTokens_DumpDefs();
    return 0;
}
