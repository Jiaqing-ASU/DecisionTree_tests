CREATE FUNCTION paths_dsl_s_needed(double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision,double precision) RETURNS double precision AS '$libdir/c','paths_dsl_s_needed' LANGUAGE C STRICT;