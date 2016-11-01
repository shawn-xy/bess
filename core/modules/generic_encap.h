#ifndef __GENERIC_ENCAP_H__
#define __GENERIC_ENCAP_H__

#include "../module.h"

#define MAX_FIELDS 8
#define MAX_FIELD_SIZE 8

struct Field {
  uint64_t value; /* onlt for constant values */
  int attr_id;    /* -1 for constant values */
  int pos;        /* relative position in the new header */
  int size;       /* in bytes. 1 <= size <= MAX_FIELD_SIZE */
};

class GenericEncap : public Module {
 public:
  static const gate_idx_t kNumIGates = 1;
  static const gate_idx_t kNumOGates = 1;

  GenericEncap() : Module(), encap_size_(), num_fields_(), fields_() {}

  struct snobj *Init(struct snobj *arg);
  pb_error_t Init(const bess::protobuf::GenericEncapArg &arg);

  void ProcessBatch(struct pkt_batch *batch);

  static const Commands<Module> cmds;

 private:
  struct snobj *AddFieldOne(struct snobj *field, struct Field *f, int idx);
  pb_error_t AddFieldOne(const bess::protobuf::GenericEncapArg_Field &field,
                         struct Field *f, int idx);

  int encap_size_;

  int num_fields_;

  struct Field fields_[MAX_FIELDS];
};

#endif