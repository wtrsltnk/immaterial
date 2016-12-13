#ifndef INPUT_H
#define INPUT_H

#include <map>
#include <string>
#include "iinput.h"

enum class ActionStates
{
    Pressed,
    Released
};

enum class MouseButtons
{
    Left,
    Right,
    Middle,
    Button4,
    Button5,

    KeyboardKeysCount
};

enum class KeyboardKeys
{
    Return,
    Escape,
    Backspace,
    Tab,
    KP_Up,
    KP_Down,
    KP_Left,
    KP_Right,
    Character_Space,
    Character_A,
    Character_B,
    Character_C,
    Character_D,
    Character_E,
    Character_F,
    Character_G,
    Character_H,
    Character_I,
    Character_J,
    Character_K,
    Character_L,
    Character_M,
    Character_N,
    Character_O,
    Character_P,
    Character_Q,
    Character_R,
    Character_S,
    Character_T,
    Character_U,
    Character_V,
    Character_W,
    Character_X,
    Character_Y,
    Character_Z,

    KeyboardKeysCount
};

class Input : public IInput
{
    DigitalActionState _digitalActionStates[MAX_DIGITAL_ACTIONS + 1];
    std::map<std::string, DigitalActionHandle> _digitalActionHandles;
    DigitalActionHandle _digitalMapping_OnKeyAction[int(KeyboardKeys::KeyboardKeysCount)];
    DigitalActionHandle _digitalMapping_OnMouseAction[int(MouseButtons::KeyboardKeysCount)];

    AnalogActionState _analogActionStates[MAX_ANALOG_ACTIONS + 1];
    std::map<std::string, AnalogActionHandle> _analogActionHandles;
    AnalogActionHandle _analogMapping_OnMouseMove;

public:
    Input();
    virtual ~Input();

    virtual void OnKeyAction(KeyboardKeys key, ActionStates state);
    virtual void OnMouseAction(MouseButtons button, ActionStates state);
    virtual void OnMouseMove(double x, double y);

    virtual DigitalActionHandle getDigitalActionHandle(const char* name);
    virtual DigitalActionState getDigitalActionData(DigitalActionHandle action);

    virtual AnalogActionHandle getAnalogActionHandle(const char* name);
    virtual AnalogActionState getAnalogActionData(AnalogActionHandle action);

};

#endif // INPUT_H
