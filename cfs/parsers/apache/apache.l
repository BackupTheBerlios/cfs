%{
#include <stdio.h>
#include <stdlib.h>
#include "cfs.h"
#include "../parseutils.h"

static cfile rootnode;
static cfile curnode;

static struct cnfnode * add_entry(char * name){
    struct cnfnode * newnode;
    newnode = create_cnfnode(name);
    append_node(curnode, newnode);
    return newnode;
}


#define SETVAR(s) \
    if(curvar) \
        free(curvar); \
    curvar = strdup(s); 
    
static void whitespace(char * data){
    cfile fd;
    fd = create_cnfnode(".whitespace");
    cnfnode_setval(fd,data);
    append_node(curnode,fd);
}

%}

%x COMMENT
%x VARIABLE
%x SECTION

DIGIT [0-9]
NONNEWLINE [^\n\r]
NEWLINE [\n\r]
VARIABLE [a-zA-Z0-9_-]
WHITESPACE [ \t]
NONQUOTE [^\"\t\r\n ][^\"\r\n]*

NOESCAPECHARS [^\"\r\n]
ESCAPE \\. 

%%

<INITIAL>"#" 			BEGIN(COMMENT);

<INITIAL>{VARIABLE}+	{
    cfile f;
    f = create_cnfnode(yytext);
    append_node(curnode,f);
    curnode = f;
    BEGIN(VARIABLE);
}


<COMMENT>{
    {NONNEWLINE}+ 	{
	    cfile fd;
	    fd = add_entry(".comment");
	    cnfnode_setval(fd, yytext);
    } 
    {NEWLINE}		BEGIN(INITIAL);
    <<EOF>>		yyterminate();
}

<VARIABLE>{
    {NONNEWLINE}+ {
        TRIM_NEWLINE
        TRIM_WHITESPACE
        cnfnode_appendval(curnode,yytext);
    }
    "\\"{NEWLINE}    /*ignore*/
    {WHITESPACE}    /*ignore*/
    {NEWLINE}	{
        curnode = curnode->parent;    
        BEGIN(INITIAL);
    }
}	

<INITIAL>"</"{VARIABLE}+">" {
    curnode = curnode->parent;
    if(!curnode)
        yyterminate(); /* problem, no current node left */
}

<INITIAL>"<"{WHITESPACE}*{VARIABLE}+ {
    cfile f;
    yytext++;
    TRIM_WHITESPACE
    f = create_cnfnode(yytext);
    append_node(curnode,f);
    curnode = f;
    BEGIN(SECTION);
}

<SECTION>{
    ">" BEGIN(INITIAL);
    [^>]+	cnfnode_appendval(curnode,yytext);
}

<INITIAL>{WHITESPACE}+	whitespace(yytext);
<INITIAL>{NEWLINE}+	whitespace(yytext);
<INITIAL>.		        /* throw a dirty error */



%%

struct cnfnode * apache_parse(FILE * file){
    yyin = file;
    rootnode = create_cnfnode("(root)");
    curnode = rootnode;
    if(yylex())
	return NULL;
    else 
	return rootnode;
}

int yywrap(){
    return 1;
}
