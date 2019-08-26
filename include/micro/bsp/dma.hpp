#pragma once

#include <micro/utils/types.hpp>

namespace micro {

struct dma_handle_t { void *ptr; }; // DMA handle type - underlying instance type is bsp library dependent.
struct dma_t { void *ptr; };        // DMA instance identifier type.

/* @brief Structure storing DMA handle and instance.
 **/
struct dma_struct {
    dma_t instance;            // Pointer to the DMA instance.
    dma_handle_t handle;       // Pointer to the DMA handle.
};

} // namespace micro
