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

<INITIAL>"#" 	{
    cfile fd;
    fd = add_entry(".comment");
    curnode = fd;
    
    BEGIN(COMMENT);
}

<INITIAL>{VARIABLE}+	{
    cfile f;
    f = create_cnfnode(yytext);
    append_node(curnode,f);
    curnode = f;
    BEGIN(VARIABLE);
}


<COMMENT>{
    {NONNEWLINE}+ 	{
	    cnfnode_setval(curnode, yytext);
    } 
    {NEWLINE}		{ 
	curnode = curnode->parent;
	BEGIN(INITIAL);
    }
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

<INITIAL>("EndSection"|"EndSubSection"){WHITESPACE}*{NEWLINE} {
    curnode = curnode->parent;
    if(!curnode)
        yyterminate(); /* problem, no current node left */
}

<INITIAL>"Section"{WHITESPACE}+{NONNEWLINE}+{NEWLINE} {
    cfile fd;
    yytext+=7;yyleng-=7;
    TRIM_NEWLINE
    TRIM_WHITESPACE
    fd = cnfnode_create(yytext);
    append_node(curnode,fd);
    curnode=fd;
    add_entry(".whitespace"); // add empty whitespace, trick to force interpreting as directory
}

<INITIAL>"SubSection"{WHITESPACE}+{NONNEWLINE}+{NEWLINE} {
    cfile fd;
    yytext+=10;yyleng-=10;
    TRIM_NEWLINE
    TRIM_WHITESPACE
    fd = cnfnode_create(yytext);
    append_node(curnode,fd);
    curnode=fd;
    add_entry(".whitespace"); // add empty whitespace, trick to force interpreting as directory
}


<INITIAL>{WHITESPACE}+	whitespace(yytext);
<INITIAL>{NEWLINE}+	whitespace(yytext);
<INITIAL>.		        /* throw a dirty error */



%%

struct cnfnode * x_parse(FILE * file){
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
