#ifndef UI_H
#define UI_H

#include <map>
#include <set>
#include <queue>
#include <string>
#include <functional>
#include <glm/glm.hpp>
#include "font.h"
#include "iinput.h"

class ControlParent
{
    glm::mat4 _ortho;
    int _width;
    int _height;
    std::set<class Control*> _controls;
protected:
    const glm::mat4& ortho() const;

public:
    virtual ~ControlParent();

    void resize(int width, int height);
    int width() const;
    int height() const;

    virtual void addControl(class Control* control);
    virtual void removeControl(class Control* control);
    virtual class Control* byId(const std::string& id);
    std::set<class Control*>& controls();

};

class Control
{
    friend class Container;
    std::string _id;
    ControlParent* _parent;
    Font& _font;
protected:
    std::string _text;
    Font::Buffer _textBuffer;
    glm::vec2 _size;
    glm::vec2 _padding;
    glm::vec4 _color;

    virtual glm::vec2 getMinimalSize();
    virtual glm::vec2 getEffectiveSize();
    glm::vec2 getEffectivePosition();
public:
    Control(const std::string& id, ControlParent* parent = nullptr);
    virtual ~Control();

    virtual void update(float elapsed);
    virtual void render(const glm::mat4& proj, const glm::mat4& view, const glm::vec2& pos);

    const std::string& id() const;

    void setParent(ControlParent* parent);
    ControlParent* parent() const;

    void setFont(const Font& font);
    const Font& font() const;

    void setText(const std::string& text);
    const std::string& text() const;

    void setSize(const glm::vec2& size);
    const glm::vec2& size() const;

    void setPadding(const glm::vec2& padding);
    const glm::vec2& padding() const;

    void setColor(const glm::vec4& color);
    const glm::vec4& color() const;
};

class Button : public Control
{
    std::function<void (const Control*)> _onClickfunc;

public:
    Button(const std::string& id);
    virtual ~Button();

    virtual void render(const glm::mat4 &proj, const glm::mat4& view, const glm::vec2& pos);

    void onClick(std::function<void (const Control*)> func);
    void click();
};

enum class ContainerChildDirections
{
    LeftToRight,
    RightToLeft,
    TopToBottom,
    BottomToTop
};

class Container : public ControlParent, public Control
{
    ContainerChildDirections _direction;
protected:
    virtual glm::vec2 getMinimalSize();

public:
    Container(const std::string& id);
    virtual ~Container();

    void setDirection(ContainerChildDirections direction);
    ContainerChildDirections direction() const;

    virtual void update(float elapsed);
    virtual void render(const glm::mat4& proj, const glm::mat4& view, const glm::vec2& pos);

};

class UI : public ControlParent
{
    IInput* _input;
    AnalogActionHandle _swipeHandle;
    DigitalActionHandle _startSwipingHandle;

    Font _font;
    Control* _hoverControl;
    Control* _clickControl;
    std::queue<Control*> _clickedControls;

    static UI* _instance;
    UI();
public:
    static UI& Manager();
    virtual ~UI();

    void init(const std::string& font, float size);
    virtual void update(float elapsed);
    virtual void render();
    static Control* findById(const std::string& id);

    void setHoverControl(Control* control);
    const Control* hoverControl() const;
    std::queue<Control*>& clickedControls();

    Font& font();
    const glm::mat4& projection() const;
    static Geometry* _hoverGeom;
    static Texture* _hoverTexture;
};

class flUI
{
    std::string _id;
    Control* _control;
public:
    flUI(const std::string& id);
    virtual ~flUI();

    flUI& Lbl(const std::string& text);
    flUI& Btn(const std::string& text);
    flUI& Cntr();
    flUI& AddChild(Control* control);
    flUI& Text(const std::string& text);
    flUI& Padding(int x, int y);
    flUI& Size(int x, int y);
    flUI& UseFont(Font& font);
    flUI& UseColor(int r, int g,  int b, int a = 255);

    flUI& OnClick(std::function<void (const Control*)> func);

    operator Control* ();
    Control* AddControl();
};

#endif // UI_H
