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

CREATE OR REPLACE FUNCTION sysinfo_disk()
RETURNS text
AS 'MODULE_PATHNAME', 'sysinfo_disk'
LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION sysinfo_ram()
RETURNS text
AS 'MODULE_PATHNAME', 'sysinfo_ram'
LANGUAGE C STRICT;

CREATE VIEW pg_osinfo AS SELECT * FROM sysinfo_os_name(), sysinfo_os_release(), sysinfo_os_version(); 
