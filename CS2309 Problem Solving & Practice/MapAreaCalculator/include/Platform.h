#pragma once

#include <string>

namespace MapAreaCalculator
{
    std::string importImagePath();

    std::string importVerticesPath();

    std::string exportImagePath();

    std::string exportVerticesPath();

    void errorMessage(std::string_view message);

} // namespace MapAreaCalculator