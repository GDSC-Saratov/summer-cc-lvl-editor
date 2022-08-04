#include "json_loader.h"

using json = nlohmann::json;

plat::Storage
load_lvl(std::string path)
{
    plat::Storage new_lvl;
    std::ifstream lvl_file(path);
    
    if (!lvl_file.is_open())
        return default_lvl(path);
    
    json lvl_json;
    new_lvl.lvl_name = path;
    lvl_file >> lvl_json;

    for (auto &entity : lvl_json["entities"])
    {
        new_lvl.entities.push_back(plat::Entity());
        for (auto &component : entity)
        {
            if (component["type"] == "Info")
            {
                new_lvl.entities.back().name = component["name"];
            }
            else if (component["type"] == "World")
            {
                b2Vec2 gravity(component["gravity"][0], component["gravity"][1]);
                float timestep = component["timeStep"];
                int32 velocityIterations = component["Velocity"];
                int32 positionIterations = component ["Position"];
                plat::World *Cur_World = new plat::World(
                    gravity,
                    timestep,
                    velocityIterations,
                    positionIterations);
               
                new_lvl.entities.back().components.push_back(Cur_World);
                new_lvl.cur_world = new_lvl.entities.size() - 1;
            }
            else if (component["type"] == "Transform")
            {
                plat::Transform *transform = new plat::Transform();
                transform->angle = component["angle"];
                transform->pos = Vector3 {
                    component["pos"][0],
                    component["pos"][1],
                    component["pos"][2]
                };
                transform->scale = Vector2 {
                    component["scale"][0],
                    component["scale"][1]
                };
                new_lvl.entities.back().components.push_back(transform);
            }
            else if (component["type"] == "PlayerControl")
            {
                plat::Player_control *player_control = new plat::Player_control();
                player_control->speed = component["speed"];
                new_lvl.entities.back().components.push_back(player_control);
            }
            else if (component["type"] == "Sprite")
            {
                new_lvl.entities.back().components.push_back(
                    new plat::Sprite(std::string(component["path"]))
                );
            }
            else if (component["type"] == "Physics")
            {
                plat::Transform *trans = new_lvl.entities.back().getComponent<plat::Transform>();
                plat::Physics *cur_phys = new plat::Physics();

                cur_phys->collider = Rectangle {
                    component["collider"][0],
                    component["collider"][1],
                    component["collider"][2],
                    component["collider"][3]
                };

                cur_phys->bodyDef.position.Set(
                    trans->pos.x,
                    trans->pos.y
                );

                b2PolygonShape *shape = new b2PolygonShape();
                
                shape->SetAsBox(
                    cur_phys->collider.width * trans->scale.x / 2.f,
                    cur_phys->collider.height * trans->scale.y / 2.f
                );

                if (component["body"] == "dynamic")
                {
                    cur_phys->bodyDef.type = b2_dynamicBody;
                    cur_phys->body = new_lvl.entities[new_lvl.cur_world].getComponent<plat::World>()->cur_world->CreateBody(&cur_phys->bodyDef);

                    b2FixtureDef *fixtureDef = new b2FixtureDef();
                    fixtureDef->shape = shape;
                    fixtureDef->density = 2.0f;
                    fixtureDef->friction = 0.3f;
                    cur_phys->body->CreateFixture(fixtureDef);
                }
                else // static
                {
                    cur_phys->body = new_lvl.entities[new_lvl.cur_world].getComponent<plat::World>()->cur_world->CreateBody(&cur_phys->bodyDef);   
                    cur_phys->body->CreateFixture(shape, 0.0f);
                }
                cur_phys->body->SetFixedRotation(true);
                new_lvl.entities.back().components.push_back(cur_phys);
            }
            else if (component["type"] == "Camera")
            {
                plat::Camera *cam = new plat::Camera();
                cam->scale = Vector2 {
                    component["scale"][0],
                    component["scale"][1]
                };
                new_lvl.entities.back().components.push_back(cam);
                new_lvl.cur_camera = new_lvl.entities.size() - 1;
            }
        }
    }

    lvl_file.close();

    return new_lvl;
}

plat::Storage default_lvl(std::string path) {
    plat::Storage new_lvl = load_lvl("Assets/Scenes/base.json");
    new_lvl.lvl_name = path;
    return new_lvl;
}

//plat::Storage
//create_lvl(std::string path) {
//
//}