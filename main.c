#include <stdio.h>

#include "tokenizer.h"
extern FILE *ngx_conf_fp;


int
main()
{
    ngx_conf_fp = fopen("test.conf", "r");
    token_info_t rv = get_token(ngx_conf_fp);
    while (rv.type != NGX_END) {
        printf("type:%d value:'%s' row:%d col:%d\n", rv.type, rv.value, rv.row, rv.col);
        rv =  get_token(ngx_conf_fp);
    }

    // fclose(ngx_conf_fp);
    return 0;
}
