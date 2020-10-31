#include "middleware/generator/gs_register.h"

#include "cpgf/metadefine.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_metadata.h"
#include "middleware/generator/texture/spheres.h"
#include "middleware/generator/texture/cylinders.h"
#include "middleware/generator/texture/chess_cubes.h"
#include "middleware/gscheme/graph/gs_types_convert_storage.h"



REFLECTION_IMPL(gs::GeneratorFuncs) {
    using namespace gs;
    using namespace cpgf;

    GDefineMetaClass<ChessCubes>
        ::define("ChessCube")
            ._annotation(MetaNames::CLASS)._element(MetaNames::DISPLAY_NAME, "Noise chess cubes")
        ._constructor<void * ()>()
        ._property("Result", &ChessCubes::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::DISPLAY_NAME, "Result")
        ._property("Frequency", &ChessCubes::GetFrequency, &ChessCubes::SetFrequency)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::EMBEDDED)._element(MetaNames::DISPLAY_NAME, "Frequency")
    ;

    GDefineMetaClass<Spheres>
        ::define("Spheres")
            ._annotation(MetaNames::CLASS)._element(MetaNames::DISPLAY_NAME, "Noise spheres")
        ._constructor<void * ()>()
        ._property("Result", &Spheres::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::DISPLAY_NAME, "Result")
        ._property("Frequency", &Spheres::GetFrequency, &Spheres::SetFrequency)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::EMBEDDED)._element(MetaNames::DISPLAY_NAME, "Frequency")
    ;

    GDefineMetaClass<Cylinders>
        ::define("Cylinders")
            ._annotation(MetaNames::CLASS)._element(MetaNames::DISPLAY_NAME, "Noise cylinders")
        ._constructor<void * ()>()
        ._property("Result", &Cylinders::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::DISPLAY_NAME, "Result")
        ._property("Frequency", &Cylinders::GetFrequency, &Cylinders::SetFrequency)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::EMBEDDED)._element(MetaNames::DISPLAY_NAME, "Frequency")
    ;
}
