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

#define TRIM_WHITESPACE \
    while(*yytext && (*yytext==' ' || *yytext=='\t')){ \
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

static char * nullsafe(char * text){
    if(text == NULL)
	return "";
    return text;
}
