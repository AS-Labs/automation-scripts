#include <bcc/proto.h>
#include <linux/sched.>

/*
 * Here we will choose 4 tracepoints
 * */

struct route_evt_t {
    char comm[TASK_COMM_LEN];
};


BPF_PERF_OUTPUT(route_evt);

static inline int do_trace(void* ctx, struct sk_buff* skb) {
    struct route_evt_t evt = {};
    bpf_get_current_comm(evt.comm, TASK_COMM_LEN);

    route_evt.perf_submit(ctx, &evt, sizeof(evt));

    return 0;
}


TRACEPOINT_PROBE(net, netif_rx) {
    return do_trace(args, (struct sk_buff*)args->skbaddr);
}
TRACEPOINT_PROBE(net, net_dev_queue) {
    return do_trace(args, (struct sk_buff*)args->skbaddr);
}
TRACEPOINT_PROBE(net, napi_gro_receive_entry) {
    return do_trace(args, (struct sk_buff*)args->skbaddr);
}
TRACEPOINT_PROBE(net, netif_receive_skb_entry) {
    return do_trace(args, (struct sk_buff*)args->skbaddr);
}

/*
 * Attaching the 4 tracepoints of the net category, loading the skbaddr field
 * and pass it to the common section which only loads the program name for now.
 * */
