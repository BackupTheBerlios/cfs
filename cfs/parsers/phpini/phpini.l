%{
#include <stdio.h>
#include <stdlib.h>
#include "cfs.h"
#include "../parseutils.h"

static cfile curvar = NULL;
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
VARIABLE [^=\n\r\t;|&~()!"\[]
WHITESPACE [ \t]
NONQUOTE [^\"\t\r\n ][^\"\r\n]*

NOESCAPECHARS [^\"\r\n]
ESCAPE \\. 

%%

";" {
    curvar = add_entry(".comment");
    BEGIN(COMMENT);
}

<COMMENT>{
    {NONNEWLINE}+ 	{
	cnfnode_setval(curvar, yytext);
    } 
    {NEWLINE}		BEGIN(INITIAL);
    <<EOF>>		yyterminate();
}

{VARIABLE}+	{	
    TRIM_WHITESPACE    
    curvar = add_entry(yytext);
    BEGIN(VARIABLE);
}

<VARIABLE>{
    "="{WHITESPACE}*	BEGIN(VARIABLE_IS);
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
    
    "\""[^\r\n"]*"\""{WHITESPACE}*{NEWLINE}+ { 
	cfile fd;
	TRIM_NEWLINE
	TRIM_WHITESPACE 
	cnfnode_setval(curvar,yytext);
	BEGIN(INITIAL);
    }
    
    {NONQUOTE}	{
	cfile fd;
	TRIM_WHITESPACE 
	TRIM_NEWLINE
	cnfnode_setval(curvar,yytext);
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
