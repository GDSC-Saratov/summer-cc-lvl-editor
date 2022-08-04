#include "components.h"
#include <raylib-ext.hpp>
#include "json_loader.h"

void read_keyboard(std::string& field);

std::string list_entities(plat::Storage storage, int current_ent_id);

void export_lvl(plat::Storage lvl);