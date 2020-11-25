#pragma once


namespace cpgf {
    class GVariant;
}

namespace gs {

using ConvertFunc = cpgf::GVariant (*)(const cpgf::GVariant&);

}
