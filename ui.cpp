#include "ui.h"
#include "input.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>

Geometry* UI::_hoverGeom = nullptr;
Texture* UI::_hoverTexture = nullptr;

UI* UI::_instance = nullptr;

UI::UI()
    : _hoverControl(nullptr), _clickControl(nullptr)
{ }

UI& UI::Manager()
{
    if (UI::_instance == nullptr) UI::_instance = new UI();

    return *UI::_instance;
}

UI::~UI()
{ }

void UI::init(const std::string& font, float size)
{
    this->_font.init(font, size);
    UI::_hoverGeom = new Geometry();
    UI::_hoverGeom->init();
    UI::_hoverTexture = new Texture();
    UI::_hoverTexture->upload();
}

void UI::update(float elapsed)
{
    for (auto control : this->controls())
        control->update(elapsed);

//    if (Input::Manager().isDown(MouseButtons::Left))
//    {
//        if (this->_clickControl != this->_hoverControl) this->_clickControl = this->_hoverControl;
//    }

//    if (!Input::Manager().isDown(MouseButtons::Left))
//    {
//        if (this->_clickControl != nullptr)
//        {
//            std::cout << "item clicked" << std::endl;
//            this->_clickedControls.push(this->_clickControl);
//            this->_clickControl = nullptr;
//        }
//    }
}

void UI::render()
{
    for (auto control : this->controls())
        control->render(this->ortho(), glm::mat4(1.0f), glm::vec2());
}

Control* UI::findById(const std::string& id)
{
    for (auto control : UI::Manager().controls())
    {
        auto container = dynamic_cast<Container*>(control);
        if (container != nullptr)
        {
            auto found = container->byId(id);
            if (found != nullptr) return found;
        }
        else if (control->id() == id)
            return control;
    }

    return nullptr;
}

void UI::setHoverControl(Control* control)
{
    this->_hoverControl = control;
}

const Control* UI::hoverControl() const
{
    return this->_hoverControl;
}

std::queue<Control*>& UI::clickedControls()
{
    return this->_clickedControls;
}

Font& UI::font()
{
    return this->_font;
}

const glm::mat4& UI::projection() const
{
    return this->ortho();
}






ControlParent::~ControlParent()
{ }

const glm::mat4& ControlParent::ortho() const
{
    return this->_ortho;
}

int ControlParent::width() const
{
    return this->_width;
}

int ControlParent::height() const
{
    return this->_height;
}

void ControlParent::resize(int width, int height)
{
    this->_width = width;
    this->_height = height;

    this->_ortho = glm::ortho(0.0f, float (width), float (height), 0.0f, -10.0f, 10.0f);
}

void ControlParent::addControl(Control *control)
{
    this->_controls.insert(control);
}

void ControlParent::removeControl(Control* control)
{
    this->_controls.erase(control);
}

Control* ControlParent::byId(const std::string& id)
{
    for (auto c : this->_controls)
        if (c->id() == id) return c;

    return nullptr;
}

std::set<class Control*>& ControlParent::controls()
{
    return this->_controls;
}







Control::Control(const std::string& id, ControlParent* parent)
    : _id(id), _parent(parent == nullptr ? &UI::Manager() : parent), _font(UI::Manager().font()),
      _color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{
    this->_textBuffer.init();
}

Control::~Control()
{ }

glm::vec2 Control::getMinimalSize()
{
    return glm::vec2(std::max(int(this->_size.x), this->_textBuffer.width()), std::max(int(this->_size.y), this->_textBuffer.height()));
}

glm::vec2 Control::getEffectiveSize()
{
    return glm::vec2(this->_textBuffer.width(), this->_textBuffer.height());
}

void Control::update(float elapsed)
{
    auto s = this->getEffectiveSize();
//    auto p = this->getEffectivePosition();
//    auto m = Input::Manager().mousePosition();

//    if (m.x >= p.x && m.x <= (p.x+s.x) && m.y >= (p.y-s.y) && m.y <= p.y)
//    {
//        UI::Manager().setHoverControl(this);
//    }
//    else if (UI::Manager().hoverControl() == this)
//    {
//        UI::Manager().setHoverControl(nullptr);
//    }
}

void Control::render(const glm::mat4& proj, const glm::mat4& view, const glm::vec2& pos)
{
    glDisable(GL_DEPTH_TEST);
    this->_textBuffer.render(proj, view, glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, 0.0f)));
    glEnable(GL_DEPTH_TEST);
}

const std::string& Control::id() const
{
    return this->_id;
}

void Control::setParent(ControlParent* parent)
{
    this->_parent = parent;
}

ControlParent* Control::parent() const
{
    return this->_parent;
}

void Control::setFont(const Font& font)
{
    this->_font = font;
    this->_font.print(this->_text, this->_textBuffer);
}

const Font& Control::font() const
{
    return this->_font;
}

void Control::setText(const std::string& text)
{
    this->_text = text;
    this->_font.print(text, this->_textBuffer);
}

const std::string& Control::text() const
{
    return this->_text;
}

void Control::setSize(const glm::vec2& size)
{
    this->_size = size;
}

const glm::vec2& Control::size() const
{
    return this->_size;
}


void Control::setPadding(const glm::vec2& padding)
{
    this->_padding = padding;
}

const glm::vec2& Control::padding() const
{
    return this->_padding;
}

void Control::setColor(const glm::vec4& color)
{
    this->_color = color;
    this->_textBuffer.setColor(color);
}

const glm::vec4& Control::color() const
{
    return this->_color;
}






Button::Button(const std::string& id)
    : Control(id)
{ }

Button::~Button()
{ }

void Button::onClick(std::function<void (const Control*)> func)
{
    this->_onClickfunc = func;
}

void Button::render(const glm::mat4 &proj, const glm::mat4& view, const glm::vec2& pos)
{
//    glDisable(GL_DEPTH_TEST);
//    glm::vec2 p = this->getEffectivePosition();

//    if (UI::Manager().hoverControl() == this)
//    {
//        auto min = this->_textBuffer.min();
//        auto max = this->_textBuffer.max();
//        std::vector<tVertex> vertices = {
//            // Front Face
//            { { min.x - _padding.x,  min.y - _padding.y,  0.0f }, {  0.0f,  0.0f,  1.0f}, { 0.0f, 0.0f } }, // Bottom Left Of The Texture and Quad
//            { { max.x + _padding.x,  min.y - _padding.y,  0.0f }, {  0.0f,  0.0f,  1.0f}, { 1.0f, 0.0f } }, // Bottom Right Of The Texture and Quad
//            { { max.x + _padding.x,  max.y + _padding.y,  0.0f }, {  0.0f,  0.0f,  1.0f}, { 1.0f, 1.0f } }, // Top Right Of The Texture and Quad
//            { { min.x - _padding.x,  max.y + _padding.y,  0.0f }, {  0.0f,  0.0f,  1.0f}, { 0.0f, 1.0f } }, // Top Left Of The Texture and Quad
//        };

//        UI::_hoverTexture->bind();
//        UI::_hoverGeom->setMode(GL_QUADS);
//        UI::_hoverGeom->setVertices(vertices);
//        UI::_hoverGeom->render(proj, glm::mat4(1.0f), glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y, 0.0f)));
//    }
//    this->_textBuffer.render(proj, glm::mat4(1.0f), glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y, 0.0f)));
//    glEnable(GL_DEPTH_TEST);
    Control::render(proj, view,  pos);
}

void Button::click()
{
    if (this->_onClickfunc) this->_onClickfunc(this);
}







Container::Container(const std::string& id)
    : Control(id), _direction(ContainerChildDirections::LeftToRight)
{ }

Container::~Container()
{ }

void Container::setDirection(ContainerChildDirections direction)
{
    this->_direction = direction;
}

ContainerChildDirections Container::direction() const
{
    return this->_direction;
}

glm::vec2 Container::getMinimalSize()
{
    glm::vec2 result;
    for (auto child : this->controls())
    {
        glm::vec2 min = child->getMinimalSize();
        switch (this->direction())
        {
        case ContainerChildDirections::LeftToRight: { result.x += min.x; if (min.y > result.y) result.y = min.y; break; }
        case ContainerChildDirections::RightToLeft: { result.x += min.x; if (min.y > result.y) result.y = min.y; break; }
        case ContainerChildDirections::TopToBottom: { result.y += min.y; if (min.x > result.x) result.x = min.x; break; }
        case ContainerChildDirections::BottomToTop: { result.y += min.y; if (min.x > result.x) result.x = min.x; break; }
        }
    }
    return result;
}

void Container::update(float elapsed)
{
    Control::update(elapsed);
    for (auto child : this->controls()) child->update(elapsed);
}

void Container::render(const glm::mat4& proj, const glm::mat4& view, const glm::vec2& pos)
{
    glm::vec2 min;
    glm::vec2 max = this->getMinimalSize();

    std::vector<tVertex> vertices = {
        // Front Face
        { { min.x - _padding.x,  min.y - _padding.y,  0.0f }, {  0.0f,  0.0f,  1.0f}, { 0.0f, 0.0f } }, // Bottom Left Of The Texture and Quad
        { { max.x + _padding.x,  min.y - _padding.y,  0.0f }, {  0.0f,  0.0f,  1.0f}, { 1.0f, 0.0f } }, // Bottom Right Of The Texture and Quad
        { { max.x + _padding.x,  max.y + _padding.y,  0.0f }, {  0.0f,  0.0f,  1.0f}, { 1.0f, 1.0f } }, // Top Right Of The Texture and Quad
        { { min.x - _padding.x,  max.y + _padding.y,  0.0f }, {  0.0f,  0.0f,  1.0f}, { 0.0f, 1.0f } }, // Top Left Of The Texture and Quad
    };

    UI::_hoverTexture->bind();
    UI::_hoverGeom->setMode(GL_QUADS);
    UI::_hoverGeom->setVertices(vertices);
    UI::_hoverGeom->setColor(this->_color);

    auto localView = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, 0.0f));
    UI::_hoverGeom->render(proj, glm::mat4(1.0f), localView);

    for (auto child : this->controls()) child->render(proj, view * localView, glm::vec2());
}






flUI::flUI(const std::string& id)
    : _id(id), _control(nullptr)
{ }

flUI::~flUI()
{ }

flUI& flUI::Lbl(const std::string& text)
{
    if (this->_control != nullptr) delete this->_control;

    this->_control = new Control(this->_id);
    this->_control->setText(text);

    return *this;
}

flUI& flUI::Btn(const std::string& text)
{
    if (this->_control != nullptr) delete this->_control;

    this->_control = new Button(this->_id);
    this->_control->setText(text);

    return *this;
}

flUI& flUI::Cntr()
{
    if (this->_control != nullptr) delete this->_control;

    this->_control = new Container(this->_id);

    return *this;
}

flUI& flUI::Text(const std::string& text)
{
    if (this->_control != nullptr)
    {
        this->_control->setText(text);
    }
    return *this;
}

flUI& flUI::Padding(int x, int y)
{
    if (this->_control != nullptr)
    {
        this->_control->setPadding(glm::vec2(float(x), float(y)));
    }
    return *this;
}

flUI& flUI::UseFont(Font& font)
{
    if (this->_control != nullptr) this->_control->setFont(font);

    return *this;
}

flUI& flUI::UseColor(int r, int g,  int b, int a)
{
    if (this->_control != nullptr) this->_control->setColor(glm::vec4(float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f, float(a) / 255.0f));

    return *this;
}

flUI& flUI::OnClick(std::function<void (const Control*)> func)
{
    Button* btn = dynamic_cast<Button*>(this->_control);

    if (btn != nullptr) btn->onClick(func);

    return *this;
}

flUI& flUI::Size(int x, int y)
{
    if (this->_control != nullptr) this->_control->setSize(glm::vec2(float(x), float(y)));

    return *this;
}

flUI& flUI::AddChild(Control* control)
{
    Container* container = dynamic_cast<Container*>(this->_control);

    if (container != nullptr && control != nullptr)
    {
        control->setParent(container);
        container->addControl(control);
    }

    return *this;
}

flUI::operator Control* ()
{
    return this->_control;
}

Control* flUI::AddControl()
{
    Container* container = dynamic_cast<Container*>(this->_control);

    if (container != nullptr) UI::Manager().addControl(container);

    return this->_control;
}
