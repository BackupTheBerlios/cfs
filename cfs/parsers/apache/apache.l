%{
#include <stdio.h>
#include <stdlib.h>
#include "cfs.h"
#include "parseutils.h"

static cfile rootnode;
static cfile curnode = NULL;

#define SETVAR(s) \
    if(curvar) \
        free(curvar); \
    curvar = strdup(s); 

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

<INITIAL>{WHITESPACE}+	/* ignore happily */
<INITIAL>.		        /* throw a dirty error */

<COMMENT>{
    {NONNEWLINE}+ 	{
	    cfile fd;
	    fd = add_entry(".comment");
	    cnfnode_setval(fd, yytext);
    } 
    {NEWLINE}+		BEGIN(INITIAL);
    <<EOF>>		yyterminate();
}

<VARIABLE>{
    {NONNEWLINE}+ {
        TRIM_NEWLINE
        TRIM_WHITESPACE
        cnfnode_appendval(curnode,yytext);
    }
    "\"{NEWLINE}    /*ignore*/
    {WHITESPACE}    /*ignore*/
    {NEWLINE}+	{
        curnode = curnode->parent;    
        BEGIN(INITIAL);
    }
    .			/* throw a dirty error */
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
    .    /* ignore */
}


%%

struct cnfnode * phpini_parse(FILE * file){
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
