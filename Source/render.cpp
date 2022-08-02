#include "render.h"
#include "components.h"

namespace plat {

std::vector<Entity *>
create_draw_order(std::vector<Entity> &entities)
{
    std::vector<Entity *> draw_queue;
    for (int i = 0; i < entities.size(); ++i)
    {
        plat::Sprite *spr = entities[i].getComponent<Sprite>();
        if (spr)
        {
            draw_queue.push_back(&(entities[i]));
        }
    }

    std::sort(draw_queue.begin(), draw_queue.end(),
        [](Entity *a, Entity *b) {
            float az = a->getComponent<Transform>()->pos.z;
            float bz = b->getComponent<Transform>()->pos.z;
            return az < bz;
    });

    return draw_queue;
}

}