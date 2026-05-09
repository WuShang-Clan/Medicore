#include "Button.h"
#include "Helpers.h"   // your existing copyStr / strLength

// ============================================================
//  Colour palette  (matches GuiMain dark theme)
// ============================================================
namespace Theme {
    // backgrounds
    static const sf::Color BG_DEEP(10, 18, 35);
    static const sf::Color BG_PANEL(16, 28, 54);
    static const sf::Color BG_CARD(22, 38, 70);

    // accents
    static const sf::Color TEAL(0, 210, 180);
    static const sf::Color TEAL_DIM(0, 150, 130);
    static const sf::Color TEAL_DARK(0, 80, 70);

    // secondary
    static const sf::Color SLATE(80, 110, 160);
    static const sf::Color SLATE_DIM(50, 75, 115);
    static const sf::Color SLATE_DARK(30, 50, 90);

    // danger
    static const sf::Color RED(220, 70, 70);
    static const sf::Color RED_DIM(170, 50, 50);
    static const sf::Color RED_DARK(80, 25, 25);

    // ghost
    static const sf::Color TRANSPARENT(0, 0, 0, 0);
    static const sf::Color GHOST_HOVER(22, 38, 70, 200);

    // text
    static const sf::Color TEXT_LIGHT(220, 235, 255);
    static const sf::Color TEXT_MUTED(110, 140, 180);
    static const sf::Color TEXT_DARK(10, 18, 35);
}

// ============================================================
//  Private helpers
// ============================================================
bool Button::containsPoint(sf::Vector2i pt) const
{
    return pt.x >= (int)m_x && pt.x <= (int)(m_x + m_w)
        && pt.y >= (int)m_y && pt.y <= (int)(m_y + m_h);
}

void Button::applyTheme()
{
    switch (m_style)
    {
    case PRIMARY:
        m_fillNormal = Theme::TEAL_DARK;
        m_fillHover = Theme::TEAL_DIM;
        m_outlineNormal = Theme::TEAL;
        m_outlineHover = Theme::TEAL;
        m_textCol = Theme::TEXT_LIGHT;
        break;

    case SECONDARY:
        m_fillNormal = Theme::SLATE_DARK;
        m_fillHover = Theme::SLATE_DIM;
        m_outlineNormal = Theme::SLATE;
        m_outlineHover = Theme::SLATE;
        m_textCol = Theme::TEXT_LIGHT;
        break;

    case DANGER:
        m_fillNormal = Theme::RED_DARK;
        m_fillHover = Theme::RED_DIM;
        m_outlineNormal = Theme::RED;
        m_outlineHover = Theme::RED;
        m_textCol = Theme::TEXT_LIGHT;
        break;

    case GHOST:
        m_fillNormal = Theme::TRANSPARENT;
        m_fillHover = Theme::GHOST_HOVER;
        m_outlineNormal = Theme::SLATE;
        m_outlineHover = Theme::TEAL;
        m_textCol = Theme::TEXT_MUTED;
        break;
    }
}

void Button::syncShape()
{
    // shape
    m_shape.setPosition(m_x, m_y);
    m_shape.setSize(sf::Vector2f(m_w, m_h));
    m_shape.setOutlineThickness(1.5f);

    bool active = m_hovered && m_enabled;
    m_shape.setFillColor(active ? m_fillHover : m_fillNormal);
    m_shape.setOutlineColor(active ? m_outlineHover : m_outlineNormal);

    // text
    m_text.setFont(*m_font);
    m_text.setString(m_label);
    m_text.setCharacterSize(m_fontSize);
    m_text.setFillColor(m_enabled ? m_textCol : Theme::TEXT_MUTED);

    // centre text inside button
    sf::FloatRect tb = m_text.getLocalBounds();
    m_text.setOrigin(tb.left + tb.width / 2.f,
        tb.top + tb.height / 2.f);
    m_text.setPosition(m_x + m_w / 2.f,
        m_y + m_h / 2.f);
}

Button::Button()
    : m_font(nullptr), m_x(0), m_y(0), m_w(120), m_h(40),
    m_fontSize(16), m_style(PRIMARY), m_hovered(false), m_enabled(true)
{
    m_label[0] = '\0';
    applyTheme();
}

Button::Button(float x, float y, float w, float h,
    const char* label, sf::Font& font,
    Style style, unsigned int fontSize)
    : m_font(&font), m_x(x), m_y(y), m_w(w), m_h(h),
    m_fontSize(fontSize), m_style(style),
    m_hovered(false), m_enabled(true)
{
    copyStr(m_label, label, 128);
    applyTheme();
    syncShape();
}

void Button::setPosition(float x, float y) { m_x = x; m_y = y; syncShape(); }
void Button::setSize(float w, float h) { m_w = w; m_h = h; syncShape(); }
void Button::setFontSize(unsigned int size) { m_fontSize = size; syncShape(); }

void Button::setLabel(const char* label)
{
    copyStr(m_label, label, 128);
    syncShape();
}

void Button::setStyle(Style style)
{
    m_style = style;
    applyTheme();
    syncShape();
}

void Button::setEnabled(bool enabled)
{
    m_enabled = enabled;
    syncShape();
}

void Button::update(sf::Vector2i mousePos)
{
    bool wasHovered = m_hovered;
    m_hovered = m_enabled && containsPoint(mousePos);
    if (m_hovered != wasHovered) syncShape();   // only rebuild when state changes
}

bool Button::handleEvent(const sf::Event& ev, sf::Vector2i mousePos)
{
    if (!m_enabled) return false;
    if (ev.type == sf::Event::MouseButtonReleased &&
        ev.mouseButton.button == sf::Mouse::Left &&
        containsPoint(mousePos))
    {
        return true;
    }
    return false;
}

void Button::draw(sf::RenderWindow& window)
{
    window.draw(m_shape);
    if (m_font) window.draw(m_text);
}