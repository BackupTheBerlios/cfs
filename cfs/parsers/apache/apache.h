#ifndef PARSER_APACHE_H
#define PARSER_APACHE_H
struct cnfnode * apache_parse(FILE *);
int apache_save(FILE *, cfile);
#endif
