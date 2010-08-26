//  Copyright (c) 2010 Webaroo
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; under version 2 of the License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// @author Saurabh Raje

#include "mysqludf.h"
#include "json.h"

/* For Windows, define PACKAGE_STRING in the VS project */
#ifndef __WIN__
#include "config.h"
#endif

/* These must be right or mysqld will not find the symbol! */
#ifdef	__cplusplus
extern "C" {
#endif
	DLLEXP my_bool jstr_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
	DLLEXP void jstr_deinit(UDF_INIT *initid);
	DLLEXP char *jstr(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);
#ifdef	__cplusplus
}
#endif

#define LOG(str) {fprintf(stderr, str); fflush(stderr);}
#define LOGINT(val) {fprintf(stderr, "%u", val); fflush(stderr);}

/*
 * IMPLEMENTATION
 */


my_bool jstr_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  if (args->arg_count != 2)
    RETURN_ERR("Invalid arguments to jstr:(string json, string key) expected");
  args->arg_type[0] = STRING_RESULT;
  args->arg_type[1] = STRING_RESULT;
  return 0;
}

void jstr_deinit(UDF_INIT *initid)
{
}

char *jstr(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
  if (args->args[0] == NULL)
    RETURN_NULL();
  json_t *root = NULL;
  // we must ensure that the length of the field is atleast +1 than required
  char terminator = args->args[0][args->lengths[0]];
  args->args[0][args->lengths[0]] = 0;
  int parse_error = json_parse_document(&root, args->args[0]);
  if (parse_error != JSON_OK)
  {
    // input json is invalid
    args->args[0][args->lengths[0]] = terminator;
    RETURN_NULL();
  }
  args->args[0][args->lengths[0]] = terminator;
  json_t *element = json_find_first_label(root, args->args[1]);
  if (element == NULL || element->child == NULL)
  {
    // input json does not have the requested element
    json_free_value(&root);
    RETURN_NULL();
  }
  char *text = NULL;
  json_tree_to_string(element->child, &text);
  *length = strlen(text);
  memcpy(result, text, *length);
  result[*length]= 0;
  free(text);
  json_free_value(&root);
  return result;
}
