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
	if(cur->value){
	    if(!strcmp(cur->name, ".comment"))
		fprintf(f,"#%s\n",cur->value);
	    else if(!strcmp(cur->name, ".whitespace"))
		fprintf(f,"%s\n",cur->value);
	    else
	    	fprintf(f,"%s %s\n",cur->name,cur->value);
	} else	
	    fprintf(f,"<%s>\n",cur->name);
	
	if(cur->first_child)
	    apache_save_recurs(f,cur,level+1);
	fprintf("</%s>\n",cur->name);
	cur = cur->next;
    }
}
