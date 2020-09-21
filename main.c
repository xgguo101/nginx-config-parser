#include "tokenizer.h"
#include <stdio.h>

extern FILE *g_ngxconf_fp;

int
main()
{
    g_ngxconf_fp = fopen("test.conf", "r");
    token_info_t rv = get_token(g_ngxconf_fp);
    while (rv.type != NGX_END) {
        printf("type:%d value:\t'%s' \t\trow:%zu col:%zu\n", 
                rv.type, 
                rv.value, 
                rv.row, 
                rv.col
        );
        rv =  get_token(g_ngxconf_fp);
    }
    fclose(g_ngxconf_fp);
    return 0;
}
