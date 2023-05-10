# pg_sysinfo PostgreSQL Extension

pg_sysinfo is a PostgreSQL extension that provides a set of views to retrieve system information on a *nix system, including CPU, RAM, and Disk usage, as well as OS details. This extension is useful for monitoring and managing PostgreSQL server resources.



## Usage

Once pg_sysinfo is installed and added to the `postgresql.conf` file, users can use the following views to retrieve system information:

- `pg_osinfo` - provides information about the operating system, including the name, release, and version.
- `pg_cpuinfo` - provides information about the CPU usage, including the number of processors, load averages, and CPU usage percentages.
- `pg_raminfo` - provides information about the memory usage, including the total, used, and free memory.
- `pg_diskinfo` - provides information about the disk usage, including the total, used, and free disk space.

## Example usage:

```sql
SELECT * FROM pg_osinfo;
SELECT * FROM pg_cpuinfo;
SELECT * FROM pg_raminfo;
SELECT * FROM pg_diskinfo;
```

```sql
SELECT cron.schedule('pg_update_sys_usage', '5 seconds', 'SELECT pg_update_sys_usage()');
```


## Examples

```sql
postgres=# select * from pg_sys_usage;
              now              | user_time | sys_time |      total_time       |   total_memory    |    used_memory     |    free_memory    
-------------------------------+-----------+----------+-----------------------+-------------------+--------------------+-------------------
 2023-05-03 05:20:41.301616+00 |  0.010057 | 0.005028 |  0.015085000000000001 | 15.63836669921875 |  6.912506103515625 | 8.725860595703125
 2023-05-03 05:20:45.047082+00 |  0.007258 |        0 |              0.007258 | 15.63836669921875 |  6.913490295410156 | 8.724876403808594
 2023-05-03 05:20:50.043556+00 |  0.005274 | 0.002637 |              0.007911 | 15.63836669921875 |  6.914226531982422 | 8.724140167236328
 2023-05-03 05:20:55.044199+00 |   0.00687 |        0 |               0.00687 | 15.63836669921875 |  6.914459228515625 | 8.723907470703125
 2023-05-03 05:21:00.046433+00 |  0.006968 |        0 |              0.006968 | 15.63836669921875 |  6.914615631103516 | 8.723751068115234
 2023-05-03 05:21:05.047852+00 |  0.008065 |        0 |              0.008065 | 15.63836669921875 |  6.914436340332031 | 8.723930358886719
 2023-05-03 05:21:10.050206+00 |  0.007251 |        0 |              0.007251 | 15.63836669921875 |  6.911155700683594 | 8.727210998535156
 2023-05-03 05:21:15.054053+00 |  0.007536 |        0 |              0.007536 | 15.63836669921875 |  6.914886474609375 | 8.723480224609375
 2023-05-03 05:21:20.053328+00 |  0.003633 | 0.003633 |              0.007266 | 15.63836669921875 |  6.915180206298828 | 8.723186492919922
 2023-05-03 05:21:25.05388+00  |         0 | 0.006814 |              0.006814 | 15.63836669921875 |  6.911724090576172 | 8.726642608642578
 2023-05-03 05:21:30.057343+00 |  0.003362 | 0.003362 |              0.006724 | 15.63836669921875 |  6.911754608154297 | 8.726612091064453
 2023-05-03 05:21:35.064831+00 |  0.011794 |        0 |              0.011794 | 15.63836669921875 |  6.912715911865234 | 8.725650787353516
 2023-05-03 05:21:40.060909+00 |  0.008982 |        0 |              0.008982 | 15.63836669921875 | 6.9121246337890625 | 8.726242065429688
 2023-05-03 05:21:45.061778+00 |  0.002618 | 0.005237 |  0.007855000000000001 | 15.63836669921875 | 6.9105072021484375 | 8.727859497070312
 2023-05-03 05:21:50.062468+00 |  0.005116 |        0 |              0.005116 | 15.63836669921875 |  6.910297393798828 | 8.728069305419922
 2023-05-03 05:21:55.063316+00 |  0.006938 |        0 |              0.006938 | 15.63836669921875 |  6.911838531494141 |  8.72652816772461
 2023-05-03 05:22:00.061424+00 |  0.004631 | 0.002315 |  0.006945999999999999 | 15.63836669921875 |  6.912025451660156 | 8.726341247558594
 2023-05-03 05:22:05.063972+00 |  0.007256 |        0 |              0.007256 | 15.63836669921875 | 6.9120635986328125 | 8.726303100585938
 2023-05-03 05:22:10.061615+00 |         0 | 0.008247 |              0.008247 | 15.63836669921875 |  6.911060333251953 | 8.727306365966797
 2023-05-03 05:22:15.064655+00 |  0.006286 |        0 |              0.006286 | 15.63836669921875 | 6.9111175537109375 | 8.727249145507812
 2023-05-03 05:22:20.064977+00 |  0.002652 | 0.005305 |  0.007956999999999999 | 15.63836669921875 | 6.9111175537109375 | 8.727249145507812


postgres=# select * from pg_osinfo;
 sysinfo_os_name | sysinfo_os_release |             sysinfo_os_version              
-----------------+--------------------+---------------------------------------------
 Linux           | 5.4.0-77-generic   | #86-Ubuntu SMP Thu Jun 17 02:35:03 UTC 2021
(1 row)


postgres=# select * from pg_diskinfo;
              mount              |     total_size     |      used_size       |     free_size      
---------------------------------+--------------------+----------------------+--------------------
 /                               | 61.306278228759766 |    54.38694763183594 |  6.919330596923828
 /sys                            |                  0 |                    0 |                  0
 /proc                           |                  0 |                    0 |                  0
 /dev                            | 7.7922515869140625 |                    0 | 7.7922515869140625
 /dev/pts                        |                  0 |                    0 |                  0
 /run                            | 1.5638389587402344 |  0.00102996826171875 | 1.5628089904785156
 /                               | 61.306278228759766 |    54.38694763183594 |  6.919330596923828
 /sys/kernel/security            |                  0 |                    0 |                  0
 /dev/shm                        |  7.819183349609375 | 0.001033782958984375 |  7.818149566650391
 /run/lock                       |       0.0048828125 |                    0 |       0.0048828125
 /sys/fs/cgroup                  |  7.819183349609375 |                    0 |  7.819183349609375
 /sys/fs/cgroup/unified          |                  0 |                    0 |                  0
 /sys/fs/cgroup/systemd          |                  0 |                    0 |                  0
 /sys/fs/pstore                  |                  0 |                    0 |                  0
 /sys/fs/bpf                     |                  0 |                    0 |                  0
 /sys/fs/cgroup/memory           |                  0 |                    0 |                  0
 /sys/fs/cgroup/pids             |                  0 |                    0 |                  0
 /sys/fs/cgroup/net_cls,net_prio |                  0 |                    0 |                  0
 /sys/fs/cgroup/devices          |                  0 |                    0 |                  0
 /sys/fs/cgroup/cpuset           |                  0 |                    0 |                  0
 /sys/fs/cgroup/cpu,cpuacct      |                  0 |                    0 |                  0
 /sys/fs/cgroup/rdma             |                  0 |                    0 |                  0
 /sys/fs/cgroup/freezer          |                  0 |                    0 |                  0
 /sys/fs/cgroup/perf_event       |                  0 |                    0 |                  0
 /sys/fs/cgroup/hugetlb          |                  0 |                    0 |                  0
 /sys/fs/cgroup/blkio            |                  0 |                    0 |                  0
 /proc/sys/fs/binfmt_misc        |                  0 |                    0 |                  0
 /dev/mqueue                     |                  0 |                    0 |                  0
 /dev/hugepages                  |                  0 |                    0 |                  0
 /sys/kernel/debug               |                  0 |                    0 |                  0
 /sys/kernel/tracing             |                  0 |                    0 |                  0
 /run/rpc_pipefs                 |                  0 |                    0 |                  0
 /sys/kernel/config              |                  0 |                    0 |                  0
 /sys/fs/fuse/connections        |                  0 |                    0 |                  0
 /snap/core20/1828               |    0.0618896484375 |      0.0618896484375 |                  0
 /boot/efi                       | 0.4990081787109375 |   3.814697265625e-06 | 0.4990043640136719
 /snap/core22/583                |       0.0712890625 |         0.0712890625 |                  0
 /snap/go/10073                  |    0.0882568359375 |      0.0882568359375 |                  0
 /snap/core22/607                |       0.0712890625 |         0.0712890625 |                  0
 /snap/go/10135                  |    0.0882568359375 |      0.0882568359375 |                  0
 /snap/core20/1852               |    0.0618896484375 |      0.0618896484375 |                  0
 /snap/snapd/18596               |    0.0487060546875 |      0.0487060546875 |                  0
 /proc/sys/fs/binfmt_misc        |                  0 |                    0 |                  0
 /run/user/1000                  | 1.5638351440429688 |                    0 | 1.5638351440429688
 /vagrant                        |   931.546989440918 |    895.4629287719727 |  36.08406066894531
 /vagrant_data                   |   931.546989440918 |    895.4629287719727 |  36.08406066894531
 /data                           |  97.92864227294922 |    39.77399826049805 |  58.15464401245117
 /snap/snapd/18933               |     0.052001953125 |       0.052001953125 |                  0
(48 rows)

postgres=# select * from pg_raminfo;
   total_memory    |    used_memeory    |     free_memeory      
-------------------+--------------------+-----------------------
 15.63836669921875 | 11.454788208007812 | 6.95255286215716e-310
(1 row)

postgres=# select * from pg_cpuinfo;
  cpu   | num_cpu | num_core 
--------+---------+----------
 CPU-1 |       4 |        4
(1 row)
```

```sql
postgres=# select * from pg_suggested_settings;
  shared_buffer  |     work_mem      | maintance_workmem 
-----------------+-------------------+-------------------
 6.2553466796875 | 0.312767333984375 |  0.62553466796875
(1 row)
```

