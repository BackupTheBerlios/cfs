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

<INITIAL>"</"{VARIABLE}+">"{WHITESPACE}*{NEWLINE} {
    curnode = curnode->parent;
    if(!curnode)
        yyterminate(); /* problem, no current node left */
}

<INITIAL>"<"{WHITESPACE}*{VARIABLE}+{WHITESPACE}* {
    cfile f;
    yytext++;yyleng--;
    TRIM_WHITESPACE
    f = cnfnode_create(yytext);
    append_node(curnode,f);
    curnode = f;
    add_entry(".whitespace"); // add empty whitespace, trick to force interpreting as directory
    BEGIN(SECTION);
}

<SECTION>{
    {WHITESPACE}*">"{WHITESPACE}*{NEWLINE} BEGIN(INITIAL);
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
