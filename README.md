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

## Examples

``` 

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
