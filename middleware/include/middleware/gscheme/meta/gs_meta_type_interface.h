#pragma once

#include <string>


class IMetaElementaryType {
    virtual std::string ToString() const = 0;
    virtual bool FromString(const std::string& value) = 0;
};
