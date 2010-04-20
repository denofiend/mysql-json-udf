USE mysql;

DROP FUNCTION IF EXISTS json_getstring;
CREATE FUNCTION json_getstring RETURNS STRING SONAME 'lib_jsonudf.so';
