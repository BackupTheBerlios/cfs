%{
#include <stdio.h>
#include <stdlib.h>
#include "cfs.h"
#include "parseutils.h"

static char * curvar = NULL;
static cfile rootnode;
static cfile curnode = NULL;

%}

%x AUTO
%x IFACE
%x MAPPING
%x IFACE_EXTRA_ARGS 

DIGIT [0-9]
NONNEWLINE [^\n\r]
NEWLINE [\n\r]
VARIABLE [a-zA-Z]
WHITESPACE [ \t]
NONWHITESPACE [^ \t]

%%

<INITIAL>{WHITESPACE}*auto{WHITESPACE}+ {
    curnode = create_cnfnode("auto");
    append_node(rootnode,curnode);
    BEGIN(AUTO);
}

<AUTO>{
    {WHITESPACE}*{NONWHITESPACE}+{WHITESPACE}+ {
	    cfile n;
	    TRIM_WHITESPACE
	    n = create_cnfnode(yytext);
	    append_node(curnode, n);
	}
    {NEWLINE}+ { BEGIN(INITIAL); }
    . /* ignore */
}

<INITIAL>{WHITESPACE}*iface{WHITESPACE}+ {
    curnode = create_cnfnode("iface");
    append_node(rootnode, curnode);
    BEGIN(IFACE);
}

<IFACE>{
    {WHITESPACE}*{NONWHITESPACE}+{WHITESPACE}+ {
	    cfile n;
	    TRIM_WHITESPACE
	    n = create_cnfnode(yytext);
	    append_node(curnode, n);
	}
    {NEWLINE}+ { BEGIN(IFACE_EXTRA_ARGS); }
    . /* ignore */
}

<IFACE_EXTRA_ARGS>{
    {WHITESPACE}*{NONWHITESPACE}+{WHITESPACE}+{NONWHITESPACE}+{WHITESPACE}*{NEWLINE}+ {
	cfile n;
	char * remember;
	TRIM_WHITESPACE
	TRIM_NEWLINE
	
	remember = yytext;
	while(!isspace(yytext)) yytext++; // we know there is a space
	*yytext='\0';
	if(strcmp(remember, "auto") && strcmp(remember,"mapping") && strcmp(remember,"")){
	    n = create_cnfnode(remember);
    	    append_node(curnode, n);
		
	    yytext++; TRIM_WHITESPACE
	    cnfnode_setval(n,yytext);
	} else {
	    BEGIN(INITIAL);
	    REJECT;
	}
	
    }
    
    <<EOF>>	yyterminate();
}


{WHITESPACE}+	/* ignore happily */
.		/* throw a dirty error */

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
