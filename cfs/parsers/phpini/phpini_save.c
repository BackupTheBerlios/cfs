#include <string.h>
#include "cfs.h"
#include "parseutils.h"

static int phpini_save_recurs(FILE * f, cfile root, int level);

int phpini_save(FILE * f, cfile root){
    phpini_save_recurs(f,root,0);
}

static int phpini_save_recurs(FILE * f, cfile root, int level){
    cfile cur;
    cur = root->first_child;
    
    while(cur){
	if(cur->value){
	    if(!strcmp(cur->name, ".comment"))
		fprintf(f,";%s\n",cur->value);
	    else
	    	fprintf(f,"%s = \"%s\"\n",cur->name,cur->value);
	}
	//else	
	
	if(cur->first_child)
	    phpini_save_recurs(f,cur,level+1);
	cur = cur->next;
    }
}
