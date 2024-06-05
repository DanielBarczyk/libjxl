// Copyright (c) the JPEG XL Project Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef LIB_JXL_ENC_ANS_PARAMS_H_
#define LIB_JXL_ENC_ANS_PARAMS_H_

// Encoder-only parameter needed for ANS entropy encoding methods.

#include <stdint.h>
#include <stdlib.h>

#include <vector>

#include "lib/jxl/common.h"

namespace jxl {

// Forward declaration to break include cycle.
struct CompressParams;

// RebalanceHistogram requires a signed type.
using ANSHistBin = int32_t;

struct HistogramParams {
  enum class ClusteringType {
    kFastest,  // Only 4 clusters.
    kFast,
    kBest,
  };

  enum class HybridUintMethod {
    kNone,        // just use kHybridUint420Config.
    k000,         // force the fastest option.
    kFast,        // just try a couple of options.
    kContextMap,  // fast choice for ctx map.
    kBest,
  };

  enum class LZ77Method {
    kNone,     // do not try lz77.
    kRLE,      // only try doing RLE.
    kLZ77,     // try lz77 with backward references.
    kOptimal,  // optimal-matching LZ77 parsing.
  };

  enum class ANSHistogramStrategy {
    kFast,         // Only try some methods, early exit.
    kApproximate,  // Only try some methods.
    kPrecise,      // Try all methods.
  };

  HistogramParams() = default;

  HistogramParams(SpeedTier tier, size_t num_ctx) {
    if (tier > SpeedTier::kFalcon) {
      clustering = ClusteringType::kFastest;
      lz77_method = LZ77Method::kNone;
    } else if (tier > SpeedTier::kTortoise) {
      clustering = ClusteringType::kFast;
    } else {
      clustering = ClusteringType::kBest;
    }
    if (tier > SpeedTier::kTortoise) {
      uint_method = HybridUintMethod::kNone;
    }
    if (tier >= SpeedTier::kSquirrel) {
      ans_histogram_strategy = ANSHistogramStrategy::kApproximate;
    }
  }

  static HistogramParams ForModular(
      const CompressParams& cparams,
      const std::vector<uint8_t>& extra_dc_precision, bool streaming_mode);

  ClusteringType clustering = ClusteringType::kBest;
  HybridUintMethod uint_method = HybridUintMethod::kBest;
  LZ77Method lz77_method = LZ77Method::kRLE;
  ANSHistogramStrategy ans_histogram_strategy = ANSHistogramStrategy::kPrecise;
  std::vector<size_t> image_widths;
  size_t max_histograms = ~0;
  bool force_huffman = false;
  bool initialize_global_state = true;
  bool streaming_mode = false;
  bool add_missing_symbols = false;
  bool add_fixed_histograms = false;

  void Save(FILE* fd) {
    fwrite(&clustering, sizeof(ClusteringType), 1, fd);
    fwrite(&uint_method, sizeof(HybridUintMethod), 1, fd);
    fwrite(&lz77_method, sizeof(LZ77Method), 1, fd);
    fwrite(&ans_histogram_strategy, sizeof(ANSHistogramStrategy), 1, fd);
    int image_widths_size = image_widths.size();
    fwrite(&image_widths_size, sizeof(int), 1, fd);
    fwrite(&image_widths[0], sizeof(size_t), image_widths_size, fd);
    fwrite(&max_histograms, sizeof(size_t), 1, fd);
    fwrite(&force_huffman, sizeof(bool), 1, fd);
    fwrite(&initialize_global_state, sizeof(bool), 1, fd);
    fwrite(&streaming_mode, sizeof(bool), 1, fd);
    fwrite(&add_missing_symbols, sizeof(bool), 1, fd);
    fwrite(&add_fixed_histograms, sizeof(bool), 1, fd);
  }

  void Load(FILE* fd) {
    fread(&clustering, sizeof(ClusteringType), 1, fd);
    fread(&uint_method, sizeof(HybridUintMethod), 1, fd);
    fread(&lz77_method, sizeof(LZ77Method), 1, fd);
    fread(&ans_histogram_strategy, sizeof(ANSHistogramStrategy), 1, fd);
    int image_widths_size;
    fread(&image_widths_size, sizeof(int), 1, fd);
    image_widths.resize(image_widths_size);
    fread(&image_widths[0], sizeof(size_t), image_widths_size, fd);
    fread(&max_histograms, sizeof(size_t), 1, fd);
    fread(&force_huffman, sizeof(bool), 1, fd);
    fread(&initialize_global_state, sizeof(bool), 1, fd);
    fread(&streaming_mode, sizeof(bool), 1, fd);
    fread(&add_missing_symbols, sizeof(bool), 1, fd);
    fread(&add_fixed_histograms, sizeof(bool), 1, fd);
  }
};

}  // namespace jxl

#endif  // LIB_JXL_ENC_ANS_PARAMS_H_
