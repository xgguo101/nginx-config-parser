#ifndef _TOKENIZER_H__
#define _TOKENIZER_H__

#include <stdio.h>  /* FILE */
#include <stddef.h> /* size_t */

/* 定义 token */
typedef enum {
    NGX_UNKNOW = -1,
    NGX_NAME,           /* 标识符 */
    NGX_OK,             /* ";" */
    NGX_START_BLOCK,    /* "{" */
    NGX_END_BLOCK,      /* "}" */
    NGX_END = 32,       /* "EOF" */
} token_t;

/* token 上下文信息 */
typedef struct {
    token_t type;   /* token 类型 */
    char *value;    /* token 值 */
    size_t row;     /* token 所在行 */
    size_t col;     /* token 所在列 */
} token_info_t;

/* 文件 FILE 指针 */
FILE *g_ngxconf_fp;

/* 返回一个 token 值 */
token_info_t get_token(FILE *fp);

#endif  /* TOKENIZER_H__ */
