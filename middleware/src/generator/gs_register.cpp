#include "middleware/generator/gs_register.h"

#include "cpgf/metadefine.h"
#include "middleware/gscheme/graph/gs_metadata.h"
#include "middleware/generator/texture/chess_cubes.h"
#include "middleware/gscheme/graph/gs_types_convert_storage.h"



REFLECTION_IMPL(gs::GeneratorFuncs) {
    using namespace gs;
    using namespace cpgf;

    GDefineMetaClass<ChessCubes>
        ::define("ChessCube")
            ._annotation(MetaNames::CLASS)._element(MetaNames::PRETTY_NAME, "Noise chess cubes")
        ._constructor<void * ()>()
        ._property("Result", &ChessCubes::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::PRETTY_NAME, "Result")
        ._property("Size", &ChessCubes::GetSize, &ChessCubes::SetSize)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::EMBEDDED)._element(MetaNames::PRETTY_NAME, "Size")
    ;
}
