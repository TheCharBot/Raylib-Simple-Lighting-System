#include "raylib.h"
/*

"_LIGHT_" is a basic implementation of a light object. _LIGHT_SOURCES_ would be an array / vector of _LIGHT_

for "*_LOC_" names, assume int type for uniform access

If any more context is necessary, contact me at charles@lpvondollen.family or on GitHub. I will try to update the repository

*/

#define MAX_LIGHTS 80

enum _LIGHT_FLICKER_TYPE_{
    NO_EFFECT,
    FLICKER,
    WAVE
};

struct _LIGHT_
{
    Vector2 pos;     // World position
    float radius;         // Pixels

    Color color;

    float intensity = 1.0f;
    float softness = 0.4f;

    bool active = true;

    int locate_id = 0;

    bool follow_player = false;

    _LIGHT_FLICKER_TYPE_ flicker_type = {};

    float flicker_strength = 1.0f;

    float brightness = 0.0f;

    // bool colored = false;
};

void init_shaders()
{
    _LIGHTING_TEX_ = LoadRenderTexture(320, 180);

    _LIGHTING_SHADER_ = LoadShader(0, "shaders/lighting.fs");

    Shader shader = _LIGHTING_SHADER_;

    _LIGHT_POSITIONS_LOC =
        GetShaderLocation(shader, "lightPositions");

    _LIGHT_RADII_LOC_ =
        GetShaderLocation(shader, "lightRadii");

    _LIGHT_COLORS_LOC_ =
        GetShaderLocation(shader, "lightColors");

    _LIGHT_INTENSITY_LOC_ =
        GetShaderLocation(shader, "lightIntensity");

    _LIGHT_SOFTNESS_LOC_ =
        GetShaderLocation(shader, "lightSoftness");

    _LIGHT_BRIGHTNESS_LOC_ =
        GetShaderLocation(shader, "lightBrightness");

    _LIGHT_COUNT_LOC_ =
        GetShaderLocation(shader, "lightCount");

    _ASPECT_LOC_ =
        GetShaderLocation(shader, "aspectRatio");

    _AMBIENT_LOC_ =
        GetShaderLocation(shader, "ambientDarkness");
}

void update_lighting()
{
    // draw_overlay;
    Vector2 positions[MAX_LIGHTS];
    float radii[MAX_LIGHTS];
    Vector3 colors[MAX_LIGHTS];
    float intensity[MAX_LIGHTS];
    float softness[MAX_LIGHTS];
    float brightness[MAX_LIGHTS];
    // int colored[MAX_LIGHTS];

    int lightCount = 0;

    for (const _LIGHT_& _LIGHT_ : _LIGHT_SOURCES_)
    {
        if (!light.active)
            continue;
        if(!CheckCollisionPointRec(light.pos, player.major_cull_rect) && !light.follow_player) continue;

        if (lightCount >= MAX_LIGHTS && !light.follow_player)
            break;
        
        

        Vector2 screenPos = GetWorldToScreen2D(light.pos, cam);

        positions[lightCount] =
        {
            screenPos.x / _WINDOW_WIDTH_,
            1.0f - (screenPos.y / _WINDOW_HEIGHT_)
        };

        if(light.follow_player){
            positions[lightCount] = {0.5f, 0.5f};
        }

        // cout <<lightCount << ": " <<   positions[lightCount].x << "  " << positions[lightCount].y << "\n";

        
        radii[lightCount] =
            light.radius / _WINDOW_WIDTH_;

        // cout << (game.ticks_passed-floor(game.ticks_passed)) << "\n";
        
        switch(light.flicker_type){
            case NONE:
                break;
            case FLICKER:
                radii[lightCount] += (GetRandomValue(-100, 100)/5000.0f)*light.flicker_strength;
                break;
            case WAVE:
                radii[lightCount] += (radii[lightCount] / 5.0f * sinf(game.ticks_passed / 10.0f)) * light.flicker_strength;;
                break;
        }

        colors[lightCount] =
        {
            light.color.r / 255.0f,
            light.color.g / 255.0f,
            light.color.b / 255.0f
        };

        intensity[lightCount] = light.intensity;
        softness[lightCount] = light.softness;
        brightness[lightCount] = light.brightness;
        // colored[lightCount] = light.colored;


        lightCount++;
    }
    // cout << _AMBIENT_LIGHT_ << "\n";
    float aspect = (float)_WINDOW_WIDTH_ / (float)_WINDOW_HEIGHT_;
    float ambient = _AMBIENT_LIGHT_;

    auto shader = _LIGHTING_SHADER_;

    // printf(
    //     "pos %.2f %.2f  radius %.2f\n",
    //     positions[0].x,
    //     positions[0].y,
    //     radii[0]
    // );

    // cout << lightCount << "\n";

    

    SetShaderValueV(shader,
        _LIGHT_POSITIONS_LOC,
        positions,
        SHADER_UNIFORM_VEC2,
        lightCount);

    SetShaderValueV(shader,
        LIGHT_RADII_LOC,
        radii,
        SHADER_UNIFORM_FLOAT,
        lightCount);

    SetShaderValueV(shader,
        _LIGHT_COLORS_LOC_,
        colors,
        SHADER_UNIFORM_VEC3,
        lightCount);

    SetShaderValueV(shader,
        _LIGHT_INTENSITY_LOC_,
        intensity,
        SHADER_UNIFORM_FLOAT,
        lightCount);

    SetShaderValueV(shader,
        _LIGHT_SOFTNESS_LOC_,
        softness,
        SHADER_UNIFORM_FLOAT,
        lightCount);
        
    SetShaderValueV(shader,
        _LIGHT_BRIGHTNESS_LOC_,
        brightness,
        SHADER_UNIFORM_FLOAT,
        lightCount);

    SetShaderValue(shader,
        _LIGHT_COUNT_LOC_,
        &lightCount,
        SHADER_UNIFORM_INT);

    SetShaderValue(shader,
        _ASPECT_LOC_,
        &aspect,
        SHADER_UNIFORM_FLOAT);

    SetShaderValue(shader,
        _AMBIENT_LOC_,
        &ambient,
        SHADER_UNIFORM_FLOAT);
}

void draw_lighting()
{
    // if (!draw_overlay)
    //     return;

    BeginTextureMode(_LIGHTING_TEX_);
    ClearBackground(BLACK);
    EndTextureMode();

    

    

    BeginTextureMode(light_scale_tex);
    ClearBackground(BLANK);

    BeginShaderMode(_LIGHTING_SHADER_);

    DrawTexturePro(
        _LIGHTING_TEX_.texture,
        {0, 0, 320, -180},
        {0, 0, 320, 180},
        {0, 0},
        0,
        WHITE
    );

    EndShaderMode();

    EndTextureMode();

    DrawTexturePro(
        light_scale_tex.texture,
        {0, 0, 320, -180},
        {0, 0, 320 * scale, 180 * scale},
        {0, 0},
        0,
        WHITE
    );

    // Image temp = LoadImageFromTexture(darkness_tex.texture);
    // ImageResizeNN(&temp, 320, 180);
    
}

// void draw_dimming()
// {
//     DrawRectangle(
//         0,
//         0,
//         _WINDOW_WIDTH_ * scale,
//         _WINDOW_HEIGHT_ * scale,
//         Fade(BLACK, 0.8f)
//     );
// }