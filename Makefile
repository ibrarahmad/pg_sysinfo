MODULE_big = pg_sysinfo
OBJS = pg_sysinfo.o

EXTENSION = pg_sysinfo
DATA = pg_sysinfo--1.0.sql
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

