// User space
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <bpf/libbpf.h>

int main(int argc, char **argv) {
    struct rlimit r = {RLIM_INFINITY, RLIM_INFINITY};

    if (setrlimit(RLIMIT_MEMLOCK, &r)) {
        perror("setrlimit(RLIMIT_MEMLOCK)");
        return 1;
    }

    struct bpf_object *obj;
    int prog_fd;

    if (bpf_prog_load("program.o", BPF_PROG_TYPE_TRACEPOINT, &obj, &prog_fd)) {
        fprintf(stderr, "Failed to load program\n");
        return 1;
    }

    if (bpf_attach_trace(prog_fd, "kmem/kmalloc")) {
        fprintf(stderr, "Failed to attach tracepoint\n");
        return 1;
    }

    if (bpf_attach_trace(prog_fd, "kmem/kfree")) {
        fprintf(stderr, "Failed to attach tracepoint\n");
        return 1;
    }

    printf("eBPF program loaded and attached\n");

    sleep(10);  // Capture memory events for 10 seconds

    bpf_detach_trace(prog_fd, "kmem/kmalloc");
    bpf_detach_trace(prog_fd, "kmem/kfree");
    bpf_object__close(obj);

    return 0;
}

