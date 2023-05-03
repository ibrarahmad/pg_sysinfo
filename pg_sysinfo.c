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
#include <sys/resource.h>

#include "postmaster/bgworker.h"
#include "fmgr.h"
#include "miscadmin.h"
#include "storage/ipc.h"
#include "storage/latch.h"
#include "storage/proc.h"
#include "storage/shmem.h"
#include "storage/spin.h"
#include "utils/builtins.h"
#include "utils/guc.h"
#include "utils/memutils.h"
#include "utils/timestamp.h"
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

void _PG_init(void);

Datum sysinfo_os_name(PG_FUNCTION_ARGS);
Datum sysinfo_os_release(PG_FUNCTION_ARGS);                                     
Datum sysinfo_os_version(PG_FUNCTION_ARGS);                                     
Datum sysinfo_cpu(PG_FUNCTION_ARGS);                                            
Datum sysinfo_disk(PG_FUNCTION_ARGS);                                           
Datum sysinfo_ram(PG_FUNCTION_ARGS);                                            

PG_FUNCTION_INFO_V1(my_worker_task);
PG_FUNCTION_INFO_V1(sysinfo_os_name);
PG_FUNCTION_INFO_V1(sysinfo_os_release);
PG_FUNCTION_INFO_V1(sysinfo_os_version);
PG_FUNCTION_INFO_V1(sysinfo_cpu);                                           
PG_FUNCTION_INFO_V1(sysinfo_cpu_usage);                                           
PG_FUNCTION_INFO_V1(sysinfo_disk);                                          
PG_FUNCTION_INFO_V1(sysinfo_ram);                                          

void _PG_init(void)
{
		/* Init */
}

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

/*
 * sysinfo_ram - a PostgreSQL function to retrieve RAM usage information
 *
 * This function uses the sysinfo() function to retrieve the total, used, and free RAM.
 * It then builds a tuple with three Float8 values, representing the total, used, and free RAM in gigabytes (GB).
 *
 * Parameters:
 * - None
 *
 * Returns:
 * - A tuple with three Float8 values: total RAM, used RAM, and free RAM (in GB)
 *
 * Throws:
 * - ERROR if failed to retrieve RAM information or create composite result type
 */

Datum sysinfo_ram(PG_FUNCTION_ARGS)
{
    /* Define variables */
    struct sysinfo si;
    double total_ram, free_ram, used_ram;
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
    values[0] = Float8GetDatumFast(total_ram / (1024 * 1024 * 1024));
    values[1] = Float8GetDatumFast(used_ram / (1024 * 1024 * 1024));
    values[2] = Float8GetDatumFast(free_ram / (1024 * 1024 * 1024));

    /* Build the tuple */
    tuple = heap_form_tuple(tupleDesc, values, nulls);

    /* Make the result */
    result = HeapTupleGetDatum(tuple);

    /* Return the result */
    PG_RETURN_DATUM(result);
}

/* Returns information about disk space usage. */
Datum sysinfo_disk(PG_FUNCTION_ARGS)
{
	ReturnSetInfo *rsinfo = (ReturnSetInfo *) fcinfo->resultinfo;
    TupleDesc tupdesc;
    Datum values[4];
    bool nulls[4] = {false};
    double total_size = 0;
    double used_size = 0;
    double free_size = 0;
	Tuplestorestate *tupstore;
 	MemoryContext per_query_ctx;                                                
 	MemoryContext oldcontext;                                                   
	struct statvfs fsInfo;
    int result = 0;
	FILE *mtabFile;
    char device[256], mountPoint[256];

	/* Switch into long-lived context to construct returned data structures */
	per_query_ctx = rsinfo->econtext->ecxt_per_query_memory;                    
    oldcontext = MemoryContextSwitchTo(per_query_ctx);
	
	/* Build a tuple descriptor for our result type */
    if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
        elog(ERROR, "return type must be a row type");

	tupstore = tuplestore_begin_heap(true, false, 1024);
    /* Build the result tuple */
 	
	rsinfo->returnMode = SFRM_Materialize;                                      
    rsinfo->setResult = tupstore;                                               
    rsinfo->setDesc = tupdesc;  

    result = statvfs("/", &fsInfo); // Get the root filesystem info
    if (result == 0)
	{
		mountPoint[0] = '/'; mountPoint[1] = 0;
		total_size = fsInfo.f_blocks * fsInfo.f_frsize;
		free_size  = fsInfo.f_bfree * fsInfo.f_frsize;
		used_size  = (fsInfo.f_blocks - fsInfo.f_bfree) * fsInfo.f_frsize;
    	
		values[0] = CStringGetTextDatum(mountPoint);
    	values[1] = Float8GetDatumFast(total_size/ (1024*1024*1024)); 
    	values[2] = Float8GetDatumFast(used_size /(1024*1024*1024));
    	values[3] =	Float8GetDatumFast(free_size / (1024*1024*1024));
		tuplestore_putvalues(tupstore, tupdesc, values, nulls);
	}

    mtabFile = fopen("/etc/mtab", "r"); // Open /etc/mtab file to get all mounted filesystems
    if (mtabFile != NULL) 
	{
        while (!feof(mtabFile)) {
            result = fscanf(mtabFile, "%255s %255s", device, mountPoint);
            if (result == 2)
		   	{
                result = statvfs(mountPoint, &fsInfo);
				if (result == 0)
				{
					total_size = fsInfo.f_blocks * fsInfo.f_frsize;
					free_size  = fsInfo.f_bfree * fsInfo.f_frsize;
					used_size  = (fsInfo.f_blocks - fsInfo.f_bfree) * fsInfo.f_frsize;
    				values[0] = CStringGetTextDatum(mountPoint);
    				values[1] = Float8GetDatumFast(total_size / (1024*1024*1024));
    				values[2] = Float8GetDatumFast(used_size / (1024*1024*1024));
    				values[3] = Float8GetDatumFast(free_size / (1024*1024*1024));
					tuplestore_putvalues(tupstore, tupdesc, values, nulls);
            	}
			}
        }
        fclose(mtabFile);
    }
	tuplestore_donestoring(tupstore);
    /* Switch back to the original context before returning */
    MemoryContextSwitchTo(oldcontext);

	return (Datum) 0; 
}

Datum
sysinfo_cpu_usage(PG_FUNCTION_ARGS)
{
	ReturnSetInfo *rsinfo = (ReturnSetInfo *) fcinfo->resultinfo;
    TupleDesc tupdesc;
    Datum values[3];
    bool nulls[3] = {false};
    double user_time;
    double system_time;
    double total_time;
	struct rusage usage;
	Tuplestorestate *tupstore;
	MemoryContext per_query_ctx;
	MemoryContext oldcontext;                                                   
    
	/* Switch into long-lived context to construct returned data structures */
	per_query_ctx = rsinfo->econtext->ecxt_per_query_memory;                    
    oldcontext = MemoryContextSwitchTo(per_query_ctx);

	 /* Build a tuple descriptor for our result type */
    if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
        elog(ERROR, "return type must be a row type");

    if (getrusage(RUSAGE_SELF, &usage) != 0) {
        perror("Error calling getrusage");
        return 1;
    }

    user_time = (double) usage.ru_utime.tv_sec + (double) usage.ru_utime.tv_usec / 1000000.0;
    system_time = (double) usage.ru_stime.tv_sec + (double) usage.ru_stime.tv_usec / 1000000.0;
    total_time = user_time + system_time;

	tupstore = tuplestore_begin_heap(true, false, 1024);

	/* Build the result tuple */
	rsinfo->returnMode = SFRM_Materialize;                                      
    rsinfo->setResult = tupstore;                                               
    rsinfo->setDesc = tupdesc;  

    values[0] = Float8GetDatumFast(user_time);
    values[1] = Float8GetDatumFast(system_time);
    values[2] = Float8GetDatumFast(total_time);
	tuplestore_putvalues(tupstore, tupdesc, values, nulls);
	
	tuplestore_donestoring(tupstore);
	MemoryContextSwitchTo(oldcontext);

	return (Datum) 0; 
}

Datum
sysinfo_cpu(PG_FUNCTION_ARGS)
{
	ReturnSetInfo *rsinfo = (ReturnSetInfo *) fcinfo->resultinfo;
    TupleDesc tupdesc;
    Datum values[3];
    bool nulls[3] = {false};
    int ncpus;
    int ncores;
    char *cpu_make;
    FILE *cpuinfo;
    char line[1024];
	Tuplestorestate *tupstore;
	MemoryContext per_query_ctx;
	MemoryContext oldcontext;                                                   
    
	cpuinfo = fopen("/proc/cpuinfo", "r");

    if (cpuinfo == NULL)
        ereport(ERROR, (errcode(ERRCODE_EXTERNAL_ROUTINE_INVOCATION_EXCEPTION), errmsg("Failed to open /proc/cpuinfo")));
    
	/* Switch into long-lived context to construct returned data structures */
	per_query_ctx = rsinfo->econtext->ecxt_per_query_memory;                    
    oldcontext = MemoryContextSwitchTo(per_query_ctx);

	 /* Build a tuple descriptor for our result type */
    if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
        elog(ERROR, "return type must be a row type");

    ncpus = 0;
    ncores = 0;
    cpu_make = NULL;

    while (fgets(line, sizeof(line), cpuinfo) != NULL) {
        char *name = NULL;
        char *value = NULL;

        if (sscanf(line, "%m[^:]:%ms", &name, &value) == 2) {
            /* Remove leading and trailing whitespace from name and value */
            char *p = name + strlen(name) - 1;

            while (isspace(*p) && p > name) {
                *p-- = '\0';
            }

            p = value + strlen(value) - 1;

            while (isspace(*p) && p > value) {
                *p-- = '\0';
            }

            if (strcmp(name, "model name") == 0) {
                /* This is the CPU make */
                cpu_make = value;
            } else if (strcmp(name, "processor") == 0) {
                /* This is a logical CPU */
                ncpus++;
            } else if (strcmp(name, "core id") == 0) {
                /* This is a core within a physical CPU */
                ncores++;
            }

            /* Free memory allocated by sscanf */
            free(name);
            free(value);
        }
    }

    fclose(cpuinfo);
	
	tupstore = tuplestore_begin_heap(true, false, 1024);
    /* Build the result tuple */
 	
	rsinfo->returnMode = SFRM_Materialize;                                      
    rsinfo->setResult = tupstore;                                               
    rsinfo->setDesc = tupdesc;  

	values[0] = cpu_make == NULL ? (Datum) 0 : CStringGetTextDatum(cpu_make);
    values[1] = Int32GetDatum(ncpus);
    values[2] = Int32GetDatum(ncores);
	tuplestore_putvalues(tupstore, tupdesc, values, nulls);
	
	tuplestore_donestoring(tupstore);
	MemoryContextSwitchTo(oldcontext);

	return (Datum) 0; 
}
