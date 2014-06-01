#include <errno.h>
#include <glob.h>
#include <limits.h>
#include <string.h> /* strdup */

#include <alpm.h>

#include "utils.h"

#include "ini.h"

static const int ini_max_recursion = 10;

/**
 * @brief INI parser backend.
 *
 * @param file path to the config file
 * @param cb callback for key/value pairs
 * @param data caller defined data to be passed to the callback
 * @param section_name the name of the current section
 * @param line buffer to read into, must be at least PATH_MAX long
 * @param depth recursion depth, should initially be 0
 *
 * @return 0 on success, 1 on parsing errors, the callback return value
 * otherwise
 */
static int _parse_ini(const char *file, ini_parser_fn cb, void *data,
		char **section_name, char *line, int depth)
{
	FILE *fp = NULL;
	int linenum = 0;
	int ret = 0;

	if(depth >= ini_max_recursion) {
		printf("config parsing exceeded max recursion depth of %d.\n",
				ini_max_recursion);
		ret = 1;
		goto cleanup;
	}

	printf( "config: attempting to read file %s\n", file);
	fp = fopen(file, "r");
	if(fp == NULL) {
		printf("config file %s could not be read: %s\n",
				file, strerror(errno));
		ret = 1;
		goto cleanup;
	}

	while(fgets(line, PATH_MAX, fp)) {
		char *key, *value, *ptr;
		size_t line_len;

		linenum++;

		/* ignore whole line and end of line comments */
		if((ptr = strchr(line, '#'))) {
			*ptr = '\0';
		}

		line_len = strtrim(line);

		if(line_len == 0) {
			continue;
		}

		if(line[0] == '[' && line[line_len - 1] == ']') {
			char *name;
			/* only possibility here is a line == '[]' */
			if(line_len <= 2) {
				printf("config file %s, line %d: bad section name.\n",
						file, linenum);
				ret = 1;
				goto cleanup;
			}
			/* new config section, skip the '[' */
			name = strdup(line + 1);
			name[line_len - 2] = '\0';

			ret = cb(file, linenum, name, NULL, NULL, data);
			free(*section_name);
			*section_name = name;

			/* we're at a new section; perform any post-actions for the prior */
			if(ret) {
				goto cleanup;
			}
			continue;
		}

		/* directive */
		/* strsep modifies the 'line' string: 'key \0 value' */
		key = line;
		value = line;
		strsep(&value, "=");
		strtrim(key);
		strtrim(value);

		if(key == NULL) {
			printf("config file %s, line %d: syntax error in config file- missing key.\n",
					file, linenum);
			ret = 1;
			goto cleanup;
		}

		if( value == NULL ){
			printf("config file %s, line %d: directive '%s' needs a value\n",
					file, linenum, key);
			ret = 1;
			goto cleanup;
		}
			
		///* Include is allowed in both options and repo sections */
		//if(strcmp(key, "Include") == 0) {
		//	glob_t globbuf;
		//	int globret;
		//	size_t gindex;

		//	if(value == NULL) {
		//		printf("config file %s, line %d: directive '%s' needs a value\n",
		//				file, linenum, key);
		//		ret = 1;
		//		goto cleanup;
		//	}
		//	/* Ignore include failures... assume non-critical */
		//	globret = glob(value, GLOB_NOCHECK, NULL, &globbuf);
		//	switch(globret) {
		//		case GLOB_NOSPACE:
		//			printf(
		//					"config file %s, line %d: include globbing out of space\n",
		//					file, linenum);
		//			break;
		//		case GLOB_ABORTED:
		//			printf(
		//					"config file %s, line %d: include globbing read error for %s\n",
		//					file, linenum, value);
		//			break;
		//		case GLOB_NOMATCH:
		//			printf(
		//					"config file %s, line %d: no include found for %s\n",
		//					file, linenum, value);
		//			break;
		//		default:
		//			for(gindex = 0; gindex < globbuf.gl_pathc; gindex++) {
		//				printf("config file %s, line %d: including %s\n",
		//						file, linenum, globbuf.gl_pathv[gindex]);
		//				ret =_parse_ini(globbuf.gl_pathv[gindex], cb, data,
		//						section_name, line, depth + 1);
		//				if(ret) {
		//					globfree(&globbuf);
		//					goto cleanup;
		//				}
		//			}
		//			break;
		//	}
		//	globfree(&globbuf);
		//	continue;
		//}
		if((ret = cb(file, linenum, *section_name, key, value, data)) != 0) {
			goto cleanup;
		}
	}

	if(depth == 0) {
		ret = cb(NULL, 0, NULL, NULL, NULL, data);
	}

cleanup:
	if(fp) {
		fclose(fp);
	}
	if(depth == 0) {
		free(*section_name);
		*section_name = NULL;
	}
	printf("config: finished parsing %s\n", file);
	return ret;
}

/**
 * @brief Parse a pacman-style INI config file.
 *
 * @param file path to the config file
 * @param cb callback for key/value pairs
 * @param data caller defined data to be passed to the callback
 *
 * @return 0 on success, 1 on parsing errors, the callback return value
 * otherwise
 *
 * @note The callback will be called at the beginning of each section with an
 * empty key and value, for each key/value pair, and when parsing is complete
 * with all arguments except @a data empty.
 *
 * @note The @a key and @a value passed to @ cb will be overwritten between
 * calls.  The section name will remain valid until after @a cb is called to
 * begin a new section.
 *
 * @note Parsing will immediately stop if the callback returns non-zero.
 */
int parse_ini(const char *file, ini_parser_fn cb, void *data)
{
	char *section_name = NULL, line[PATH_MAX];
	return _parse_ini(file, cb, data, &section_name, line, 0);
}

