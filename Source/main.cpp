#define RAYEXT_IMPLEMENTATION
#include <raylib-ext.hpp>
#include <iostream>
#include <algorithm>
#include "json_loader.h"
#include "render.h"
#include "editor.h"

bool mod = true;
bool insert_mod = false;
std::string path_to_lvl = "Assets/Scenes/";

int current_ent_id = -1;
std::string input_field = "";

int
main()
{
    const Vector2 screen_size { 720, 480 };
    InitWindow(screen_size.x, screen_size.y, "Creative Coding: Platformer");
    SetTargetFPS(60);
    Font fontTtf = LoadFontEx("Assets/Fonts/monogram.ttf", 32, 0, 104);
    plat::Storage storage;
    std::vector<plat::Entity *> draw_queue;
    plat::Camera* cam;
    plat::Transform* cam_t;

    while (!WindowShouldClose())
    {
        /*for (auto &entity : storage.entities)
        {
            for (auto &component : entity.components)
            {
                component->update(GetFrameTime(), entity.id, storage);
            }
        }*/

        

        BeginDrawing();
        ClearBackground(BLACK);
        
        if (mod) {
            read_keyboard(path_to_lvl);

            DrawRectangleV(Vector2{ 0,screen_size.y * 0.2f }, Vector2{ screen_size.x ,screen_size.y * 0.6f }, WHITE);
            DrawTextEx(fontTtf, "Enter path to lvl", screen_size * 0.3, (float)fontTtf.baseSize, 2, LIME);
            DrawTextEx(fontTtf, path_to_lvl, Vector2{ screen_size.x * 0.2f, screen_size.y * 0.5f }, (float)fontTtf.baseSize, 2, LIME);
            EndDrawing();
            if (IsKeyDown(KEY_ENTER)) {
                storage = load_lvl(path_to_lvl);
                draw_queue = create_draw_order(storage.entities);
                mod = false;

                cam = storage.entities[storage.cur_camera].getComponent<plat::Camera>();
                cam_t = storage.entities[storage.cur_camera].getComponent<plat::Transform>();

            }
            continue;
        }
        
        
        

        for (int i = 0; i < draw_queue.size(); ++i)
        {
            for (int j = 0; j < draw_queue[i]->components.size(); ++j)
            {
                plat::Sprite *spr = draw_queue[i]->getComponent<plat::Sprite>();
                if (spr)
                {
                    plat::Transform *t = draw_queue[i]->getComponent<plat::Transform>();
                    plat::Physics *ph = draw_queue[i]->getComponent<plat::Physics>();

                    Vector2 screen_pos = {
                        (t->pos.x - cam_t->pos.x) * cam->scale.x,
                        (cam_t->pos.y - t->pos.y) * cam->scale.y
                    };
                    screen_pos += screen_size * 0.5f;
                    int sprite_width = spr->image.width * t->scale.x * cam->scale.x;
                    int sprite_height = spr->image.height * t->scale.y * cam->scale.y;
                    screen_pos -= Vector2 {
                        (float) sprite_width,
                        (float) sprite_height
                    } * 0.5f;

                    if (sprite_width != spr->texture.width
                        || sprite_height != spr->texture.height)
                    {
                        UnloadTexture(spr->texture);
                        Image image = ImageCopy(spr->image);
                        ImageResizeNN(&image, sprite_width, sprite_height);
                        spr->texture = LoadTextureFromImage(image);
                    }

                    DrawTextureV(spr->texture, screen_pos, WHITE);
                    if (ph)
                    {
                        auto c = ph->body->GetFixtureList()->GetAABB(0).GetCenter();
                        auto a = ph->body->GetFixtureList()->GetAABB(0).lowerBound;
                        auto b = ph->body->GetFixtureList()->GetAABB(0).upperBound;
                        Vector2 screen_pos = {
                            (c.x - cam_t->pos.x) * cam->scale.x,
                            (cam_t->pos.y - c.y) * cam->scale.y
                        };
                        screen_pos += screen_size * 0.5f;
                        screen_pos -= Vector2 {
                            (float) b.x - a.x,
                            (float) b.y - a.y
                        } * 0.5f;
                        DrawRectangleLines(
                            screen_pos.x, screen_pos.y,
                            b.x - a.x + 1,
                            b.y - a.y + 1,
                            RED
                        );
                    }
                }
            }
        }
        
        if (insert_mod) {
            DrawRectangleV(Vector2{ 0,0 }, Vector2{ screen_size.x ,screen_size.y }, Color{ 100,100,100,100 });
            
            

            if (current_ent_id != -1) {
                DrawTextEx(fontTtf, "- > "+storage.entities[current_ent_id].name, Vector2{ 10,10 }, (float)fontTtf.baseSize, 2, GREEN);
                DrawTextEx(fontTtf, "enter - add new component " + input_field,
                    Vector2{ 10,10 + (float)fontTtf.baseSize * 2 },
                    (float)fontTtf.baseSize, 2, RED);
            }
            else {
                
                DrawTextEx(fontTtf, "enter - add new entity", Vector2{ 10,10 }, (float)fontTtf.baseSize, 2, GREEN);

                DrawTextEx(fontTtf, "type entitie name: " + input_field,
                           Vector2{ 10,10 + (float)fontTtf.baseSize * 2 },
                           (float)fontTtf.baseSize, 2, RED);

                read_keyboard(input_field);

                if (IsKeyPressed(KEY_ENTER)) {
                    storage.entities.push_back(plat::Entity());
                    storage.entities.back().id = storage.entities.size()-1;
                    storage.entities.back().name = input_field;
                    current_ent_id = storage.entities.back().id;
                }
            }

            if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Q)) {
                insert_mod = false;
            }
        }
        else {
            cam->update(GetFrameTime(), storage.cur_camera, storage);


            if (IsKeyDown(KEY_TAB)) {
                DrawRectangleV(Vector2{ 0,0 }, Vector2{ screen_size.x ,screen_size.y }, Color{ 100,100,100,100 });
                DrawTextEx(fontTtf, list_entities(storage, current_ent_id), Vector2{ 10,10 }, (float)fontTtf.baseSize*0.75, 2, RED);
            }
            else if (IsKeyPressed(KEY_UP)) {
                current_ent_id--;
            }
            else if (IsKeyPressed(KEY_DOWN)) {
                current_ent_id++;
            }
            else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_A)) {
                insert_mod = true;
                current_ent_id = -1;
                input_field = "";
            }
            else {
                DrawTextEx(fontTtf, "lvl-editor: " + storage.lvl_name, Vector2{ 10,10 }, (float)fontTtf.baseSize, 2, RED);
                DrawTextEx(fontTtf, "entities: " + std::to_string(storage.entities.size()), Vector2{ 10,40 }, (float)fontTtf.baseSize, 2, RED);
            }
        }
        EndDrawing();
    }
    
    CloseWindow();

    return 0;
}
