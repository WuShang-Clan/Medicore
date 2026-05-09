#include "InputField.h"
#include "Helpers.h"

namespace FieldTheme {
    static const sf::Color BG_NORMAL(16, 28, 54);
    static const sf::Color BG_ACTIVE(22, 38, 70);
    static const sf::Color OUT_NORMAL(50, 75, 115);
    static const sf::Color OUT_ACTIVE(0, 210, 180);
    static const sf::Color TEXT_COL(220, 235, 255);
    static const sf::Color PLACEHOLDER(80, 110, 160);
    static const sf::Color CURSOR_COL(0, 210, 180);
}

bool InputField::containsPoint(sf::Vector2i pt) const
{
    return pt.x >= (int)m_x && pt.x <= (int)(m_x + m_w)
        && pt.y >= (int)m_y && pt.y <= (int)(m_y + m_h);
}
void InputField::syncShape()
{
    m_box.setPosition(m_x, m_y);
    m_box.setSize(sf::Vector2f(m_w, m_h));
    m_box.setOutlineThickness(1.5f);
    m_box.setFillColor(m_active ? m_colBoxActive : m_colBoxNormal);
    m_box.setOutlineColor(m_active ? m_colOutlineActive : m_colOutlineNormal);

    // placeholder
    m_placeholderText.setFont(*m_font);
    m_placeholderText.setString(m_placeholder);
    m_placeholderText.setCharacterSize(m_fontSize);
    m_placeholderText.setFillColor(m_colPlaceholder);
    m_placeholderText.setPosition(m_x + 12.f, m_y + (m_h - m_fontSize) / 2.f - 2.f);

    // cursor bar
    m_cursor.setSize(sf::Vector2f(2.f, (float)m_fontSize + 4.f));
    m_cursor.setFillColor(m_colCursor);
}

void InputField::rebuildDisplayText()
{
    if (!m_font) return;
    static char display[512];
    if (m_isPassword) {
        for (int i = 0; i < m_length; i++) display[i] = '*';
        display[m_length] = '\0';
    }
    else {
        copyStr(display, m_buffer, 512);
    }

    m_displayText.setFont(*m_font);
    m_displayText.setString(display);
    m_displayText.setCharacterSize(m_fontSize);
    m_displayText.setFillColor(m_colText);
    m_displayText.setPosition(m_x + 12.f, m_y + (m_h - m_fontSize) / 2.f - 2.f);

    // position cursor just after last character
    sf::FloatRect tb = m_displayText.getLocalBounds();
    float cursorX = m_x + 12.f + tb.width + 2.f;
    float cursorY = m_y + (m_h - (float)m_fontSize - 4.f) / 2.f;
    m_cursor.setPosition(cursorX, cursorY);
}

//  Constructors
InputField::InputField()
    : m_font(nullptr), m_length(0), m_x(0), m_y(0), m_w(200), m_h(44),
    m_fontSize(16), m_active(false), m_isPassword(false),
    m_cursorVisible(true), m_maxLength(255),
    m_colBoxNormal(FieldTheme::BG_NORMAL),
    m_colBoxActive(FieldTheme::BG_ACTIVE),
    m_colOutlineNormal(FieldTheme::OUT_NORMAL),
    m_colOutlineActive(FieldTheme::OUT_ACTIVE),
    m_colText(FieldTheme::TEXT_COL),
    m_colPlaceholder(FieldTheme::PLACEHOLDER),
    m_colCursor(FieldTheme::CURSOR_COL)
{
    m_buffer[0] = '\0';
    m_placeholder[0] = '\0';
}

InputField::InputField(float x, float y, float w, float h,
    const char* placeholder, sf::Font& font,
    bool isPassword, unsigned int fontSize, int maxLength)
    : m_font(&font), m_length(0), m_x(x), m_y(y), m_w(w), m_h(h),
    m_fontSize(fontSize), m_active(false), m_isPassword(isPassword),
    m_cursorVisible(true), m_maxLength(maxLength),
    m_colBoxNormal(FieldTheme::BG_NORMAL),
    m_colBoxActive(FieldTheme::BG_ACTIVE),
    m_colOutlineNormal(FieldTheme::OUT_NORMAL),
    m_colOutlineActive(FieldTheme::OUT_ACTIVE),
    m_colText(FieldTheme::TEXT_COL),
    m_colPlaceholder(FieldTheme::PLACEHOLDER),
    m_colCursor(FieldTheme::CURSOR_COL)
{
    m_buffer[0] = '\0';
    copyStr(m_placeholder, placeholder, 128);
    if (m_maxLength >= 256) m_maxLength = 255;
    syncShape();
    rebuildDisplayText();
}
//  Mutators
void InputField::setPosition(float x, float y) { m_x = x; m_y = y; syncShape(); rebuildDisplayText(); }
void InputField::setSize(float w, float h) { m_w = w; m_h = h; syncShape(); rebuildDisplayText(); }

void InputField::setActive(bool active)
{
    m_active = active;
    m_cursorClock.restart();
    m_cursorVisible = true;
    syncShape();
}

void InputField::clear()
{
    m_buffer[0] = '\0';
    m_length = 0;
    rebuildDisplayText();
}

void InputField::setText(const char* text)
{
    copyStr(m_buffer, text, 256);
    m_length = 0;
    while (m_buffer[m_length]) m_length++;
    if (m_length > m_maxLength) { m_length = m_maxLength; m_buffer[m_length] = '\0'; }
    rebuildDisplayText();
}

void InputField::setPlaceholder(const char* ph)
{
    copyStr(m_placeholder, ph, 128);
    syncShape();
}

void InputField::setMaxLength(int max)
{
    m_maxLength = (max < 255) ? max : 255;
}

void InputField::update()
{
    if (m_active && m_cursorClock.getElapsedTime().asMilliseconds() > 530) {
        m_cursorVisible = !m_cursorVisible;
        m_cursorClock.restart();
    }
}
//  Event handling
void InputField::handleEvent(const sf::Event& ev, sf::Vector2i mousePos)
{
    // click to activate / deactivate
    if (ev.type == sf::Event::MouseButtonPressed &&
        ev.mouseButton.button == sf::Mouse::Left)
    {
        bool wasActive = m_active;
        m_active = containsPoint(mousePos);
        if (m_active != wasActive) {
            m_cursorClock.restart();
            m_cursorVisible = true;
            syncShape();
        }
        return;
    }
 
    if (!m_active) return;

    if (ev.type == sf::Event::TextEntered)
    {
        if (ev.type == sf::Event::TextEntered)
        {
            sf::Uint32 c = ev.text.unicode;

            //  BACKSPACE
            if (c == 8)
            {
                if (m_length > 0) {
                    m_length--;
                    m_buffer[m_length] = '\0';
                    rebuildDisplayText();
                }
                return;
            }

            // ignore other control keys
            if (c == 127 || c == 27 || c == 13 || c == 9) return;

            if (c < 32 || c > 126) return;
            if (m_length >= m_maxLength) return;

            m_buffer[m_length] = (char)c;
            m_length++;
            m_buffer[m_length] = '\0';
            rebuildDisplayText();
        }
    }

}
void InputField::draw(sf::RenderWindow& window)
{
    window.draw(m_box);

    if (m_length == 0 && !m_active) {
        window.draw(m_placeholderText);
    }
    else {
        window.draw(m_displayText);
        if (m_active && m_cursorVisible)
            window.draw(m_cursor);
    }
}