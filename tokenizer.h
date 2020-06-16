#ifndef _TOKENIZER_H__
#define _TOKENIZER_H__

#include <stdio.h>  /* FILE */
#include <stddef.h> /* size_t */

/* 定义 token */
typedef enum {
    NGX_UNKNOW = -1,
    NGX_NAME,           /* 连续字符串 */
    NGX_OK,             /* ";" */
    NGX_START_BLOCK,    /* "{" */
    NGX_END_BLOCK,      /* "}" */
    NGX_END,            /* "EOF" */
} token_t;

/* token 上下文信息 */
typedef struct token_info {
    token_t type;   /* token */
    char *value;    /* token 值 */
    size_t row;     /* token 所在行 */
    size_t col;     /* token 所在列 */
} token_info_t;

/* 文件 FILE 指针 */
extern FILE *ngx_conf_fp;

/* 返回一个 token 值 */
extern token_info_t get_token(FILE *fp);

#endif  /* TOKENIZER_H__ */