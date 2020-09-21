/**
 * 解析 ngx 配置文件
 * 调用 `get_token` 函数返回一个 token 值
 */

#include "tokenizer.h"

#include <ctype.h>  /* ispunct, isalnum */
#include <stdio.h>  /* FILE, fgetc, ftell, fseek */

#define LINE_LIMIT 2048             /* NGX配置文件每行最长限制 */
#define NAME_LIMIT 1024             /* NGX标识符最长限制 */

extern FILE *g_ngxconf_fp;          /* 文件指针 */
static char file_closed = 0;        /* 文件是否关闭 */
static size_t curr_row = 1;         /* 当前行 */
static size_t curr_col = 1;         /* 当前列 */
static size_t curr_row_end_col = 1; /* 当前行最后一列 */


/* 读取一个字符并返回 */
static int
read1()
{
    int next;
    if (file_closed)
        return -1;  /* EOF */

    if ((next = fgetc(g_ngxconf_fp)) == EOF) {
        file_closed = 1;
        fclose(g_ngxconf_fp);
    } else if (next == '\n') {
        ++curr_row;
        curr_row_end_col = curr_col;
        curr_col = 1;
    } 
    else
        ++curr_col;
    return next;
} 


/* 回退一个字符 */
static void
unread1()
{
    if (ftell(g_ngxconf_fp) == 0 || file_closed)
        return;
    
    fseek(g_ngxconf_fp, -1, SEEK_CUR);       /* 文件内部指针回退一步 */
    if (curr_row != 1 && curr_col == 1) {    /* 表示上个字符为 '\n' */
        --curr_row;
        curr_col = curr_row_end_col;
    } 
    else
        --curr_col;
}


/* 读取一行并返回，以 '\0' 结尾 */
static char *
readline(char *line)
{
    int rv;
    char *p = line;
    static size_t maxline = LINE_LIMIT;  /* 每行最长限制 */
    
    while (--maxline > 0 && (rv = read1()) != EOF && rv != '\n') {
        *p++ = rv;
    }
    if (rv == '\n')
        unread1();  /* 回退一步 */
    *p++ = '\0';
    return line;
}


/** 
 * 是否是 ngx 合法标识符
 */
static char
is_ngx_ident(char c)
{
    return (c != ';'
        && c != '{'
        && c != '}'
        && c != '\''
        && c != '"'
        && c != '#'
        && ispunct(c)
    ) || isalnum(c);
}


/* 解析 token */
static token_info_t
parse()
{
    static char line_buf[LINE_LIMIT];
    static char name_buf[NAME_LIMIT];
    token_info_t r;

    int g_next = read1();

    /* 丢弃空白 */
    if (isspace(g_next)) {
        int next;
        do {
            next = read1();
        } while (isspace(next));
        unread1();  /* 回退一步 */
    }

    /* 丢弃注释 */
    if (g_next == '#') {
        char *p = line_buf;
        readline(p);
    }
    
    /* 解析标识符名称 */
    if (is_ngx_ident(g_next) && g_next != EOF) {
        char *p = name_buf;
        *p++= g_next;

        int next;
        while (is_ngx_ident(next = read1()) && next != EOF) {
            *p++ = next;
        }
        *p++ = '\0';
        unread1();  /* 回退一步 */

        r.type = NGX_NAME;
        r.value = name_buf;
        goto end;
    }

    /* 解析字符串 */
    if (g_next == '\'' || g_next == '"') {
        char *p = name_buf;
        int next;
        while ((next = read1()) != g_next && next != '\n') {
            *p++ = next;
        }
        *p++ = '\0';
        r.type = NGX_NAME;
        r.value = name_buf;
        goto end;
    }

    switch (g_next) {
    case '{':
        r.type = NGX_START_BLOCK;
        r.value = "{";
        break;
    case '}':
        r.type = NGX_END_BLOCK;
        r.value = "}"; 
        break;
    case ';':
        r.type = NGX_OK;
        r.value = ";";
        break;
    case EOF:
        r.type = NGX_END;
        r.value = NULL;
        break;
    default:
        /* 递归 */
        return parse();
    }
end:
    r.col = curr_col;
    r.row = curr_row;
    return r;
}


/* 返回一个 token */
token_info_t
get_token(FILE *fp)
{
    int next;
    token_info_t r;

    if ((next = read1()) != -1) {
        unread1();
        r = parse();
    }
    else
        r.type = NGX_UNKNOW;

    return r;
}
