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


#include <string.h>
#include "cfs.h"
#include "../parseutils.h"

static int apache_save_recurs(FILE * f, cfile root, int level);

int apache_save(FILE * f, cfile root){
    apache_save_recurs(f,root,0);
}

static int apache_save_recurs(FILE * f, cfile root, int level){
    cfile cur;
    cur = root->first_child;
    
    while(cur){
	if(!cur->first_child){
	    if(!strcmp(cur->name, ".comment"))
		fprintf(f,"#%s\n",nullsafe(cur->value));
	    else if(!strcmp(cur->name, ".whitespace"))
		fprintf(f,"%s",nullsafe(cur->value));
	    else
	    	fprintf(f,"%s %s\n",cur->name,nullsafe(cur->value));
	} else	
	    fprintf(f,"<%s %s>\n",cur->name,nullsafe(cur->value));
	
	if(cur->first_child){
	    apache_save_recurs(f,cur,level+1);
	    fprintf(f,"</%s>\n",cur->name);
	}
	cur = cur->next;
    }
}
