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

struct cnfnode {
  struct cnfnode *next;
  char *name;
  char *value;
  struct cnfnode *first_child;
  struct cnfnode *parent;
};

/* allocate empty node struct, copy name */
struct cnfnode *create_cnfnode(const char *name);

void cnfnode_setval(struct cnfnode *cn, const char *value);
void cnfnode_appendval(struct cnfnode *cn, const char *value);


/* free memory of node, leaving children intact */
void destroy_cnfnode(struct cnfnode *cn);

/* free recursively all children, then cn */
void destroy_cnftree(struct cnfnode *cn);

/* append a node */
void append_node(struct cnfnode *cn_parent, struct cnfnode *cn);

/* remove a node from tree list (but do not destroy it) */
void unlink_node(struct cnfnode *cn);

/* walks through list starting with cnf_list,
   and returns 1st node with matching name */
struct cnfnode *find_node(struct cnfnode *cn_list, const char *name);

