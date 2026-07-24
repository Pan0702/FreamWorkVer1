#pragma once
#include <vector>
#include "navigation_span.h"

struct NavigationHeightfieldCell
{
    std::vector<NavigationSpan> spans;
};
