#define RAYEXT_IMPLEMENTATION
#include <raylib-ext.hpp>
#include <iostream>
#include <algorithm>
#include "json_loader.h"
#include "render.h"
#include "editor.h"

bool mod = true;
bool insert_mod = false;
bool grab_mod = false;
bool show_mod = false;
bool play_mod = false;
std::string path_to_lvl = "Assets/Scenes/";

int current_ent_id = 0;
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
        if (play_mod) {
         
            for (auto& entity : storage.entities)
            {
                for (auto& component : entity.components)
                {
                    component->update(GetFrameTime(), entity.id, storage);
                }
            }

        }
        

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
                    if (ph && play_mod)
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
                DrawTextEx(fontTtf, "enter - add new component " + input_field +
                    "1 Transform\n2 Physic\n3 Sprite\n4 Player controll\n",
                    Vector2{ 10,10 + (float)fontTtf.baseSize * 2 },
                    (float)fontTtf.baseSize, 2, RED);
                read_keyboard(input_field);
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
                    input_field = "";
                }
            }

            if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Q)) {
                insert_mod = false;
                current_ent_id = 0;
            }
        }
        else if (grab_mod) {
            cam->update(GetFrameTime(), storage.cur_camera, storage);
            
            DrawTextEx(fontTtf, "grab mod\n" + std::to_string(current_ent_id) + " entitie: " + storage.entities[current_ent_id].name,
                Vector2{ 10,10 }, (float)fontTtf.baseSize, 2, GREEN);

            storage.entities[current_ent_id].getComponent<plat::Transform>()->pos = 
               storage.entities[storage.cur_camera].getComponent<plat::Transform>()->pos;
            
           
            Vector3 cam_pos= storage.entities[current_ent_id].getComponent<plat::Transform>()->pos;
            plat::Physics* ph = storage.entities[current_ent_id].getComponent<plat::Physics>();
            
            if (ph) {
                ph->body->SetTransform(b2Vec2{ cam_pos.x, cam_pos.y }, 0);
            }
            
            if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Q)) {
                grab_mod = false;
            }
        }
        else if (show_mod) {
            DrawRectangleV(Vector2{ 0,0 }, Vector2{ screen_size.x ,screen_size.y }, Color{ 100,100,100,100 });
            DrawTextEx(fontTtf, list_entities(storage, current_ent_id), Vector2{ 10,10 }, (float)fontTtf.baseSize * 0.75, 2, WHITE);

            if (IsKeyPressed(KEY_UP)) {
                current_ent_id--;
                if (current_ent_id < 0) {
                    current_ent_id = 0;
                }
            }
            else if (IsKeyPressed(KEY_DOWN)) {
                current_ent_id++;
                if (current_ent_id > storage.entities.size() - 1) {
                    current_ent_id = storage.entities.size() - 1;
                }
            }

            if (IsKeyReleased(KEY_TAB)) {
                show_mod = false;
            }
        }
        else {
            


            if (IsKeyDown(KEY_TAB)) {
                show_mod = true;
            }
            else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_A)) {
                insert_mod = true;
                current_ent_id = -1;
                input_field = "";
            }
            /*else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_D)) {
                if (current_ent_id != -1) {
                    storage.entities.push_back(storage.entities[current_ent_id].copy());
                    storage.entities.back().id = storage.entities.size() - 1;
                } 
            }*/
            else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_G)) {
                if (current_ent_id != -1) {
                    grab_mod = true;
                    storage.entities[storage.cur_camera].getComponent<plat::Transform>()->pos =
                        storage.entities[current_ent_id].getComponent<plat::Transform>()->pos;
                }
            }
            else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_P)) {
                play_mod = true;
            }
            else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_E)) {
                play_mod = false;
            }
            else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_J) && !play_mod) {
                export_lvl(storage);
            }
            else {
                DrawTextEx(fontTtf, "Lvl-editor: " + storage.lvl_name + "\n" + 
                                    "Entities: " + std::to_string(storage.entities.size()) + "\n" +
                                    "Current entitie: " + std::to_string(current_ent_id) + " " + 
                                    storage.entities[current_ent_id].name + "\n" +  
                                    ((play_mod)?("Play mod"):("Editor mod")),
                                    Vector2{ 10,10 }, (float)fontTtf.baseSize, 2, GREEN);
                
            }

            cam->update(GetFrameTime(), storage.cur_camera, storage);
            
            
        }
        EndDrawing();
    }
    
    CloseWindow();

    return 0;
}
