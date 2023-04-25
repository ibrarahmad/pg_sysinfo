#include "postgres.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/array.h"
#include "utils/elog.h"
#include "funcapi.h"

#include "pg_sysinfo.h"

PG_MODULE_MAGIC;

typedef struct {
    char* name;
    int total_size;
    int used_size;
    int free_size;
} sysinfo_data;

Datum sysinfo_os_name(PG_FUNCTION_ARGS);                                        
Datum sysinfo_os_release(PG_FUNCTION_ARGS);                                     
Datum sysinfo_os_version(PG_FUNCTION_ARGS);                                     
Datum sysinfo_cpu(PG_FUNCTION_ARGS);                                            
Datum sysinfo_disk(PG_FUNCTION_ARGS);                                           
Datum sysinfo_ram(PG_FUNCTION_ARGS);                                            

PG_FUNCTION_INFO_V1(sysinfo_os_name);
PG_FUNCTION_INFO_V1(sysinfo_os_release);
PG_FUNCTION_INFO_V1(sysinfo_os_version);
PG_FUNCTION_INFO_V1(sysinfo_cpu);                                           
PG_FUNCTION_INFO_V1(sysinfo_disk);                                          
PG_FUNCTION_INFO_V1(sysinfo_ram);                                          

/* Returns the name of the operating system. */
Datum sysinfo_os_name(PG_FUNCTION_ARGS)
{
    struct utsname utsname;
    char os_name[256];

    if (uname(&utsname) != 0) {
        ereport(ERROR,
                (errcode(ERRCODE_EXTERNAL_ROUTINE_INVOCATION_EXCEPTION),
                 errmsg("Failed to retrieve OS information")));
    }

    snprintf(os_name, sizeof(os_name), "%s", utsname.sysname);

    PG_RETURN_TEXT_P(cstring_to_text(os_name));
}

/* Returns the release of the operating system. */
Datum sysinfo_os_release(PG_FUNCTION_ARGS)
{
    struct utsname utsname;
    char os_release[256];

    if (uname(&utsname) != 0) {
        ereport(ERROR,
                (errcode(ERRCODE_EXTERNAL_ROUTINE_INVOCATION_EXCEPTION),
                 errmsg("Failed to retrieve OS information")));
    }

    snprintf(os_release, sizeof(os_release), "%s", utsname.release);

    PG_RETURN_TEXT_P(cstring_to_text(os_release));
}

/* Returns the version of the operating system. */
Datum sysinfo_os_version(PG_FUNCTION_ARGS)
{
    struct utsname utsname;
    char os_version[256];

    if (uname(&utsname) != 0) {
        ereport(ERROR,
                (errcode(ERRCODE_EXTERNAL_ROUTINE_INVOCATION_EXCEPTION),
                 errmsg("Failed to retrieve OS information")));
    }

    snprintf(os_version, sizeof(os_version), "%s", utsname.version);

    PG_RETURN_TEXT_P(cstring_to_text(os_version));
}
Datum sysinfo_ram(PG_FUNCTION_ARGS)
{
    struct sysinfo si;
    long total_ram, free_ram, used_ram;
    Datum values[3];
    bool nulls[3] = { false, false, false };
    TupleDesc tupleDesc;
    HeapTuple tuple;
    Datum result;

    /* Retrieve RAM information */
    if (sysinfo(&si) != 0) {
        ereport(ERROR, (errmsg("Failed to retrieve RAM information")));
    }

    /* Calculate used and free RAM */
    total_ram = (long)si.totalram * si.mem_unit;
    free_ram = (long)si.freeram * si.mem_unit;
    used_ram = total_ram - free_ram;

    /* Build the tuple descriptor */
    if (get_call_result_type(fcinfo, NULL, &tupleDesc) != TYPEFUNC_COMPOSITE) {
        ereport(ERROR, (errmsg("Unable to create composite result type")));
    }

    /* Set the values */
    values[0] = CStringGetTextDatum("RAM");
    values[1] = CStringGetTextDatum(psprintf("%ld/%ld MB", used_ram / (1024 * 1024), total_ram / (1024 * 1024)));
    values[2] = CStringGetTextDatum(psprintf("%.2f%%", (double)used_ram / total_ram * 100));

    /* Build the tuple */
    tuple = heap_form_tuple(tupleDesc, values, nulls);

    /* Make the result */
    result = HeapTupleGetDatum(tuple);

    PG_RETURN_DATUM(result);
}

/* Returns information about disk space usage. */
Datum sysinfo_disk(PG_FUNCTION_ARGS)
{
    DIR* dir;
    struct dirent* entry;
    struct statvfs stat;
    int block_size;
    int total_size = 0;
    int used_size = 0;
    int free_size = 0;
    sysinfo_data* result;

    MemoryContext old_context = CurrentMemoryContext;
    MemoryContext temp_context = AllocSetContextCreate(CurrentMemoryContext,
                                                       "sysinfo_disk temporary context",
                                                       ALLOCSET_SMALL_MINSIZE,
                                                       ALLOCSET_SMALL_INITSIZE,
                                                       ALLOCSET_SMALL_MAXSIZE);

    /* Switch to the temporary context for allocating the result */
    MemoryContextSwitchTo(temp_context);

    dir = opendir("/");

    if (dir == NULL) {
        ereport(ERROR,
                (errcode(ERRCODE_EXTERNAL_ROUTINE_INVOCATION_EXCEPTION),
                 errmsg("Failed to open root directory")));
    }

    while ((entry = readdir(dir)) != NULL) {
        char path[PATH_MAX];

        if (entry->d_name[0] == '.') {
            continue;
        }

        snprintf(path, sizeof(path), "/%s", entry->d_name);

        if (statvfs(path, &stat) != 0) {
            continue;
        }

        block_size = stat.f_frsize;
        total_size += block_size * stat.f_blocks;
        used_size += block_size * (stat.f_blocks - stat.f_bfree);
        free_size += block_size * stat.f_bfree;
    }

    closedir(dir);

    /* Switch back to the original context before returning */
    MemoryContextSwitchTo(old_context);

    /* Allocate the result in the original context */
    result = (sysinfo_data*) palloc(sizeof(sysinfo_data));
    result->name = "Disk";
    result->total_size = total_size;
    result->used_size = used_size;
    result->free_size = free_size;

    PG_RETURN_POINTER(result);
}


