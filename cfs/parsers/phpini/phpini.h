#ifndef PARSER_PHPINI_H
#define PARSER_PHPINI_H
struct cnfnode * phpini_parse(FILE *);
int phpini_save(FILE *, cfile);
#endif
