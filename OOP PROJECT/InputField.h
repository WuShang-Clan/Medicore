#pragma once
#include <SFML/Graphics.hpp>

class InputField
{
private:
    sf::RectangleShape  m_box;
    sf::Text            m_displayText;   
    sf::Text            m_placeholderText;
    sf::RectangleShape  m_cursor;
    sf::Font* m_font; 

    char                m_buffer[256];   // actual typed text
    int                 m_length;        // current char count

    char                m_placeholder[128];
    float               m_x, m_y, m_w, m_h;
    unsigned int        m_fontSize;

    bool                m_active;
    bool                m_isPassword;
    bool                m_cursorVisible;
    sf::Clock           m_cursorClock;

    int                 m_maxLength;

    // colours
    sf::Color           m_colBoxNormal;
    sf::Color           m_colBoxActive;
    sf::Color           m_colOutlineNormal;
    sf::Color           m_colOutlineActive;
    sf::Color           m_colText;
    sf::Color           m_colPlaceholder;
    sf::Color           m_colCursor;

    void syncShape();
    void rebuildDisplayText();
    bool containsPoint(sf::Vector2i pt) const;

public:
    InputField();
    InputField(float x, float y, float w, float h,
        const char* placeholder, sf::Font& font,
        bool isPassword = false,
        unsigned int fontSize = 16,
        int maxLength = 255);

    // mutators
    void setPosition(float x, float y);
    void setSize(float w, float h);
    void setActive(bool active);
    void clear();
    void setText(const char* text);
    void setPlaceholder(const char* ph);
    void setMaxLength(int max);

    void update();

    // call inside pollEvent loop
    void handleEvent(const sf::Event& ev, sf::Vector2i mousePos);

    void draw(sf::RenderWindow& window);

    const char* getText()     const { return m_buffer; }
    int         getLength()   const { return m_length; }
    bool        isActive()    const { return m_active; }
    bool        isEmpty()     const { return m_length == 0; }
    float       getX()        const { return m_x; }
    float       getY()        const { return m_y; }
    float       getWidth()    const { return m_w; }
    float       getHeight()   const { return m_h; }
};