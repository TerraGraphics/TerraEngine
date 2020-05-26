#include "middleware/gscheme/editor/gs_id.h"


uintptr_t GSGetNextID() {
    static uintptr_t nextId = 1;

    return nextId++;
}
