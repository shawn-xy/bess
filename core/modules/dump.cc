#include <cmath>

#include <rte_hexdump.h>

#include "dump.h"

#define NS_PER_SEC 1000000000ul

static const uint64_t DEFAULT_INTERVAL_NS = 1 * NS_PER_SEC; /* 1 sec */

const Commands<Module> Dump::cmds = {
    {"set_interval", MODULE_FUNC &Dump::CommandSetInterval, 0},
};

struct snobj *Dump::Init(struct snobj *arg) {
  min_interval_ns_ = DEFAULT_INTERVAL_NS;
  next_ns_ = ctx.current_tsc();

  if (arg && (arg = snobj_eval(arg, "interval"))) {
    return CommandSetInterval(arg);
  } else {
    return nullptr;
  }
}

pb_error_t Dump::Init(const bess::protobuf::DumpArg &arg) {
  min_interval_ns_ = DEFAULT_INTERVAL_NS;
  next_ns_ = ctx.current_tsc();

  if (arg.interval() != 0.0) {
    return CommandSetInterval(arg);
  } else {
    return pb_errno(0);
  }
}

void Dump::ProcessBatch(struct pkt_batch *batch) {
  if (unlikely(ctx.current_ns() >= next_ns_)) {
    struct snbuf *pkt = batch->pkts[0];

    printf("----------------------------------------\n");
    printf("%s: packet dump\n", name().c_str());
    snb_dump(stdout, pkt);
    rte_hexdump(stdout, "Metadata buffer", pkt->_metadata, SNBUF_METADATA);
    next_ns_ = ctx.current_ns() + min_interval_ns_;
  }

  RunChooseModule(get_igate(), batch);
}

struct snobj *Dump::CommandSetInterval(struct snobj *arg) {
  double sec = snobj_number_get(arg);

  if (std::isnan(sec) || sec < 0.0) {
    return snobj_err(EINVAL, "invalid interval");
  }

  min_interval_ns_ = static_cast<uint64_t>(sec * NS_PER_SEC);

  return nullptr;
}

pb_error_t Dump::CommandSetInterval(const bess::protobuf::DumpArg &arg) {
  double sec = arg.interval();

  if (std::isnan(sec) || sec < 0.0) {
    return pb_error(EINVAL, "invalid interval");
  }

  min_interval_ns_ = static_cast<uint64_t>(sec * NS_PER_SEC);

  return pb_errno(0);
}

ADD_MODULE(Dump, "dump", "Dump packet data and metadata attributes")