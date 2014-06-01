#ifndef INI_H

#define INI_H


typedef int (ini_parser_fn)(const char *file, int line, const char *section,
		char *key, char *value, void *data);

int parse_ini(const char *file, ini_parser_fn cb, void *data);

		
#endif /* end of include guard: INI_H */
