#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <iostream>
using namespace std;

//  STRING UTILITIES
inline int strLength(const char* s) {
    if (!s) return 0;
    int i = 0;
    while (s[i]) i++;
    return i;
}

inline void copyStr(char* dest, const char* src, int size) {
    if (!src) { dest[0] = '\0'; return; }
    int i = 0;
    while (i < size - 1 && src[i] != 0) { dest[i] = src[i]; i++; }
    dest[i] = '\0';
}

// Append src onto dst (dst has capacity cap).
inline void strAppend(char* dst, const char* src, int cap) {
    int di = strLength(dst);
    int si = 0;
    while (src[si] && di < cap - 1) dst[di++] = src[si++];
    dst[di] = '\0';
}

inline bool strEquals(const char* a, const char* b) {
    if (!a || !b) return false;
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return false;
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

inline bool strEqualsIC(const char* a, const char* b) {
    if (!a || !b) return false;
    int i = 0;
    while (a[i] && b[i]) {
        char ca = a[i], cb = b[i];
        if (ca >= 'A' && ca <= 'Z') ca = ca - 'A' + 'a';
        if (cb >= 'A' && cb <= 'Z') cb = cb - 'A' + 'a';
        if (ca != cb) return false;
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

inline void nextToken(const char* src, char delim, int& offset, char* token, int maxLen) {
    int i = 0;
    while (src[offset] && src[offset] != delim && i < maxLen - 1)
        token[i++] = src[offset++];
    token[i] = '\0';
    if (src[offset] == delim) offset++;
}

//  NUMBER <-> STRING CONVERSION
inline void intToStr(int val, char* buf, int bufLen) {
    if (bufLen <= 0) return;
    if (val == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    bool neg = (val < 0);
    if (neg) val = -val;
    char tmp[20]; int i = 0;
    while (val > 0) { tmp[i++] = '0' + (val % 10); val /= 10; }
    if (neg) tmp[i++] = '-';
    int j = 0;
    for (int k = i - 1; k >= 0 && j < bufLen - 1; k--) buf[j++] = tmp[k];
    buf[j] = '\0';
}

inline float strToFloat(const char* s) {
    if (!s) return 0.0f;
    float result = 0.0f;
    int i = 0;
    bool neg = false;
    if (s[0] == '-') { neg = true; i = 1; }
    while (s[i] >= '0' && s[i] <= '9') result = result * 10 + (s[i++] - '0');
    if (s[i] == '.') {
        i++;
        float dec = 0.1f;
        while (s[i] >= '0' && s[i] <= '9') {
            result += (s[i++] - '0') * dec;
            dec *= 0.1f;
        }
    }
    return neg ? -result : result;
}

inline int strToInt(const char* s) {
    if (!s) return 0;
    int result = 0, i = 0;
    bool neg = false;
    if (s[0] == '-') { neg = true; i = 1; }
    while (s[i] >= '0' && s[i] <= '9') result = result * 10 + (s[i++] - '0');
    return neg ? -result : result;
}

// Parses a non-negative integer from s into out.
inline bool parseInt(const char* s, int& out) {
    out = 0;
    if (!s || !s[0]) return false;
    for (int i = 0; s[i]; i++) {
        if (s[i] < '0' || s[i] > '9') return false;
        out = out * 10 + (s[i] - '0');
    }
    return true;
}

// Parses a non-negative decimal number from s into out.
inline bool parseFloat(const char* s, float& out) {
    out = 0.f;
    if (!s || !s[0]) return false;
    bool dot = false;
    int  dec = 0;
    for (int i = 0; s[i]; i++) {
        if (s[i] == '.' && !dot) { dot = true; continue; }
        if (s[i] < '0' || s[i] > '9') return false;
        if (dot) { dec++; out += (s[i] - '0') / (dec == 1 ? 10.f : 100.f); }
        else      out = out * 10.f + (s[i] - '0');
    }
    return true;
}

static void ftoaBuf(float v, char* buf, int bufSize) {
    int whole = (int)v;
    int frac = (int)((v - (float)whole) * 100.f + 0.5f);
    if (frac >= 100) { whole++; frac = 0; }
    char tmp[32]; intToStr(whole, tmp, 32);
    int bi = 0;
    for (int i = 0; tmp[i] && bi < bufSize - 1; i++) buf[bi++] = tmp[i];
    if (bi < bufSize - 1) buf[bi++] = '.';
    if (bi < bufSize - 1) buf[bi++] = '0' + frac / 10;
    if (bi < bufSize - 1) buf[bi++] = '0' + frac % 10;
    buf[bi] = '\0';
}

inline bool isNumeric(const char* s) {
    if (!s || s[0] == '\0') return false;
    for (int i = 0; s[i]; i++)
        if (s[i] < '0' || s[i] > '9') return false;
    return true;
}

//  DATE UTILITIES
inline void safeLocaltime(const time_t* t, tm* result) {
#if defined(_MSC_VER)
    localtime_s(result, t);
#else
    localtime_r(t, result);
#endif
}

inline void getTodayDate(char* buf, int bufLen) {
    time_t now = time(nullptr);
    tm ltm;
    safeLocaltime(&now, &ltm);
    strftime(buf, bufLen, "%d-%m-%Y", &ltm);
}

// Dates must be in DD-MM-YYYY format.
inline int compareDates(const char* d1, const char* d2) {
    for (int k = 6; k <= 9; k++) {      
        if (d1[k] > d2[k]) return  1;
        if (d1[k] < d2[k]) return -1;
    }
    for (int k = 3; k <= 4; k++) {      
        if (d1[k] > d2[k]) return  1;
        if (d1[k] < d2[k]) return -1;
    }
    for (int k = 0; k <= 1; k++) {      
        if (d1[k] > d2[k]) return  1;
        if (d1[k] < d2[k]) return -1;
    }
    return 0;
}

//  CONSOLE I/O HELPERS
inline void readLine(char* buf, int maxLen) {
    int i = 0;
    char c;
    while (i < maxLen - 1 && std::cin.get(c) && c != '\n')
        buf[i++] = c;
    buf[i] = '\0';
}

inline int readInt() {
    char buf[20];
    readLine(buf, 20);
    return strToInt(buf);
}

inline float readFloat() {
    char buf[30];
    readLine(buf, 30);
    return strToFloat(buf);
}

inline void printDivider(int w) {
    for (int i = 0; i < w; i++) std::cout << '=';
    std::cout << "\n";
}

inline void printDiv(int w) { printDivider(w); } 

// ============================================================
//  SFML GUI SHARED UTILITIES
#include "SFML/Graphics.hpp"

//  Feedback message (one unified system for all role screens)
static char g_uiMsg[256] = {};
static bool g_uiMsgIsError = false;

inline void setUIMsg(const char* msg, bool isError = false) {
    int i = 0;
    while (msg[i] && i < 255) { g_uiMsg[i] = msg[i]; i++; }
    g_uiMsg[i] = '\0';
    g_uiMsgIsError = isError;
}
inline void clearUIMsg() { g_uiMsg[0] = '\0'; }


inline void drawUIMsg(sf::RenderWindow& win, sf::Font& font,
    float x = 60.f, float wide = 0.f)
{
    if (!g_uiMsg[0]) return;
    extern sf::Color _uiMsgTeal;
    sf::Color c = g_uiMsgIsError
        ? sf::Color(220, 70, 70)
        : sf::Color(0, 210, 180);

    float barW = (wide > 0.f ? wide : (float)win.getSize().x - x - 8.f);
    sf::RectangleShape bar(sf::Vector2f(barW, 30.f));
    bar.setPosition(x, 58.f);
    bar.setFillColor(sf::Color(c.r, c.g, c.b, 30));
    bar.setOutlineThickness(1.f);
    bar.setOutlineColor(sf::Color(c.r, c.g, c.b, 80));
    win.draw(bar);

    sf::Text txt;
    txt.setFont(font);
    txt.setString(g_uiMsg);
    txt.setCharacterSize(13);
    txt.setFillColor(c);
    txt.setPosition(x + 10.f, 65.f);
    win.draw(txt);
}

//  Unified table helpers
inline void drawTableHeader(sf::RenderWindow& win, sf::Font& font,
    const float* colX, const char** labels,
    int numCols, float startY,
    float rowLeft = 40.f, float rowWidth = 0.f)
{
    float rw = rowWidth > 0.f
        ? rowWidth
        : (float)win.getSize().x - rowLeft - 40.f;

    sf::RectangleShape hdrBg(sf::Vector2f(rw, 26.f));
    hdrBg.setPosition(rowLeft, startY);
    hdrBg.setFillColor(sf::Color(0, 60, 55, 180));
    win.draw(hdrBg);

    sf::Text t;
    t.setFont(font);
    t.setCharacterSize(13);
    t.setFillColor(sf::Color(0, 210, 180)); 
    for (int i = 0; i < numCols; i++) {
        t.setString(labels[i]);
        t.setPosition(colX[i], startY + 6.f);
        win.draw(t);
    }
}

inline void drawTableRow(sf::RenderWindow& win, sf::Font& font,
    const float* colX, const char** vals,
    const sf::Color* cols, int numCols,
    float y, int rowIdx,
    float rowLeft = 40.f, float rowWidth = 0.f)
{
    float rw = rowWidth > 0.f
        ? rowWidth
        : (float)win.getSize().x - rowLeft - 40.f;

    sf::Color bg = (rowIdx % 2 == 0)
        ? sf::Color(20, 34, 65)    // C::BG_CARD
        : sf::Color(14, 24, 50);   // C::BG_PANEL

    sf::RectangleShape rowBg(sf::Vector2f(rw, 26.f));
    rowBg.setPosition(rowLeft, y);
    rowBg.setFillColor(bg);
    win.draw(rowBg);

    sf::Text t;
    t.setFont(font);
    t.setCharacterSize(13);
    for (int i = 0; i < numCols; i++) {
        t.setString(vals[i]);
        t.setFillColor(cols[i]);
        t.setPosition(colX[i], y + 5.f);
        win.draw(t);
    }
}

//  Unified scroll handler
inline void handleScroll(const sf::Event& ev, int totalRows,
    int visibleRows, int& scrollOffset)
{
    if (ev.type != sf::Event::KeyPressed) return;
    if (ev.key.code == sf::Keyboard::Down) {
        if (scrollOffset < totalRows - visibleRows) scrollOffset++;
    }
    if (ev.key.code == sf::Keyboard::Up) {
        if (scrollOffset > 0) scrollOffset--;
    }
}

//  Unified inline back button
inline bool drawBackButton(sf::RenderWindow& win, sf::Font& font,
    sf::Vector2i mouse, const sf::Event& ev,
    float x = 60.f)
{
    unsigned int winH = win.getSize().y;
    float btnY = (float)winH - 54.f;

    bool hover = (mouse.x >= (int)x && mouse.x <= (int)(x + 130) &&
        mouse.y >= (int)btnY && mouse.y <= (int)(btnY + 34));

    sf::RectangleShape btn(sf::Vector2f(130.f, 34.f));
    btn.setPosition(x, btnY);
    btn.setFillColor(hover
        ? sf::Color(20, 34, 65)         // C::BG_CARD on hover
        : sf::Color(0, 0, 0, 0));       
    btn.setOutlineThickness(1.f);
    btn.setOutlineColor(sf::Color(80, 110, 160));   // C::SLATE
    win.draw(btn);

    sf::Text label;
    label.setFont(font);
    label.setString("< Back");
    label.setCharacterSize(13);
    label.setFillColor(sf::Color(140, 170, 210));   // C::TEXT_MID
    label.setPosition(x + 14.f, btnY + 8.f);
    win.draw(label);

    return hover
        && ev.type == sf::Event::MouseButtonReleased
        && ev.mouseButton.button == sf::Mouse::Left;
}


