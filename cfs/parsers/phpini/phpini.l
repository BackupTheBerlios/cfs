/*
    This file is part of cfs

    Copyright (C) 2004  Jens Hermans <paranoid@pcwereld.be>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


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
