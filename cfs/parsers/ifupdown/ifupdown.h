#ifndef PARSER_IFUPDOWN_H
#define PARSER_IFUPDOWN_H
struct cnfnode * ifupdown_parse(FILE *);
int ifupdown_save(FILE *, cfile);
#endif
