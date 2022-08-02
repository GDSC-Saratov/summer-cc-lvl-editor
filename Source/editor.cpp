#include "editor.h"

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
        ls_ent += "- $ " + entity.name ;
        
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

    return ls_ent;
}