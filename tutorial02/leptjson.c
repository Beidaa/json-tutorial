#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod() */
#include <string.h>

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int lept_parse_literal(lept_context* c, lept_value* v ,const char* ch ,lept_type type){
    EXPECT(c,ch[0]);
    int i;
    for (i=0;ch[i+1];i++){
        if (c->json[i]!=ch[i+1]) return LEPT_PARSE_INVALID_VALUE;
    }
    c->json+=i;
    v->type=type;
    return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context* c, lept_value* v) {
    char* end;int len=strlen(c->json);
    /* \TODO validate number */
    if (c->json[0]=='+'||c->json[0]== '.'||c->json[0]== 'I'||c->json[0]== 'i'||c->json[0]== 'n'||c->json[0]== 'N'||c->json[len-1]=='.') return LEPT_PARSE_INVALID_VALUE;
    else if (c->json[0]=='0'&&(c->json[1]!='\0'&&c->json[1]!='.'&&c->json[1]!='e'&&c->json[1]!='E')) return LEPT_PARSE_ROOT_NOT_SINGULAR;
    else {
        v->n = strtod(c->json, &end);
        if (c->json == end)
            return LEPT_PARSE_INVALID_VALUE;
        c->json = end;
        v->type = LEPT_NUMBER;
        return LEPT_PARSE_OK;
    }

}

static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 't' :return lept_parse_literal(c, v, "true", LEPT_TRUE);
        case 'f' :return lept_parse_literal(c, v, "false", LEPT_FALSE);
        case 'n' :return lept_parse_literal(c, v, "null", LEPT_NULL);
        default :  return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}
