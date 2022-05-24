#ifndef SYSCALL_H
#define SYSCALL_H
#include <stdint.h>
typedef enum Syscall Syscall;

enum Syscall{
    SYS_read,
    SYS_write,
    SYS_open,
    SYS_close,
    SYS_stat,
    SYS_fstat,
    SYS_lstat,
    SYS_poll,
    SYS_lseek,
    SYS_mmap,
    SYS_mprotect,
    SYS_munmap,
    SYS_brk,
    SYS_rt_sigaction,
    SYS_rt_sigprocmask,
    SYS_rt_sigreturn,
    SYS_ioctl,
    SYS_pread64,
    SYS_pwrite64,
    SYS_readv,
    SYS_writev,
    SYS_access,
    SYS_pipe,
    SYS_select,
    SYS_shed_yeld,
    SYS_mremap,
    SYS_msync,
    SYS_mincore,
    SYS_madvise,
    SYS_shmget,
    SYS_shmat,
    SYS_shmactl,
    SYS_dup,
    SYS_dup2,
    SYS_pause,
    SYS_nanosleep,
    SYS_getitimer,
    SYS_alarm,
    SYS_setitimer,
    SYS_getpid,
    SYS_sendfile,
    SYS_socket,
    SYS_connect,
    SYS_accept,
    SYS_sendto,
    SYS_recvfrom,
    SYS_sendmsg,
    SYS_recvmsg,
    SYS_shutdown,
    SYS_bind,
    SYS_listen,
    SYS_getsocketname,
    SYS_getpeername,
    SYS_socketpair,
    SYS_setsockopt,
    SYS_getsockopt,
    SYS_clone,
    SYS_fork,
    SYS_vfork,
    SYS_execve,
    SYS_exit,
    SYS_wait4,
    SYS_kill,
    SYS_uname,
    SYS_semget,
    SYS_semop,
    SYS_semctl,
    SYS_shmdt,
    SYS_msgget,
    SYS_msgsnd,
    SYS_msgrcv,
    SYS_msgctl,
    SYS_fcntl,
    SYS_flock,
    SYS_fsync,
    SYS_fdatasync,
    SYS_truncate,
    SYS_ftruncate,
    SYS_getdents,
    SYS_getcwd,
    SYS_chdir,
    SYS_fchdir,
    SYS_rename,
    SYS_mkdir,
    SYS_rmdir,
    SYS_creat,
    SYS_link,
    SYS_unlink,
    SYS_symlink,
    SYS_readlink,
    SYS_chmod,
    SYS_fchmod,
    SYS_chown,
    SYS_fchown,
    SYS_lchown,
    SYS_umask,
    SYS_gettimeofday,
    SYS_getrlimit,
    SYS_getrusage,
    SYS_sysinfo,
    SYS_times,
    SYS_ptrace,
    SYS_getuid,
    SYS_syslog,
    SYS_getgid,
    SYS_setuid,
    SYS_setgid,
    SYS_geteuid,
    SYS_getegid,
    SYS_setpgid,
    SYS_getppid,
    SYS_getpgrp,
    SYS_setsid,
    SYS_setreuid,
    SYS_setregid,
    SYS_getgroups,
    SYS_setgroups,
    SYS_setresuid,
    SYS_getresuid,
    SYS_setresgid,
    SYS_getresgid,
    SYS_getpgid,
    SYS_setfsuid,
    SYS_setfsgid,
    SYS_getsid,
    SYS_capget,
    SYS_capset,
    SYS_rt_sigpending,
    SYS_rt_sigtimedwait,
    SYS_rt_sigqueueinfo,
    SYS_rt_sigsuspend,
    SYS_sigaltstack,
    SYS_utime,
    SYS_mknod,
    SYS_uselib,
    SYS_personality,
    SYS_ustat,
    SYS_statfs,
    SYS_fstatfs,
    SYS_sysfs,
    SYS_getpriority,
    SYS_setpriority,
    SYS_sched_setparam,
    SYS_sched_getparam,
    SYS_sched_setscheduler,
    SYS_sched_getscheduler,
    SYS_sched_get_priority_max,
    SYS_sched_get_priority_min,
    SYS_sched_rr_get_interval,
    SYS_mlock,
    SYS_munlock,
    SYS_mlockall,
    SYS_munlockall,
    SYS_vhangup,
    SYS_modify_ldt,
    SYS_pivot_root,
    SYS_sysctl,
    SYS_prctl,
    SYS_arch_prctl,
    SYS_adjtimex,
    SYS_setrlimit,
    SYS_chroot,
    SYS_sync,
    SYS_acct,
    SYS_settimeofday,
    SYS_mount,
    SYS_umount2,
    SYS_swapon,
    SYS_swapoff,
    SYS_reboot,
    SYS_sethostname,
    SYS_setdomainname,
    SYS_iopl,
    SYS_ioperm,
    SYS_create_module,
    SYS_init_module,
    SYS_delete_module,
    SYS_get_kernel_syms,
    SYS_query_module,
    SYS_quotactl,
    SYS_nfsservctl,
    SYS_getpmsg,
    SYS_putpmsg,
    SYS_afs_syscall,
    SYS_tuxcall,
    SYS_security,
    SYS_gettid,
    SYS_readahead,
    SYS_setxattr,
    SYS_lsetxattr,
    SYS_fsetxattr,
    SYS_getxattr,
    SYS_lgetxattr,
    SYS_fgetxattr,
    SYS_listxattr,
    SYS_llistxattr,
    SYS_flistxattr,
    SYS_removexattr,
    SYS_lremovexattr,
    SYS_fremovexattr,
    SYS_tkill,
    SYS_time,
    SYS_futex,
    SYS_sched_setaffinity,
    SYS_sched_getaffinity,
    SYS_set_thread_area,
    SYS_io_setup,
    SYS_io_destroy,
    SYS_io_getevents,
    SYS_io_submit,
    SYS_io_cancel,
    SYS_get_thread_area,
    SYS_lookup_dcookie,
    SYS_epoll_create,
    SYS_epoll_ctl_old,
    SYS_epoll_wait_old,
    SYS_remap_file_pages,
    SYS_getdents64,
    SYS_set_tid_address,
    SYS_restart_syscall,
    SYS_semtimedop,
    SYS_fadvise64,
    SYS_timer_create,
    SYS_timer_settime,
    SYS_timer_gettime,
    SYS_timer_getoverrun,
    SYS_timer_delete,
    SYS_clock_settime,
    SYS_clock_gettime,
    SYS_clock_getres,
    SYS_clock_nanosleep,
    SYS_exit_group,
    SYS_epoll_wait,
    SYS_epoll_ctl,
    SYS_tgkill,
    SYS_utimes,
    SYS_vserver,
    SYS_mbind,
    SYS_set_mempolicy,
    SYS_get_mempolicy,
    SYS_mq_open,
    SYS_mq_unlink,
    SYS_mq_timedsend,
    SYS_mq_timedreceive,
    SYS_mq_notify,
    SYS_mq_getsetattr,
    SYS_kexec_load,
    SYS_waitid,
    SYS_add_key,
    SYS_request_key,
    SYS_keyctl,
    SYS_ioprio_set,
    SYS_ioprio_get,
    SYS_inotify_init,
    SYS_inotify_add_watch,
    SYS_inotify_rm_watch,
    SYS_migrate_pages,
    SYS_openat,
    SYS_mkdirat,
    SYS_mknodat,
    SYS_fchownat,
    SYS_futimesat,
    SYS_newfstatat,
    SYS_unlinkat,
    SYS_renameat,
    SYS_linkat,
    SYS_symlinkat,
    SYS_readlinkat,
    SYS_fchmodat,
    SYS_faccessat,
    SYS_pselect6,
    SYS_ppoll,
    SYS_unshare,
    SYS_set_robust_list,
    SYS_get_robust_list,
    SYS_splice,
    SYS_tee,
    SYS_sync_file_range,
    SYS_vmsplice,
    SYS_move_pages,
    SYS_utimensat,
    SYS_epoll_pwait,
    SYS_signalfd,
    SYS_timerfd_create,
    SYS_eventfd,
    SYS_fallocate,
    SYS_timerfd_settime,
    SYS_timerfd_gettime,
    SYS_accept4,
    SYS_signalfd4,
    SYS_eventfd2,
    SYS_epoll_create1,
    SYS_dup3,
    SYS_pipe2,
    SYS_inotify_init1,
    SYS_preadv,
    SYS_pwritev,
    SYS_rt_tgsigqueueinfo,
    SYS_perf_event_open,
    SYS_recvmmsg,
    SYS_fanotify_init,
    SYS_fanotify_mark,
    SYS_prlimit64,
    SYS_name_to_handle_at,
    SYS_open_by_handle_at,
    SYS_clock_adjtime,
    SYS_syncfs,
    SYS_sendmmsg,
    SYS_setns,
    SYS_getcpu,
    SYS_process_vm_readv,
    SYS_process_vm_writev,
    SYS_kcmp,
    SYS_finit_module,
    SYSCALL_COUNT
};

static const char const * const syscalls[] = {
"sys_read",
"sys_write",
"sys_open",
"sys_close",
"sys_stat",
"sys_fstat",
"sys_lstat",
"sys_poll",
"sys_lseek",
"sys_mmap",
"sys_mprotect",
"sys_munmap",
"sys_brk",
"sys_rt_sigaction",
"sys_rt_sigprocmask",
"sys_rt_sigreturn",
"sys_ioctl",
"sys_pread64",
"sys_pwrite64",
"sys_readv",
"sys_writev",
"sys_access",
"sys_pipe",
"sys_select",
"sys_sched_yield",
"sys_mremap",
"sys_msync",
"sys_mincore",
"sys_madvise",
"sys_shmget",
"sys_shmat",
"sys_shmctl",
"sys_dup",
"sys_dup2",
"sys_pause",
"sys_nanosleep",
"sys_getitimer",
"sys_alarm",
"sys_setitimer",
"sys_getpid",
"sys_sendfile",
"sys_socket",
"sys_connect",
"sys_accept",
"sys_sendto",
"sys_recvfrom",
"sys_sendmsg",
"sys_recvmsg",
"sys_shutdown",
"sys_bind",
"sys_listen",
"sys_getsockname",
"sys_getpeername",
"sys_socketpair",
"sys_setsockopt",
"sys_getsockopt",
"sys_clone",
"sys_fork",
"sys_vfork",
"sys_execve",
"sys_exit",
"sys_wait4",
"sys_kill",
"sys_uname",
"sys_semget",
"sys_semop",
"sys_semctl",
"sys_shmdt",
"sys_msgget",
"sys_msgsnd",
"sys_msgrcv",
"sys_msgctl",
"sys_fcntl",
"sys_flock",
"sys_fsync",
"sys_fdatasync",
"sys_truncate",
"sys_ftruncate",
"sys_getdents",
"sys_getcwd",
"sys_chdir",
"sys_fchdir",
"sys_rename",
"sys_mkdir",
"sys_rmdir",
"sys_creat",
"sys_link",
"sys_unlink",
"sys_symlink",
"sys_readlink",
"sys_chmod",
"sys_fchmod",
"sys_chown",
"sys_fchown",
"sys_lchown",
"sys_umask",
"sys_gettimeofday",
"sys_getrlimit",
"sys_getrusage",
"sys_sysinfo",
"sys_times",
"sys_ptrace",
"sys_getuid",
"sys_syslog",
"sys_getgid",
"sys_setuid",
"sys_setgid",
"sys_geteuid",
"sys_getegid",
"sys_setpgid",
"sys_getppid",
"sys_getpgrp",
"sys_setsid",
"sys_setreuid",
"sys_setregid",
"sys_getgroups",
"sys_setgroups",
"sys_setresuid",
"sys_getresuid",
"sys_setresgid",
"sys_getresgid",
"sys_getpgid",
"sys_setfsuid",
"sys_setfsgid",
"sys_getsid",
"sys_capget",
"sys_capset",
"sys_rt_sigpending",
"sys_rt_sigtimedwait",
"sys_rt_sigqueueinfo",
"sys_rt_sigsuspend",
"sys_sigaltstack",
"sys_utime",
"sys_mknod",
"sys_uselib",
"sys_personality",
"sys_ustat",
"sys_statfs",
"sys_fstatfs",
"sys_sysfs",
"sys_getpriority",
"sys_setpriority",
"sys_sched_setparam",
"sys_sched_getparam",
"sys_sched_setscheduler",
"sys_sched_getscheduler",
"sys_sched_get_priority_max",
"sys_sched_get_priority_min",
"sys_sched_rr_get_interval",
"sys_mlock",
"sys_munlock",
"sys_mlockall",
"sys_munlockall",
"sys_vhangup",
"sys_modify_ldt",
"sys_pivot_root",
"sys_sysctl",
"sys_prctl",
"sys_arch_prctl",
"sys_adjtimex",
"sys_setrlimit",
"sys_chroot",
"sys_sync",
"sys_acct",
"sys_settimeofday",
"sys_mount",
"sys_umount2",
"sys_swapon",
"sys_swapoff",
"sys_reboot",
"sys_sethostname",
"sys_setdomainname",
"sys_iopl",
"sys_ioperm",
"sys_create_module",
"sys_init_module",
"sys_delete_module",
"sys_get_kernel_syms",
"sys_query_module",
"sys_quotactl",
"sys_nfsservctl",
"sys_getpmsg",
"sys_putpmsg",
"sys_afs_syscall",
"sys_tuxcall",
"sys_security",
"sys_gettid",
"sys_readahead",
"sys_setxattr",
"sys_lsetxattr",
"sys_fsetxattr",
"sys_getxattr",
"sys_lgetxattr",
"sys_fgetxattr",
"sys_listxattr",
"sys_llistxattr",
"sys_flistxattr",
"sys_removexattr",
"sys_lremovexattr",
"sys_fremovexattr",
"sys_tkill",
"sys_time",
"sys_futex",
"sys_sched_setaffinity",
"sys_sched_getaffinity",
"sys_set_thread_area",
"sys_io_setup",
"sys_io_destroy",
"sys_io_getevents",
"sys_io_submit",
"sys_io_cancel",
"sys_get_thread_area",
"sys_lookup_dcookie",
"sys_epoll_create",
"sys_epoll_ctl_old",
"sys_epoll_wait_old",
"sys_remap_file_pages",
"sys_getdents64",
"sys_set_tid_address",
"sys_restart_syscall",
"sys_semtimedop",
"sys_fadvise64",
"sys_timer_create",
"sys_timer_settime",
"sys_timer_gettime",
"sys_timer_getoverrun",
"sys_timer_delete",
"sys_clock_settime",
"sys_clock_gettime",
"sys_clock_getres",
"sys_clock_nanosleep",
"sys_exit_group",
"sys_epoll_wait",
"sys_epoll_ctl",
"sys_tgkill",
"sys_utimes",
"sys_vserver",
"sys_mbind",
"sys_set_mempolicy",
"sys_get_mempolicy",
"sys_mq_open",
"sys_mq_unlink",
"sys_mq_timedsend",
"sys_mq_timedreceive",
"sys_mq_notify",
"sys_mq_getsetattr",
"sys_kexec_load",
"sys_waitid",
"sys_add_key",
"sys_request_key",
"sys_keyctl",
"sys_ioprio_set",
"sys_ioprio_get",
"sys_inotify_init",
"sys_inotify_add_watch",
"sys_inotify_rm_watch",
"sys_migrate_pages",
"sys_openat",
"sys_mkdirat",
"sys_mknodat",
"sys_fchownat",
"sys_futimesat",
"sys_newfstatat",
"sys_unlinkat",
"sys_renameat",
"sys_linkat",
"sys_symlinkat",
"sys_readlinkat",
"sys_fchmodat",
"sys_faccessat",
"sys_pselect6",
"sys_ppoll",
"sys_unshare",
"sys_set_robust_list",
"sys_get_robust_list",
"sys_splice",
"sys_tee",
"sys_sync_file_range",
"sys_vmsplice",
"sys_move_pages",
"sys_utimensat",
"sys_epoll_pwait",
"sys_signalfd",
"sys_timerfd_create",
"sys_eventfd",
"sys_fallocate",
"sys_timerfd_settime",
"sys_timerfd_gettime",
"sys_accept4",
"sys_signalfd4",
"sys_eventfd2",
"sys_epoll_create1",
"sys_dup3",
"sys_pipe2",
"sys_inotify_init1",
"sys_preadv",
"sys_pwritev",
"sys_rt_tgsigqueueinfo",
"sys_perf_event_open",
"sys_recvmmsg",
"sys_fanotify_init",
"sys_fanotify_mark",
"sys_prlimit64",
"sys_name_to_handle_at",
"sys_open_by_handle_at",
"sys_clock_adjtime",
"sys_syncfs",
"sys_sendmmsg",
"sys_setns",
"sys_getcpu",
"sys_process_vm_readv",
"sys_process_vm_writev",
"sys_kcmp",
"sys_finit_module",
};

static const uint16_t const number_of_syscall_args[] = {
    3,  //syscall[0]
    3,  //syscall[1]
    3,  //syscall[2]
    1,  //syscall[3]
    2,  //syscall[4]
    2,  //syscall[5]
    2,  //syscall[6]
    3,  //syscall[7]
    3,  //syscall[8]
    6,  //syscall[9]
    3,  //syscall[10]
    2,  //syscall[11]
    1,  //syscall[12]
    4,  //syscall[13]
    4,  //syscall[14]
    0,  //syscall[15]
    3,  //syscall[16]
    4,  //syscall[17]
    4,  //syscall[18]
    3,  //syscall[19]
    3,  //syscall[20]
    2,  //syscall[21]
    1,  //syscall[22]
    5,  //syscall[23]
    0,  //syscall[24]
    5,  //syscall[25]
    3,  //syscall[26]
    3,  //syscall[27]
    3,  //syscall[28]
    3,  //syscall[29]
    3,  //syscall[30]
    3,  //syscall[31]
    1,  //syscall[32]
    2,  //syscall[33]
    0,  //syscall[34]
    2,  //syscall[35]
    2,  //syscall[36]
    1,  //syscall[37]
    3,  //syscall[38]
    0,  //syscall[39]
    4,  //syscall[40]
    3,  //syscall[41]
    3,  //syscall[42]
    3,  //syscall[43]
    6,  //syscall[44]
    6,  //syscall[45]
    3,  //syscall[46]
    3,  //syscall[47]
    2,  //syscall[48]
    3,  //syscall[49]
    2,  //syscall[50]
    3,  //syscall[51]
    3,  //syscall[52]
    4,  //syscall[53]
    5,  //syscall[54]
    5,  //syscall[55]
    5,  //syscall[56]
    0,  //syscall[57]
    0,  //syscall[58]
    3,  //syscall[59]
    1,  //syscall[60]
    4,  //syscall[61]
    2,  //syscall[62]
    1,  //syscall[63]
    3,  //syscall[64]
    3,  //syscall[65]
    4,  //syscall[66]
    1,  //syscall[67]
    2,  //syscall[68]
    4,  //syscall[69]
    5,  //syscall[70]
    3,  //syscall[71]
    3,  //syscall[72]
    2,  //syscall[73]
    1,  //syscall[74]
    1,  //syscall[75]
    2,  //syscall[76]
    2,  //syscall[77]
    3,  //syscall[78]
    2,  //syscall[79]
    1,  //syscall[80]
    1,  //syscall[81]
    2,  //syscall[82]
    2,  //syscall[83]
    1,  //syscall[84]
    2,  //syscall[85]
    2,  //syscall[86]
    1,  //syscall[87]
    2,  //syscall[88]
    3,  //syscall[89]
    2,  //syscall[90]
    2,  //syscall[91]
    3,  //syscall[92]
    3,  //syscall[93]
    3,  //syscall[94]
    1,  //syscall[95]
    2,  //syscall[96]
    2,  //syscall[97]
    2,  //syscall[98]
    1,  //syscall[99]
    1,  //syscall[100]
    4,  //syscall[101]
    0,  //syscall[102]
    3,  //syscall[103]
    0,  //syscall[104]
    1,  //syscall[105]
    1,  //syscall[106]
    0,  //syscall[107]
    0,  //syscall[108]
    2,  //syscall[109]
    0,  //syscall[110]
    0,  //syscall[111]
    0,  //syscall[112]
    2,  //syscall[113]
    2,  //syscall[114]
    2,  //syscall[115]
    2,  //syscall[116]
    3,  //syscall[117]
    3,  //syscall[118]
    3,  //syscall[119]
    3,  //syscall[120]
    1,  //syscall[121]
    1,  //syscall[122]
    1,  //syscall[123]
    1,  //syscall[124]
    2,  //syscall[125]
    2,  //syscall[126]
    2,  //syscall[127]
    4,  //syscall[128]
    3,  //syscall[129]
    2,  //syscall[130]
    2,  //syscall[131]
    2,  //syscall[132]
    3,  //syscall[133]
    1,  //syscall[134]
    1,  //syscall[135]
    2,  //syscall[136]
    2,  //syscall[137]
    2,  //syscall[138]
    3,  //syscall[139]
    2,  //syscall[140]
    3,  //syscall[141]
    2,  //syscall[142]
    2,  //syscall[143]
    3,  //syscall[144]
    1,  //syscall[145]
    1,  //syscall[146]
    1,  //syscall[147]
    2,  //syscall[148]
    2,  //syscall[149]
    2,  //syscall[150]
    1,  //syscall[151]
    0,  //syscall[152]
    0,  //syscall[153]
    3,  //syscall[154]
    2,  //syscall[155]
    1,  //syscall[156]
    5,  //syscall[157]
    3,  //syscall[158]
};

#endif /* SYSCALL_H */