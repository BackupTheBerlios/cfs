%{
#include <stdio.h>
#include <stdlib.h>
#include "cfs.h"
#include "parseutils.h"

static char * curvar = NULL;
static cfile rootnode;
static cfile section = NULL;

static struct cnfnode * add_entry(char * name){
    struct cnfnode * newnode;
    newnode = create_cnfnode(name);
    if(section)
	append_node(section, newnode);
    else 
	append_node(rootnode, newnode);
    return newnode;
}

static void whitespaceadd(char * text){
    cfile fd;
    fd = add_entry(".whitespace");
    cnfnode_setval(fd,text);    
}

#define SETVAR(s) \
    if(curvar) \
        free(curvar); \
    curvar = strdup(s); 

%}

%x COMMENT
%x VARIABLE
%x VARIABLE_IS


DIGIT [0-9]
NONNEWLINE [^\n\r]
NEWLINE [\n\r]
VARIABLE [a-zA-Z0-9_-]
WHITESPACE [ \t]
NONQUOTE [^\"\t\r\n ][^\"\r\n]*

NOESCAPECHARS [^\"\r\n]
ESCAPE \\. 

%%

";" 			BEGIN(COMMENT);

<COMMENT>{
    {NONNEWLINE}+ 	{
	cfile fd;
	fd = add_entry(".comment");
	cnfnode_setval(fd, yytext);
    } 
    {NEWLINE}		BEGIN(INITIAL);
    <<EOF>>		yyterminate();
}

{VARIABLE}+	SETVAR(yytext); BEGIN(VARIABLE);

<VARIABLE>{
    "="			BEGIN(VARIABLE_IS);
    {WHITESPACE} 	/* ignore, can't respresent this */
    {NEWLINE}		BEGIN(INITIAL);
    .			/* throw a dirty error */
}	

<INITIAL>[[][^[]+[\]]{WHITESPACE}*{NEWLINE}	{
    TRIM_WHITESPACE
    TRIM_NEWLINE
    yytext++; yyleng--;
    yytext[yyleng-1] = '\0';
    section = create_cnfnode(yytext);
    append_node(rootnode,section);
}

<VARIABLE_IS>{
    {WHITESPACE}+ /*ignore*/
    
    "\""[^\r\n"]*"\""{NEWLINE}+ { 
	struct cnfnode * fd;
	TRIM_NEWLINE
	TRIM_WHITESPACE 

	yytext++; yyleng--; 
	yytext[yyleng-1] = '\0'; 
	fd = add_entry(curvar);
	cnfnode_setval(fd,yytext);
	BEGIN(INITIAL);
    }
    
    {NONQUOTE}	{
	struct cnfnode * fd;
	TRIM_WHITESPACE 
	TRIM_NEWLINE
	fd = add_entry(curvar);
	cnfnode_setval(fd,yytext);
    }
    {NEWLINE}	BEGIN(INITIAL);
}

<INITIAL>{WHITESPACE}+	whitespaceadd(yytext);
<INITIAL>{NEWLINE}+	whitespaceadd(yytext);
.		yyterminate();

%%

struct cnfnode * phpini_parse(FILE * file){
    yyin = file;
    rootnode = create_cnfnode("(root)");
    if(yylex())
	return NULL;
    else 
	return rootnode;
}

int yywrap(){
    return 1;
}
