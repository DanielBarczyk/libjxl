#ifndef LIB_JXL_ENC_STATE_IMPORT_H_
#define LIB_JXL_ENC_STATE_IMPORT_H_

#include "lib/jxl/base/status.h"
#include "lib/jxl/enc_cache.h"

namespace jxl {

Status SaveEncoderState(const char * filename, PassesEncoderState* enc_state);

Status LoadEncoderState(const char * filename, PassesEncoderState* enc_state);

}  // namespace jxl

#endif


