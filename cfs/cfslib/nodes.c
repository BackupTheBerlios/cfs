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
#include <string.h>

#include "nodes.h"

struct cnfnode *create_cnfnode(const char *name)
{
  struct cnfnode *cn = NULL;

  cn = (struct cnfnode *)malloc(sizeof(struct cnfnode));
  if(cn){
    memset(cn, 0, sizeof(struct cnfnode));
    
    cn->name = strdup(name);
  }
  return cn;
}

void cnfnode_setval(struct cnfnode *cn, const char *value)
{
  if(cn){
    if(cn->value) free(cn->value);
    cn->value = strdup(value);
  }
}

void cnfnode_appendval(struct cnfnode *cn, const char *value){
  if(cn){
    char *tmp;
    if(!cn->value){
      cnfnode_setval(cn,value);
      return;
    }
    tmp = strdup(cn->value);
    tmp = realloc(tmp, sizeof(char)*(strlen(tmp)+strlen(value)+1));
    strcat(tmp, value);
    cnfnode_setval(cn,tmp);
    free(tmp);
  }	
}

void destroy_cnfnode(struct cnfnode *cn)
{
  if(cn){
    if(cn->name) free(cn->name);
    if(cn->value) free(cn->value);
    free(cn);
  }
}

void destroy_cnftree(struct cnfnode *cn_root)
{
  struct cnfnode *cn = NULL, *cn_next;

  for(cn = cn_root->first_child; cn; cn = cn_next){
    cn_next = cn->next;
    destroy_cnftree(cn);
  }
  destroy_cnfnode(cn_root);
}

void append_node(struct cnfnode *cn_parent, struct cnfnode *cn)
{
  struct cnfnode **cnp;

  for(cnp = &(cn_parent->first_child); *cnp; cnp = &((*cnp)->next));
  *cnp = cn;
  cn->parent = cn_parent;
}

void unlink_node(struct cnfnode *cn)
{
  struct cnfnode **pcn;

  for(pcn = &(cn->parent->first_child); *pcn && *pcn != cn; pcn = &((*pcn)->next));
  if(*pcn)
    *pcn = cn->next;
}

struct cnfnode *find_node(struct cnfnode *cn_list, const char *name)
{
  struct cnfnode *cn;

  for(cn = cn_list; cn; cn = cn->next){
    if(strcmp(cn->name, name) == 0)
      return cn;
  }
  return NULL;
}

void dump_nodes(struct cnfnode *cn_root, int level)
{
  int i;
  struct cnfnode *cn;

  for(i = 0; i < level; i++)
    putchar('\t');
  printf("%s", cn_root->name);
  if(cn_root->value)
    printf(" = '%s'", cn_root->value);
  putchar('\n');

  for(cn = cn_root->first_child; cn; cn = cn->next)
    dump_nodes(cn, level+1);
}

