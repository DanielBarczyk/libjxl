#include "lib/jxl/enc_state_import.h"

#include "lib/jxl/base/status.h"
#include "lib/jxl/chroma_from_luma.h"
#include "lib/jxl/enc_cache.h"

namespace jxl {

Status SaveEncoderState(const char * filename, PassesEncoderState* enc_state) {
    JXL_DEBUG_V(2, "Exporting encoder state to file %s", filename);
    
    fprintf(stdout,
        "Beginning Encoder State debug.\nPasses: %lu\nhistograms: %lu %lu %lu %lu\n",
        enc_state->passes.size(),
        enc_state->histogram_idx.size(),
        enc_state->coeffs.size(),
        enc_state->special_frames.size(),
        enc_state->shared.coeff_orders.size()
    );

    FILE * export_fd;
    JXL_RETURN_IF_ERROR(export_fd = fopen(filename, "wb"));

    // saving coeffs
    int coeff_size = enc_state->coeffs.size() * sizeof(ACImage);
    fwrite(&coeff_size, sizeof(int), 1, export_fd);

    for(auto const& coeff : enc_state->coeffs) {
        fwrite(&coeff, sizeof(ACImage), 1, export_fd);
    }

    // saving shared
    //cmap
    fwrite(&enc_state->shared.cmap, sizeof(ColorCorrelationMap), 1, export_fd);

    fclose(export_fd);
    return true;
};

Status LoadEncoderState(const char * filename, PassesEncoderState* enc_state){
    JXL_DEBUG_V(2, "Importing encoder state from file %s", filename);

    FILE * import_fd;
    JXL_RETURN_IF_ERROR(import_fd = fopen(filename, "rb"));

    // reading coeffs
    int coeff_size;
    fread(&coeff_size, sizeof(int), 1, import_fd);
    if (enc_state->coeffs.size() == 0) {
        for (int i=0; i<coeff_size; i++) {
            std::unique_ptr<ACImageT<int32_t>> coeffs;
            fread(&coeffs, sizeof(ACImage), 1, import_fd);
            enc_state->coeffs.emplace_back(std::move(coeffs));
        }
        enc_state->coeffs.resize(0);
    }

    fclose(import_fd);
    return true;
};

}  // namespace jxl