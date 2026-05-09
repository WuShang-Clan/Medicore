#pragma once
#include <SFML/Graphics.hpp>

class Button
{
public:
    enum Style { PRIMARY, SECONDARY, DANGER, GHOST };

private:
    sf::RectangleShape  m_shape;
    sf::Text            m_text;
    sf::Font* m_font;        
    char                m_label[128];

    float               m_x, m_y, m_w, m_h;
    unsigned int        m_fontSize;
    Style               m_style;
    bool                m_hovered;
    bool                m_enabled;

    sf::Color           m_fillNormal;
    sf::Color           m_fillHover;
    sf::Color           m_outlineNormal;
    sf::Color           m_outlineHover;
    sf::Color           m_textCol;

    void applyTheme();
    void syncShape();
    bool containsPoint(sf::Vector2i pt) const;

public:
    Button();
    Button(float x, float y, float w, float h,
        const char* label, sf::Font& font,
        Style style = PRIMARY,
        unsigned int fontSize = 16);

    void setPosition(float x, float y);
    void setSize(float w, float h);
    void setLabel(const char* label);
    void setStyle(Style style);
    void setEnabled(bool enabled);
    void setFontSize(unsigned int size);

    // call every frame
    void update(sf::Vector2i mousePos);

    // call inside pollEvent loop — returns true on left-click release
    bool handleEvent(const sf::Event& ev, sf::Vector2i mousePos);

    void draw(sf::RenderWindow& window);

    bool        isHovered()  const { return m_hovered; }
    bool        isEnabled()  const { return m_enabled; }
    const char* getLabel()   const { return m_label; }
    float       getX()       const { return m_x; }
    float       getY()       const { return m_y; }
    float       getWidth()   const { return m_w; }
    float       getHeight()  const { return m_h; }
};