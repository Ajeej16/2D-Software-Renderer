/* date = October 30th 2021 5:11 pm */

#ifndef INPUT_H
#define INPUT_H

// NOTE(ajeej): Used to access keys in key array
typedef enum key_id
{
    // NOTE(ajeej): Look in "input_keys.inc" for definitions
#define Key(name, str) Key_ID_##name,
#include "keys.inc"
#undef Key
    Key_ID_Max,
} key_id;


typedef enum mouse_id
{
    Mouse_ID_Left,
    Mouse_ID_Right,
    Mouse_ID_Middle,
    Mouse_ID_Btn1,
    Mouse_ID_Btn2,
    
    Mouse_ID_Max,
} mouse_id;



// TODO(ajeej): Handle Characters
//              - Character Buffer?

typedef enum input_state
{
    
    Input_State_Down = (1<<0),
    Input_State_Pressed = (1<<1),
    Input_State_Released = (1<<2),
    Input_State_Double = (1<<3),
    Input_State_Ctrl = (1<<4),
    Input_State_Shift = (1<<5),
    Input_State_Alt = (1<<6),
    
} key_state;

typedef struct input_t
{
    u8 key_states[75];
    u8 mouse_states[5];
    v2_t mouse_pos;
    v2_t mouse_delta;
    v2_t mouse_scroll;
} input_t;

#define IsKeyPressed(input, id) ((input).key_states[id] & Input_State_Pressed)
#define IsKeyReleased(input, id) ((input).key_states[id] & Input_State_Released)
#define IsKeyDown(input, id) ((input).key_states[id] & Input_State_Down)
#define IsKeyUp(input, id) (!IsKeyDown(input, id))

#define IsMousePressed(input, id) ((input).mouse_states[id] & Input_State_Pressed)
#define IsMouseReleased(input, id) ((input).mouse_states[id] & Input_State_Released)
#define IsMouseDown(input, id) ((input).mouse_states[id] & Input_State_Down)
#define IsMouseUp(input, id) (!IsMouseDown(input, id))

#define IsKeyAndModPressed(input, id, mod) (IsKeyPressed(input, id) && (input.key_states[id] & mod))


internal b32
update_input_state(u8 *input_state, b32 is_down, b32 was_down, u8 modifiers)
{
    b32 was_pressed_or_released = 0;
    
    // TODO(ajeej): Find a way to make modifier update faster
    *input_state &= ~(Input_State_Ctrl|Input_State_Alt|Input_State_Shift);
    *input_state |= modifiers;
    
    if(is_down)
    {
        *input_state |= Input_State_Down;
        if(!was_down)
        {
            *input_state |= Input_State_Pressed;
            was_pressed_or_released = 1;
        }
    }
    else
    {
        *input_state &= ~(Input_State_Down);
        *input_state &= ~(Input_State_Pressed);
        *input_state |= Input_State_Released;
        was_pressed_or_released = 1;
    }
    
    return was_pressed_or_released;
}

#endif //INPUT_H
