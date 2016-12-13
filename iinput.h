#ifndef IINPUT_H
#define IINPUT_H

#define MAX_ANALOG_ACTIONS 16

#define MAX_DIGITAL_ACTIONS 128

struct DigitalActionState
{
    bool state;
    bool active;
};

enum class AnalogActionSourceTypes
{
    AbsoluteMouse,
    RelativeMouse
};

struct AnalogActionState
{
    AnalogActionSourceTypes source;
    float x, y;
    bool active;
};

typedef unsigned int DigitalActionHandle;
typedef unsigned int AnalogActionHandle;

class IInput
{
public:
    virtual ~IInput();

    virtual DigitalActionHandle getDigitalActionHandle(const char* name) = 0;
    virtual DigitalActionState getDigitalActionData(DigitalActionHandle action) = 0;

    virtual AnalogActionHandle getAnalogActionHandle(const char* name) = 0;
    virtual AnalogActionState getAnalogActionData(AnalogActionHandle action) = 0;
};

#endif // IINPUT_H
