/*
    This file is part of llconf2

    Copyright (C) 2004  Oliver Kurth <oku@debian.org>

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "strutils.h"
#include "nodes.h"
#include "entry.h"

struct cnfresult *create_cnfresult(struct cnfnode *cn, const char *path)
{
  struct cnfresult *cr = NULL;

  cr = (struct cnfresult *)malloc(sizeof(struct cnfresult));
  if(cr){
    memset(cr, 0, sizeof(struct cnfresult));
    
    cr->cnfnode = cn;
    cr->path = strdup(path);
  }
  return cr;
}

void destroy_cnfresult(struct cnfresult *cr)
{
  if(cr){
    if(cr->path) free(cr->path);
    free(cr);
  }
}

void append_result(struct cnfresult *cr_prev, struct cnfresult *cr)
{
  struct cnfresult **crp;

  for(crp = &(cr_prev->next); *crp; crp = &((*crp)->next));
  *crp = cr;
}

void _cnf_find_entry(struct cnfresult **pcr, struct cnfnode *cn_parent,
		     const char *fullpath, const char *path)
{
  struct cnfresult *cr = NULL;
  struct cnfnode *cn_list;
  struct cnfnode *cn;
  char dname[256], *q;
  const char *p;

  p = path;
  cn_list = cn_parent->first_child;

  while(1){
    q = dname;
    while(*p && (*p != '/') && (q < dname+255))
      *(q++) = *(p++);
    *q = 0;
    
    if((*p == '/') && p[1]){
      p++;
      for(cn = cn_list; cn; cn = cn->next){
	if(strcmp(cn->name, dname) == 0){
	  _cnf_find_entry(pcr, cn, fullpath, p);
	}
      }
    }else{
      for(cn = cn_list; cn; cn = cn->next){
	if(strcmp(cn->name, dname) == 0){
	  cr = create_cnfresult(cn, fullpath);
	  if(*pcr){
	    append_result(*pcr, cr);
	  }else{
	    *pcr = cr;
	  }
	}
      }
      break;
    }
  }
}
      
struct cnfresult *cnf_find_entry(struct cnfnode *cn_root, const char *path)
{
  struct cnfresult *cnf_res = NULL;

  if(strcmp(path, ".") == 0)
    cnf_res = create_cnfresult(cn_root, path);
  else 
    _cnf_find_entry(&cnf_res, cn_root, path, path);

  return cnf_res;
}

struct cnfnode *cnf_add_branch(struct cnfnode *cn_root, const char *path, int do_merge)
{
  struct cnfnode *cn, *cn_parent;
  char dname[256], *q;
  const char *p;
  
  cn_parent = cn_root;
  p = path;
  while(*p){
    q = dname;
    while(*p && (*p != '/') && (*p != '=') && (q < dname+255)) *q++ = *p++;
    *q = 0;

    cn = NULL;
    if(do_merge){
      for(cn = cn_parent->first_child; cn; cn = cn->next){
	if(strcmp(cn->name, dname) == 0){
	  break;
	}
      }
    }
    if(cn == NULL){
      append_node(cn_parent, cn = create_cnfnode(dname));
    }
    cn_parent = cn;

    if(*p == '='){
      char buf[256];
      p++;
      if(*p)
	cnfnode_setval(cn, dup_next_line_b(&p, buf, 255));
      else
	cnfnode_setval(cn, "");
      break;
    }else if(*p == '/'){
      p++;
    }else break;
  }
  return cn;
}

void cnf_del_branch(struct cnfnode *cn_root, const char *path, int del_empty)
{
  struct cnfresult *cr;

  cr = cnf_find_entry(cn_root, path);
  if(cr){
    struct cnfnode *cn = cr->cnfnode;
    unlink_node(cn);
    destroy_cnftree(cn);

    if(del_empty){
      for(cn = cn->parent; cn->parent; cn = cn->parent){
	if(cn->first_child == NULL){
	  unlink_node(cn);
	  destroy_cnftree(cn);
	}
      }
    }
  }
}

	  
