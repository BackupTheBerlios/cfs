#define TRIM_WHITESPACE \
    while(*yytext==' ' || *yytext=='\t'){ \
	yytext++; yyleng--; \
    } \
    while(*(yytext+yyleng-1)==' ' || *(yytext+yyleng-1)=='\t'){ \
	yytext[yyleng-1] = '\0'; \
	yyleng--; \
    }

#define TRIM_NEWLINE \
    while(*(yytext+yyleng-1)=='\n' || *(yytext+yyleng-1)=='\r'){ \
	yytext[yyleng-1] = '\0'; \
	yyleng--; \
    }

static inline writeline(FILE *f, char *line){
    char * tmp;
    tmp = (char *) malloc(strlen(line)+2);
    strcpy(tmp, line);
    strcat(tmp, "\n");
    fwrite(tmp,strlen(line)+1,1,f);
}
