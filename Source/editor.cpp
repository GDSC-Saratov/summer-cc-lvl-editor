#include "editor.h"

using json = nlohmann::json;



void export_lvl(plat::Storage lvl) {
    json json_lvl;
    json_lvl["entities"] = {};

    for (auto& entity : lvl.entities)
    {
        json ent_cmp = {};
        json name_cmp;
        name_cmp["type"] = "Info";
        name_cmp["name"] = entity.name;

        ent_cmp.push_back(name_cmp);
        
        for (auto& component : entity.components)
        {
            json cmp;
            if (component->get_component_type() == plat::Transform().get_component_type()) {
                
                cmp["type"] = "Transform";
                Vector3 pos = entity.getComponent<plat::Transform>()->pos;
                cmp["pos"] = {pos.x, pos.y, pos.z};
                
                cmp["angle"] = entity.getComponent<plat::Transform>()->angle;
                
                Vector2 scale = entity.getComponent<plat::Transform>()->scale;
                cmp["scale"] = {scale.x, scale.y};
            }
            else if (component->get_component_type() == plat::Sprite().get_component_type()) {
                cmp["type"] = "Sprite";
                cmp["path"] = entity.getComponent<plat::Sprite>()->path;
            }
            else if (component->get_component_type() == plat::Camera().get_component_type()) {
                cmp["type"] = "Camera";
                Vector2 scale = entity.getComponent<plat::Camera>()->scale;
                cmp["scale"] = {scale.x, scale.y};
            }
            else if (component->get_component_type() == plat::World().get_component_type()) {
                cmp["type"] = "World";
                b2Vec2 gravity = entity.getComponent<plat::World>()->cur_world->GetGravity();
                cmp["gravity"] = { gravity.x, gravity.y };
                cmp["Velocity"] = 6;
                cmp["Position"] = 2;
                cmp["timeStep"] = entity.getComponent<plat::World>()->time_settings.dt;
            }
            else if (component->get_component_type() == plat::Physics().get_component_type()) {
                cmp["type"] = "Physics";
                
                if (entity.getComponent<plat::Physics>()->bodyDef.type == b2_dynamicBody){
                    cmp["body"] = "dynamic";
                }
                else {
                    cmp["body"] = "static";
                }
                
                Rectangle rect = entity.getComponent<plat::Physics>()->collider;
                cmp["collider"] = {rect.x, rect.y, rect.width, rect.height};
            }
            else if (component->get_component_type() == plat::Player_control().get_component_type())
            {
                cmp["type"] = "PlayerControl";
                cmp["speed"] = entity.getComponent<plat::Player_control>()->speed;

                
            }
            ent_cmp.push_back(cmp);
        }
        json_lvl["entities"].push_back(ent_cmp);
    }

    std::ofstream lvl_file(lvl.lvl_name);
    json_lvl >> lvl_file;
}

void read_keyboard(std::string& field) {
    int key = GetCharPressed();

    while (key > 0)
    {
        if ((key >= 32) && (key <= 125))
        {
            field += (char)key;
        }

        key = GetCharPressed();  // Check next character in the queue
    }

    if (IsKeyPressed(KEY_BACKSPACE))
    {
        if (field.length() > 0)
            field = field.substr(0,field.length() - 1);
    }
    else if (IsKeyPressed(KEY_F1))
    {
        field = "Assets/Scenes/";
    }

}



std::string list_entities(plat::Storage storage, int current_ent_id) {
    std::string ls_ent;

    for (auto& entity : storage.entities)
    {
        if (entity.id < 5 + current_ent_id && entity.id > current_ent_id - 1  ) {

            ls_ent += std::to_string(entity.id) + " $ " + entity.name;

            if (entity.id == current_ent_id) {
                ls_ent += "    <-----";
            }

            ls_ent += "\n";

            for (auto& component : entity.components)
            {
                ls_ent += "* - " + component->get_component_type();
                ls_ent += "\n";
            }
        }
    }

    return ls_ent;
}

