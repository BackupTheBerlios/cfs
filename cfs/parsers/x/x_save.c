#include <string.h>
#include "cfs.h"
#include "../parseutils.h"

static int x_save_recurs(FILE * f, cfile root, int level);

int x_save(FILE * f, cfile root){
    x_save_recurs(f,root,0);
}

static int x_save_recurs(FILE * f, cfile root, int level){
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
	    x_save_recurs(f,cur,level+1);
	    fprintf(f,"</%s>\n",cur->name);
	}
	cur = cur->next;
    }
}