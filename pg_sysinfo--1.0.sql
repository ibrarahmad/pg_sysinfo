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
		OUT used_memeory float8,
		OUT free_memeory float8
)
RETURNS SETOF record
AS 'MODULE_PATHNAME', 'sysinfo_ram'
LANGUAGE C STRICT;

CREATE VIEW pg_osinfo AS SELECT * FROM sysinfo_os_name(), sysinfo_os_release(), sysinfo_os_version(); 
CREATE VIEW pg_cpuinfo AS SELECT * FROM sysinfo_cpu();
CREATE VIEW pg_raminfo AS SELECT * FROM sysinfo_ram();
CREATE VIEW pg_diskinfo AS SELECT * FROM sysinfo_disk();

