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
#include <unistd.h>
#include <errno.h>

#include "cfs.h"
#include "config.h"

#ifndef MODULE_DIR
#	define MODULE_DIR "/usr/lib/cfs"
#endif

FILE *open_file_or_exit(const char *fname, const char *mode)
{
  FILE *fptr;

  if(strcmp(fname, "-") == 0){
    if(mode[0] == 'r')
      fptr = stdin;
    else
      fptr = stdout;
  }else{
    fptr = fopen(fname, mode);
    if(!fptr){
      fprintf(stderr, "could not open '%s': %s\n", fname, strerror(errno));
      exit(1);
    }
  }
  return fptr;
}

FILE *open_tmpfile_or_exit(const char *fname)
{
  if(strcmp(fname, "-") == 0){
    return stdout;
  }else{
    char tmpname[256];
    snprintf(tmpname, 256, "%s.tmp", fname);
    return open_file_or_exit(tmpname, "w");
  }
}

void move_tmpfile_or_exit(const char *fname)
{
  if(strcmp(fname, "-") == 0)
    return;
  else{
    char tmpname[256];
    snprintf(tmpname, 256, "%s.tmp", fname);
    if(rename(tmpname, fname) != 0){
      fprintf(stderr, "could not move '%s' to '%s' : %s\n",
	      tmpname, fname, strerror(errno));
      exit(1);
    }
  }
}

struct cnfnode *parse_options(const char *string)
{
  struct cnfnode *cn_top, *cn;
  const char *p;
  char *q, buf[256];

  cn_top = create_cnfnode("(root)");

  p = string;
  while(*p){
    q = buf;
    while(*p && *p != '=' && *p != ',' && q < buf+255) *q++ = *p++;
    *q = 0;
    cn = create_cnfnode(buf);
    append_node(cn_top, cn);

    if(*p && *p == '='){
      p++; q = buf;
      while(*p && *p != ',' && q < buf+255) *q++ = *p++;
      *q = 0;
      cnfnode_setval(cn, buf);

    }else
      cnfnode_setval(cn, "");
    if(*p) p++;
  }

  return cn_top;
}

int main(int argc, char *argv[])
{
  char *fname_in = NULL, *fname_out = NULL;
  char *modname = NULL;
  struct cnfnode *opt_root = NULL;
  int exit_code = 0;
  int showhidden = 0;

  if(argc > 1){
    modname = argv[1];
    argc--;
    argv++;
  }else
    exit(1);

  while(1){
    int c;

    c = getopt(argc, argv, "i:o:f:hO:");

    if (c == -1)
      break;

    switch(c){
    case 'i':
      fname_in = optarg;
      break;
    case 'h':
      showhidden = 1;
      break;
    case 'o':
      fname_out = optarg;
      break;
    case 'f':
      fname_out = fname_in = optarg;
      break;
    case 'O':
      opt_root = parse_options(optarg);
      break;
    case '?':
      break;
    default:
      printf ("?? getopt returned character code 0%o ??\n", c);
    }
  }

  if (optind < argc) {
    FILE *fin, *fout;
    struct confline *lines;
    struct cnfnode *cn_root;
    struct cnfresult *cnf_res = NULL, *cr;
    char *action = argv[optind];
    struct cnfmodule *module;
    
    loadmodule(MODULE_DIR,modname,opt_root);

    module = find_cnfmodule(modname);
    if(!module){
      fprintf(stderr, "no module with name '%s' found.\n", modname);
      exit(1);
      exit_code = 1;
    }
      
    if(!fname_in)
      fname_in = module->default_file;

    if(!fname_out)
      fname_out = module->default_file;

    if(fname_in){
      fin = open_file_or_exit(fname_in, "r");
    }else{
      fprintf(stderr, "no input filename given, and the module has no default.\n");
      exit_code = 1;
      exit(1);
    }

    cn_root = module->parser(fin);
    
    if(fin != stdin) fclose(fin);

    if(strcmp(action, "dump") == 0){

      dump_nodes(cn_root, 0,showhidden);

    }else if(strcmp(action, "get") == 0){

      cnf_res = cnf_find_entry(cn_root, argv[optind+1]);

      if(cnf_res){
	for(cr = cnf_res; cr; cr = cr->next){
	  printf("%s\n", cr->cnfnode->value ? cr->cnfnode->value : "");
	}
      }else
	exit_code = 1;

    }else if(strcmp(action, "exists") == 0){

      cnf_res = cnf_find_entry(cn_root, argv[optind+1]);

      if(!cnf_res)
	exit_code = 1;

    }else if((strcmp(action, "set") == 0) || (strcmp(action, "unparse") == 0)){
      int result;

      if(!fname_out){
	fprintf(stderr, "no output filename given, and the module has no default.\n");
	exit(1);
      }

      if(strcmp(action, "set") == 0){
	cnf_res = cnf_find_entry(cn_root, argv[optind+1]);
	if(cnf_res){
	  /* we set the _first_ found result */
	  cnfnode_setval(cnf_res->cnfnode, argv[optind+2]);
	}else{
	  fprintf(stderr, "%s not found\n", argv[optind+1]);
	  exit_code = 1;
	}
      }

      fout = open_tmpfile_or_exit(fname_out);

      result = module->unparser(fout, cn_root);
      if(fout != stdout) fclose(fout);

      move_tmpfile_or_exit(fname_out);

    }else if((strcmp(action, "add") == 0) || (strcmp(action, "del") == 0) || (strcmp(action, "rem") == 0)){
      int result;

      if(!fname_out){
	fprintf(stderr, "no output filename given, and the module has no default.\n");
	exit(1);
      }

      if(strcmp(action, "add") == 0){
	cnf_res = cnf_find_entry(cn_root, argv[optind+1]);
	if(cnf_res){
	  /* we add/del to the _first_ found result */
	  cnf_add_branch(cnf_res->cnfnode, argv[optind+2], 0);
	}else{
	  fprintf(stderr, "%s not found\n", argv[optind+1]);
	  exit_code = 1;
	}
      }else
	cnf_del_branch(cn_root, argv[optind+1], strcmp(action, "del") == 0);

      fout = open_tmpfile_or_exit(fname_out);

      result = module->unparser(fout, cn_root);
      if(fout != stdout) fclose(fout);

      move_tmpfile_or_exit(fname_out);

    }else if(strcmp(action, "list") == 0){

      cnf_res = cnf_find_entry(cn_root, argv[optind+1]);
      for(cr = cnf_res; cr; cr = cr->next){
	struct cnfnode *cn = cr->cnfnode;
	  for(cn = cn->first_child;cn; cn = cn->next)
	    printf("%s\n", cn->name);
      }
    }
  }
  exit(exit_code);
}
