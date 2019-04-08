/*  Experimenting with easings and rings !
  * Copyright (c)  - 2019    Vlad Adrian (@demizdor :: https://github.com/Demizdor/)
 */

// INCLUDES
// --------------------------------------------------------
#include <stdio.h>
#include <limits.h>
#include <math.h>

#include <raylib.h>

#include "external/easings.h"
#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"
// --------------------------------------------------------


// TYPES
// --------------------------------------------------------
enum {
    RING_EXPAND = 0,
    RING_CONTRACT
};

typedef struct 
{
    float time;
    float end;
    Color color;
    int phase;
} Ring;
// --------------------------------------------------------


#define SIZEOF(A) (sizeof(A)/sizeof(A[0]))

// CONSTANTS
// --------------------------------------------------------
const int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 450;
const char* const EasingNames = "EaseLinear;EaseSineIn;EaseSineOut;EaseSineInOut;EaseCircIn;EaseCircOut;EaseCircInOut;"
"EaseCubicIn;EaseCubicOut;EaseCubicInOut;EaseQuadIn;EaseQuadOut;EaseQuadInOut;EaseBackIn;EaseBackOut;EaseBackInOut;"
"EaseBounceIn;EaseBounceOut;EaseBounceInOut;EaseElasticIn;EaseElasticOut;EaseElasticInOut";
const float (*Easings[])(float, float, float, float) = {
    &EaseLinearInOut, &EaseSineIn, &EaseSineOut, &EaseSineInOut,
    &EaseCircIn, &EaseCircOut, &EaseCircInOut,
    &EaseCubicIn, &EaseCubicOut, &EaseCubicInOut, 
    &EaseQuadIn, &EaseQuadOut, &EaseQuadInOut,
    &EaseBackIn, &EaseBackOut, &EaseBackInOut,
    &EaseBounceIn, &EaseBounceOut, &EaseBounceInOut,
    &EaseElasticIn, &EaseElasticOut, &EaseElasticInOut
};
// --------------------------------------------------------

// VARIABLES
// --------------------------------------------------------
int ANGLE = 90;
int PADDING = 2;
int SIZE = 8;
int SPACING = 12;
float TIME = 1.6f, MIN_DELAY = 0.1f, MAX_DELAY = 0.9f;
int SEGMENTS = 0;
bool DRAW_RING_LINES = true, DRAW_RINGS = true, COLORS = false;
float HUE = 200.0f, SAT = 0.20f, VAL = 0.95f, STEP= 15.5f;
int START = 2, END = 8, RENDER_EASING_EXPAND = 1, RENDER_EASING_CONTRACT = 4, TIME_EASING = 16;
bool PAUSED = false;
Ring rings[16] = {0};
// --------------------------------------------------------



// FUNCTION FORWARD DECLARATIONS
// --------------------------------------------------------
void DrawAnimation(void);
void RingReset(int ring);
void DrawGui();
void Update();
Color GenerateRandomColor(float s, float v);
// --------------------------------------------------------


int main(int argc, char **argv)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib - ring test");
    SetTargetFPS(60);
    
    GuiLoadStyleDefault();
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_RIGHT);
    GuiSetStyle(SLIDER, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_RIGHT);
    GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_RIGHT);
    
    HUE = GetRandomValue(0, 360);
    STEP = GetRandomValue(10, 137);
    
    // Initialize the rings
    for(int i=0; i<SIZEOF(rings); ++i) 
    {
        RingReset(i);
        //rings[i].color = GenerateRandomColor(0.499f, 0.988f);
        if(COLORS) rings[i].color = ColorFromHSV((Vector3){ fmod(HUE+(i+1)*STEP, 360.0f), SAT, VAL});
        else rings[i].color = GRAY;
        rings[i].phase = RING_EXPAND;
    }
    
    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        Update();
        
        DrawAnimation();
        DrawGui();
        
        DrawText(TextFormat("FPS:%i %s", GetFPS(), PAUSED == true ? "PAUSED": " "),10,10,20,BLACK);
        DrawText("Press SPACE to pause / unpause!",10,30,10,BLACK);
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}


void DrawAnimation()
{
    const Vector2 center = {(SCREEN_WIDTH-100)/2, SCREEN_HEIGHT/2};
    const float frame = GetFrameTime();
    for(int i=0; i< SIZEOF(rings); ++i)
    {
        float angle = 0.0f;
        int radius = 20 + i*SPACING;
        
        angle = (rings[i].phase == RING_EXPAND) ? Easings[RENDER_EASING_EXPAND](rings[i].time, 0.0f, 179.0f, rings[i].end) :
                            Easings[RENDER_EASING_CONTRACT](rings[i].time, 179.0f, -179.0f, rings[i].end);
        
        if(i >= START && i <= END) {
            if(i == 0) {
                if(DRAW_RINGS) 
                    DrawCircleSector(center, radius, ANGLE - angle + PADDING, ANGLE + angle - PADDING,SEGMENTS, rings[i].color);
                if(DRAW_RING_LINES)
                    DrawCircleSectorLines(center, radius, ANGLE + angle + PADDING,  360+(ANGLE - angle)-PADDING, SEGMENTS, rings[i].color);
                    
            } else {
                if(DRAW_RINGS)
                    DrawRing(center, radius, radius + SIZE, ANGLE - angle + PADDING, ANGLE+angle - PADDING, SEGMENTS, rings[i].color );
                if(DRAW_RING_LINES) 
                    DrawRingLines(center, radius, radius + SIZE, ANGLE + angle + PADDING,  360+(ANGLE-angle)-PADDING, SEGMENTS, rings[i].color);
            }
        }
        
        if(PAUSED == false)
        {
            const float time = rings[i].time + frame;
            if(time >= rings[i].end) RingReset(i);
            else rings[i].time = time;
        }
    }
}

void RingReset(int ring)
{
    rings[ring].time = 0;
    //rings[ring].end = EaseElasticIn(ring, 0.9f, 0.92f, SIZEOF(rings)-1) + (float)(SIZEOF(rings)-ring)*0.02f;
    rings[ring].end = TIME + Easings[TIME_EASING](ring, MIN_DELAY, MAX_DELAY, SIZEOF(rings));
    rings[ring].phase = !rings[ring].phase;
}

void DrawGui()
{
    GuiLabel((Rectangle){500,25,140,10}, "Timer Easing");
    TIME_EASING = GuiComboBox((Rectangle){650,20,140,20}, EasingNames, TIME_EASING);
    
    GuiLabel((Rectangle){500,50,140,10}, "Render Easing I");
    RENDER_EASING_EXPAND = GuiComboBox((Rectangle){650,45,140,20}, EasingNames, RENDER_EASING_EXPAND);
    
    GuiLabel((Rectangle){500,75,140,10}, "Render Easing II");
    RENDER_EASING_CONTRACT = GuiComboBox((Rectangle){650,70,140,20}, EasingNames, RENDER_EASING_CONTRACT);
    
    TIME = GuiSlider((Rectangle){650,120,111,20}, "Start time", TIME, 0.4f, 4.0f, true);
    MIN_DELAY = GuiSlider((Rectangle){650,145,111,20}, "Min Delay", MIN_DELAY, 0.1f, 1.0f, true);
    MAX_DELAY = GuiSlider((Rectangle){650,170,111,20}, "Max Delay", MAX_DELAY, 0.2f, 2.0f, true);
    
    bool colors = COLORS;
    float hue=HUE, sat=SAT, val = VAL, step=STEP;
    colors = GuiCheckBox((Rectangle){705,410,20,20}, "Colors", colors);
    if(colors)
    {
        hue = GuiSlider((Rectangle){650,210,110,20}, "Hue", hue, 0.0f, 360.f, true);
        sat = GuiSlider((Rectangle){650,235,110,20}, "Sat", sat, 0.0f, 1.0f, true);
        val = GuiSlider((Rectangle){650,260,110,20}, "Val", val, 0.0f, 1.0f, true);
        step = GuiSlider((Rectangle){650,285,110,20}, "Step", step, 0.0f, 137.508f, true);
    }
    if(colors != COLORS || HUE!= hue || SAT != sat || val != VAL || step != STEP)
    {
        // REGENERATE RING COLORS
        for(int i=0; i<SIZEOF(rings); ++i)
        {
            if(colors) rings[i].color = ColorFromHSV((Vector3){ fmod(hue+(i+1)*step, 360.0f), sat, val});
            else rings[i].color = GRAY;
        }
    }
    COLORS = colors; HUE = hue; SAT = sat; VAL = val; STEP = step;
    
    const Vector2 mouse = GetMousePosition();
    GuiLabel((Rectangle){495,325,105,10}, "Min/Max");
    GuiSpinner((Rectangle){605,320,85,20}, &START, 0, SIZEOF(rings)-1, CheckCollisionPointRec(mouse,(Rectangle){605,320,85,20}));
    START = START >= SIZEOF(rings) ? SIZEOF(rings)-1 : START;
    GuiSpinner((Rectangle){705,320,85,20}, &END, 0, SIZEOF(rings)-1, CheckCollisionPointRec(mouse,(Rectangle){705,320,85,20}));
    END = END >= SIZEOF(rings) ? SIZEOF(rings)-1 : END;

    GuiLabel((Rectangle){495,355,105,10}, "Angle/Padding");
    GuiSpinner((Rectangle){605,350,85,20},&ANGLE, -360, 360, CheckCollisionPointRec(mouse,(Rectangle){605,350,85,20}));
    GuiSpinner((Rectangle){705,350,85,20},&PADDING, -180, 180, CheckCollisionPointRec(mouse,(Rectangle){705,350,85,20}));

    GuiLabel((Rectangle){495,385,105,10}, "Size/Spacing");
    GuiSpinner((Rectangle){605,380,85,20},&SIZE, 1, 90, CheckCollisionPointRec(mouse,(Rectangle){605,380,85,20}));
    GuiSpinner((Rectangle){705,380,85,20},&SPACING, -40, 40, CheckCollisionPointRec(mouse,(Rectangle){705,380,85,20}));
    
    GuiLabel((Rectangle){495,415,105,10}, "Segments");
    GuiSpinner((Rectangle){605,410,85,20},&SEGMENTS, 0, 40, CheckCollisionPointRec(mouse,(Rectangle){605,410,85,20}));
    
    
    if(GuiButton((Rectangle){20,380,100,30}, "RESET"))
    {
        for(int i=0; i<SIZEOF(rings); ++i) 
        {
            RingReset(i);
            if(COLORS) rings[i].color = ColorFromHSV((Vector3){ fmod(HUE+(i+1)*STEP, 360.0f), SAT, VAL});
            else rings[i].color = GRAY;
            rings[i].phase = RING_EXPAND;
        }
    }
    
    DRAW_RINGS = GuiCheckBox((Rectangle){20,415,20,20}, "Draw Rings", DRAW_RINGS);
    DRAW_RING_LINES = GuiCheckBox((Rectangle){135,415,20,20}, "Draw Lines", DRAW_RING_LINES);
}


void Update()
{
    if(IsKeyPressed(KEY_SPACE)) PAUSED = !PAUSED;   // Toggle `Pause mode` when space is pressed
}

// Generates a nice color with a random hue
Color GenerateRandomColor(float s, float v)
{
    const float Phi = 0.618033988749895f; // golden ratio conjugate
    Vector3 hsv = {GetRandomValue(0, 360), s, v};
    hsv.x = fmod((hsv.x + hsv.x*Phi), 360.0f);
    return ColorFromHSV(hsv);
}