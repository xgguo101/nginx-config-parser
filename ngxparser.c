#include <stdio.h>
#include <stdlib.h>

static FILE *fp;                    /* 文件 FILE 指针 */
static char file_closed = 0;        /* 文件是否关闭 */
static size_t curr_row = 1;         /* 当前行 */
static size_t curr_col = 1;         /* 当前列 */
static size_t curr_row_end_col = 1; /* 当前行最后一列 */


/* 字符串数据 */
typedef struct {
    char *data;
} StringBuffer;


/* 定义 token */
typedef enum {
    // NGX_UNKNOW = -1,
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


// /* 获取文件大小 */
// static long
// get_filesize(FILE *fp)
// {
//     fseek(fp, 0, SEEK_END);
//     long size = ftell(fp);
//     fseek(fp, 0, SEEK_SET);
//     return size;
// }


/* 读取一个字符并返回 */
static int
read1()
{
    int next;
    if (file_closed)
        return -1;  /* EOF */

    if ((next = fgetc(fp)) == EOF) {
        file_closed = 1;
        fclose(fp);
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
    if (ftell(fp) == 0 || file_closed)
        return;
    
    fseek(fp, -1, SEEK_CUR);                /* 文件内部指针回退一步 */
    if (curr_row != 1 && curr_col == 1) {   /* 表示上个字符为 '\n' */
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
    static size_t maxline = 1024;  /* 每行最长限制 */
    
    while (--maxline > 0 && (rv = read1()) != EOF && rv != '\n') {
        *p++ = rv;
    }
    if (rv == '\n' || EOF)
        unread1();  /* 回退一步 */
    *p++ = '\0';
    return line;
}


/** 是否是 ngx 合法字符串 
 *  todo: 严格标准
 */
static char
is_ngx_ident(char c)
{
   return isalnum(c)    \
          || c == '$'   \
          || c == '_'   \
          || c == '.'   \
          || c == '\\'  \
          || c == '/'   \
          || c == '"'   \
          || c == '\''  \
          || c == '~'   \
          || c == '-';
}


/* 获取一个 token 值并返回 */
token_info_t
get_token(StringBuffer *name_buf, StringBuffer *line_buf)
{
    token_info_t r;
    int next = read1(); 
    
    /* 解析标识符名称 */
    if (is_ngx_ident(next) && next != EOF) {
         char *p = name_buf->data;
        *p++= next;

        int next;
        while (is_ngx_ident(next = read1()) && next != EOF) {
            *p++ = next;
        }
        *p++ = '\0';
        unread1();  /* 回退一步 */

        r.type = NGX_NAME;
        r.value = name_buf->data;
        r.col = curr_col;
        r.row = curr_row; 
        return r; 
    }

    /* 解析注释，直接丢弃不保存 */
    if (next == '#') {
        char *p = line_buf->data;
        readline(p);
    }
    
    /* 丢弃空白 */
    if (isspace(next)) {
        int next;
        do {
             next = read1();
        } while (isspace(next));
        unread1();  /* 回退一步 */
    }

    switch (next) {
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
        return get_token(name_buf, line_buf);
    }
    r.col = curr_col;
    r.row = curr_row;
    return r;
}


int
main()
{
    fp = fopen("test.conf", "r");

    StringBuffer name, line;
    StringBuffer *name_p = &name;
    StringBuffer *line_p = &line;
    name_p->data = (char *) malloc(1024);
    line_p->data = (char *) malloc(1024);
    token_info_t rv;

    int c;
    while ((c = read1()) != -1) {
        unread1();  /* 回退一步 */
        rv = get_token(name_p, line_p);
        printf("type:%d value:'%s' row:%d col:%d\n", rv.type, rv.value, rv.row, rv.col);
    }

    if (!file_closed)
       fclose(fp);

    return 0;
}
