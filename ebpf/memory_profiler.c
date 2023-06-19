#include <linux/bpf.h>
#include <linux/ptrace.h>
#include <linux/sched.h>
#include <linux/utsname.h>

struct bpf_map_def SEC("maps") alloc_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(u64),
    .value_size = sizeof(u64),
    .max_entries = 10240,
};

SEC("kprobe/kmalloc")
int trace_kmalloc(struct pt_regs *ctx)
{
    u64 addr = PT_REGS_RC(ctx);
    u64 zero = 0;

    bpf_map_update_elem(&alloc_map, &addr, &zero, BPF_NOEXIST);
    return 0;
}

SEC("kprobe/kfree")
int trace_kfree(struct pt_regs *ctx)
{
    u64 addr = PT_REGS_PARM1(ctx);

    bpf_map_delete_elem(&alloc_map, &addr);
    return 0;
}

