CREATE EXTENSION IF NOT EXISTS plpgsql;

CREATE TYPE sysinfo_data AS (
    name text,
    value text
);
CREATE OR REPLACE FUNCTION sysinfo_os_name()
RETURNS text
AS 'MODULE_PATHNAME', 'sysinfo_os_name'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION sysinfo_os_release()
RETURNS text
AS 'MODULE_PATHNAME', 'sysinfo_os_release'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION sysinfo_os_version()
RETURNS text
AS 'MODULE_PATHNAME', 'sysinfo_os_version'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION sysinfo_cpu
(
		OUT cpu TEXT,
		OUT num_cpu INT,
		OUT num_core INT
)
RETURNS SETOF record
AS 'MODULE_PATHNAME', 'sysinfo_cpu'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION sysinfo_cpu_usage
(
		OUT user_time float8,
		OUT sys_time float8,
		OUT total_time float8
)
RETURNS SETOF record
AS 'MODULE_PATHNAME', 'sysinfo_cpu_usage'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION sysinfo_disk
(
		OUT mount TEXT,
		OUT total_size float8,
		OUT used_size float8,
		OUT free_size float8
)
RETURNS SETOF record
AS 'MODULE_PATHNAME', 'sysinfo_disk'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION sysinfo_ram
(
		OUT total_memory float8,
		OUT used_memory float8,
		OUT free_memory float8
)
RETURNS SETOF record
AS 'MODULE_PATHNAME', 'sysinfo_ram'
LANGUAGE C STRICT;

CREATE VIEW pg_osinfo AS SELECT * FROM sysinfo_os_name(), sysinfo_os_release(), sysinfo_os_version(); 
CREATE VIEW pg_cpuinfo AS SELECT * FROM sysinfo_cpu();
CREATE VIEW pg_cpu_usage AS SELECT * FROM sysinfo_cpu_usage();
CREATE VIEW pg_raminfo AS SELECT * FROM sysinfo_ram();
CREATE VIEW pg_diskinfo AS SELECT * FROM sysinfo_disk();

CREATE TABLE pg_sys_usage AS SELECT now(), * FROM pg_cpu_usage, pg_raminfo;

CREATE OR REPLACE FUNCTION pg_update_sys_usage()
RETURNS VOID AS $$
BEGIN
    INSERT INTO pg_sys_usage (now, user_time, sys_time, total_time, total_memory, used_memory, free_memory)
    SELECT now(), user_time, sys_time, total_time, total_memory, used_memory, free_memory FROM pg_cpu_usage, pg_raminfo;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION pg_adjust_setting()
RETURNS VOID AS $$
BEGIN
	set_shared_buffers(get_shared_buffers() + 1024*1024);
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_shared_buffers() RETURNS integer AS $$           
DECLARE                                                                         
    shared_buffers_setting text;                                             
BEGIN                                                                           
    EXECUTE 'SHOW shared_buffers' INTO shared_buffers_setting;                  
    RETURN regexp_replace(shared_buffers_setting, '[^0-9]', '', 'g')::integer;                                              
END;                                                                            
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION set_shared_buffers(size varchar)
RETURNS void AS
$$
BEGIN
  EXECUTE 'ALTER SYSTEM SET shared_buffers = ' || size;
END;
$$
LANGUAGE plpgsql;


CREATE VIEW pg_suggested_settings AS SELECT (total_memory * 40) / 100 AS shared_buffer, (total_memory * 2) / 100 work_mem,
 (total_memory * 4) / 100 as maintance_workmem FROM sysinfo_ram(); 
