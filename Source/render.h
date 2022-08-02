#pragma once
#include <raylib-ext.hpp>
#include "components.h"
#include<algorithm>

namespace plat {

struct Viewport
{
    float left, right;
    float bottom, top;
};

std::vector<Entity *>
create_draw_order(std::vector<Entity> &entities);

}
