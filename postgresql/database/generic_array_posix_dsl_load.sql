CREATE FUNCTION generic_array_dsl_posix(double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision) RETURNS double precision AS '$libdir/c','generic_array_dsl_posix' LANGUAGE C STRICT;