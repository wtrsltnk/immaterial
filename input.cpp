#include "input.h"
#include <iostream>

IInput::~IInput()
{ }

Input::Input()
{
    // For now we hard code this part
    this->_digitalActionHandles = std::map<std::string, DigitalActionHandle>({
                                                                                 { "start_throw", 1 },
                                                                                 { "menu_up", 2 },
                                                                                 { "menu_down", 3 },
                                                                                 { "menu_left", 4 },
                                                                                 { "menu_right", 5 },
                                                                                 { "menu_select", 6 },
                                                                                 { "menu_cancel", 7 }
                                                                             });

    this->_analogActionHandles = std::map<std::string, AnalogActionHandle>({
                                                                                 { "throwing", 1 }
                                                                             });

    for (unsigned int i; i < MAX_DIGITAL_ACTIONS; i++)
    {
        this->_digitalMapping_OnKeyAction[i] = MAX_DIGITAL_ACTIONS;
        this->_digitalMapping_OnMouseAction[i] = MAX_DIGITAL_ACTIONS;
    }
    this->_digitalMapping_OnMouseAction[0] = 1; // "start_throw"
    this->_analogMapping_OnMouseMove = 1;       // "throwing"
}

Input::~Input()
{ }

void Input::OnKeyAction(KeyboardKeys key, ActionStates state)
{
    DigitalActionHandle mappedAction = this->_digitalMapping_OnKeyAction[int(key)];
    if (mappedAction != MAX_DIGITAL_ACTIONS)
    {
        this->_digitalActionStates[mappedAction].state = (state == ActionStates::Pressed);
    }
}

void Input::OnMouseAction(MouseButtons button, ActionStates state)
{
    DigitalActionHandle mappedAction = this->_digitalMapping_OnMouseAction[int(button)];
    if (mappedAction != MAX_DIGITAL_ACTIONS)
    {
        this->_digitalActionStates[mappedAction].state = (state == ActionStates::Pressed);
    }
}

void Input::OnMouseMove(double x, double y)
{
    if (this->_analogMapping_OnMouseMove != MAX_ANALOG_ACTIONS)
    {
        this->_analogActionStates[int(this->_analogMapping_OnMouseMove)].x = x;
        this->_analogActionStates[int(this->_analogMapping_OnMouseMove)].y = y;
        this->_analogActionStates[int(this->_analogMapping_OnMouseMove)].source = AnalogActionSourceTypes::AbsoluteMouse;
    }
}

DigitalActionHandle Input::getDigitalActionHandle(const char* name)
{
    std::cout << name << std::endl;
    if (this->_digitalActionHandles.find(name) != this->_digitalActionHandles.end()) return this->_digitalActionHandles[name];

    return -1;
}

DigitalActionState Input::getDigitalActionData(DigitalActionHandle action)
{
    if (action >= 0 && action < MAX_DIGITAL_ACTIONS)
    {
        return this->_digitalActionStates[action];
    }
    return this->_digitalActionStates[MAX_DIGITAL_ACTIONS];
}

AnalogActionHandle Input::getAnalogActionHandle(const char* name)
{
    std::cout << name << std::endl;
    if (this->_analogActionHandles.find(name) != this->_analogActionHandles.end()) return this->_analogActionHandles[name];

    return -1;
}

AnalogActionState Input::getAnalogActionData(AnalogActionHandle action)
{
    if (action >= 0 && action < MAX_ANALOG_ACTIONS)
    {
        return this->_analogActionStates[action];
    }
    return this->_analogActionStates[MAX_ANALOG_ACTIONS];
}
