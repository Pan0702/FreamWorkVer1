#pragma once
#include <vector>
#include "navigation_span.h"

/**
 * @brief ハイトフィールドの 1 セルが持つ、高さ方向のスパン列。
 */
struct NavigationHeightfieldCell
{
    std::vector<NavigationSpan> spans;
};
