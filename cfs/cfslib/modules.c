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

#include "strutils.h"
#include "nodes.h"
#include "modules.h"
#include <dlfcn.h>

struct cnfmodule *cnfmodules = NULL;

int register_cnfmodule(struct cnfmodule *cm, struct cnfnode *opt_root)
{
  struct cnfmodule **pcm;

  for(pcm = &cnfmodules; *pcm; pcm = &((*pcm)->next));
  *pcm = cm;
  cm->opt_root = opt_root;
}

struct cnfmodule *find_cnfmodule(const char *name)
{
  struct cnfmodule *cm;

  for(cm = cnfmodules; cm; cm = cm->next){
    if(strcmp(cm->name, name) == 0)
      return cm;
  }
  return NULL;
}

int loadmodule(char * module_dir, char * name, struct cnfnode *opt_root){
    char buf[1024];
    int ret;
    void * handle;
    void (*reghandle)(struct cnfnode *);
    
    snprintf(buf, 1024, "%s/lib%s.so", module_dir, name);
    handle = dlopen(buf, RTLD_LAZY);
    if(!handle)
        return -1;
    if((reghandle = dlsym(handle,"reg")) == NULL)
        return -1;
    (*reghandle)(opt_root);
    return 0;
}
