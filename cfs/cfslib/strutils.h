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

char *dup_next_word(char **pp);
char *dup_next_word_b(char **pp, char *buf, int n);
char *dup_next_quoted(char **pp, char qchar);
char *dup_next_quoted_b(char **pp, char *buf, int n, char qchar);
char *dup_next_line(char **pp);
char *dup_next_line_b(char **pp, char *buf, int n);
