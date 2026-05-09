#define _CRT_SECURE_NO_WARNINGS

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Helpers.h"
#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "FileHandler.h"
#include "Validator.h"
#include "HospitalException.h"
#include "FileNotFoundException.h"
#include "InsufficientFundsException.h"
#include "InvalidInputException.h"
#include "SlotUnavailableException.h"
#include "Button.h"
#include "InputField.h"

#include <iomanip>
#include <cstring>
#include <cstdio>
#include <ctime>

using namespace std;
using namespace sf;

//  Window constants
static const unsigned int WIN_W = 1280;
static const unsigned int WIN_H = 720;
static const char* TITLE = "MediCore  |  Hospital Management System";
static const char* FONT_PATH = "arial.ttf";

//MUSIC
sf::Music g_music;
bool g_musicInit = false;

sf::SoundBuffer g_errorBuffer;
sf::Sound g_errorSound;
bool g_errorInit = false;

void playErrorSound() {
    if (g_errorInit) {
        g_errorSound.play();
    }
}

//  File paths
static const char* FILE_PATIENTS = "data/patients.txt";
static const char* FILE_DOCTORS = "data/doctors.txt";
static const char* FILE_ADMIN = "data/admin.txt";
static const char* FILE_APPOINTMENTS = "data/appointments.txt";
static const char* FILE_BILLS = "data/bills.txt";
static const char* FILE_PRESCRIPTIONS = "data/prescriptions.txt";
static const char* FILE_SECURITY = "data/security_log.txt";
static const char* FILE_DISCHARGED = "data/discharged.txt";

//  In-memory stores 
static Storage<Patient>       patients;
static Storage<Doctor>        doctors;
static Storage<Appointment>   appointments;
static Storage<Bill>          bills;
static Storage<Prescription>  prescriptions;
static Admin                  adminUser;

//  Screen enum is every distinct ui view
enum Screen {
    // shared
    SCR_MAIN_MENU = 0,
    SCR_ERROR,

    // patient
    SCR_PATIENT_LOGIN,
    SCR_PATIENT_MENU,
    SCR_BOOK_APPT,
    SCR_CANCEL_APPT,
    SCR_VIEW_APPTS,
    SCR_VIEW_RECORDS,
    SCR_VIEW_BILLS,
    SCR_PAY_BILL,
    SCR_TOPUP,

    // doctor
    SCR_DOCTOR_LOGIN,
    SCR_DOCTOR_MENU,
    SCR_DOC_TODAY,
    SCR_DOC_COMPLETE,
    SCR_DOC_NOSHOW,
    SCR_DOC_PRESCRIBE,
    SCR_DOC_HISTORY,

    // admin
    SCR_ADMIN_LOGIN,
    SCR_ADMIN_MENU,
    SCR_ADMIN_ADD_DOCTOR,
    SCR_ADMIN_REMOVE_DOCTOR,
    SCR_ADMIN_ALL_PATIENTS,
    SCR_ADMIN_ALL_DOCTORS,
    SCR_ADMIN_ALL_APPTS,
    SCR_ADMIN_UNPAID_BILLS,
    SCR_ADMIN_DISCHARGE,
    SCR_ADMIN_SECURITY,
    SCR_ADMIN_REPORT,
    SCR_ADMIN_ADD_PATIENT,
    SCR_ADMIN_REMOVE_PATIENT,

    SCR_COUNT
};

//  Global states for ui
static Screen   g_screen = SCR_MAIN_MENU;
static Patient* g_patient = nullptr;
static Doctor* g_doctor = nullptr;
static char     g_errorMsg[256] = {};
static Screen   g_errorReturn = SCR_MAIN_MENU;

// For screens that show a scrollable list we keep a simple offset
static int      g_scrollOffset = 0;

//  Theme helperss
namespace C {
    Color BG_DEEP(8, 15, 30);
    Color BG_PANEL(14, 24, 50);
    Color BG_CARD(20, 34, 65);
    Color TEAL(0, 210, 180);
    Color TEAL_DIM(0, 150, 130);
    Color SLATE(80, 110, 160);
    Color RED(220, 70, 70);
    Color TEXT_HI(220, 235, 255);
    Color TEXT_MID(140, 170, 210);
    Color TEXT_DIM(80, 110, 160);
    Color DIVIDER(30, 50, 90);
}

static void drawDivider(sf::RenderWindow& win, float y)
{
    sf::RectangleShape line(sf::Vector2f((float)WIN_W - 80.f, 1.f));
    line.setPosition(40.f, y);
    line.setFillColor(C::DIVIDER);
    win.draw(line);
}

static void showError(const char* msg, Screen returnTo)
{
    int i = 0;
    while (msg[i] && i < 255) { g_errorMsg[i] = msg[i]; i++; }
    g_errorMsg[i] = '\0';
    g_errorReturn = returnTo;
    g_screen = SCR_ERROR;
}

//  Tiny utility: making sf::text with common settings
static sf::Text makeText(const char* str, sf::Font& font,
    unsigned int size, sf::Color col,
    float x, float y)
{
    sf::Text t;
    t.setFont(font);
    t.setString(str);
    t.setCharacterSize(size);
    t.setFillColor(col);
    t.setPosition(x, y);
    return t;
}

static void centreX(sf::Text& t, float left, float width)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top);
    t.setPosition(left + width / 2.f, t.getPosition().y);
}

static void drawTopBar(sf::RenderWindow& win, sf::Font& font)
{
    // background strip
    sf::RectangleShape bar(sf::Vector2f((float)WIN_W, 56.f));
    bar.setFillColor(C::BG_PANEL);
    bar.setOutlineThickness(0.f);
    win.draw(bar);

    // bottom accent line
    sf::RectangleShape line(sf::Vector2f((float)WIN_W, 2.f));
    line.setPosition(0.f, 54.f);
    line.setFillColor(C::TEAL_DIM);
    win.draw(line);

    // logo / title
    Text logo = makeText("MEDICORE", font, 22, C::TEAL, 36.f, 14.f);
    win.draw(logo);

    Text sub = makeText("Hospital Management System", font, 13, C::TEXT_DIM, 160.f, 20.f);
    win.draw(sub);

    // showing logged-in user on right side
    if (g_patient) {
        char buf[128];
        buf[0] = '\0';

        const char* name = g_patient->getName();
        int i = 0;
        while (name[i]) { buf[i] = name[i]; i++; }
        const char* suffix = "  |  Patient";
        int j = 0;
        while (suffix[j]) { buf[i++] = suffix[j++]; }
        buf[i] = '\0';
        Text uTxt = makeText(buf, font, 14, C::TEXT_MID, 0.f, 19.f);
        FloatRect b = uTxt.getLocalBounds();
        uTxt.setPosition(WIN_W - b.width - 36.f, 19.f);
        win.draw(uTxt);
    }
    else if (g_doctor) {
        char buf[128];
        const char* name = g_doctor->getName();
        int i = 0;
        while (name[i]) { buf[i] = name[i]; i++; }
        const char* suffix = "  |  Doctor";
        int j = 0;
        while (suffix[j]) { buf[i++] = suffix[j++]; }
        buf[i] = '\0';
        Text uTxt = makeText(buf, font, 14, C::TEXT_MID, 0.f, 19.f);
        FloatRect b = uTxt.getLocalBounds();
        uTxt.setPosition(WIN_W - b.width - 36.f, 19.f);
        win.draw(uTxt);
    }
}

static void drawCard(sf::RenderWindow& win,
    float x, float y, float w, float h,
    Color fill = C::BG_CARD)
{
    RectangleShape card(Vector2f(w, h));
    card.setPosition(x, y);
    card.setFillColor(fill);
    card.setOutlineThickness(1.f);
    card.setOutlineColor(C::DIVIDER);
    win.draw(card);
}

static void loadAllData()
{
    try {
        if (!FileHandler::loadPatients(patients, FILE_PATIENTS))     throw FileNotFoundException(FILE_PATIENTS);
        if (!FileHandler::loadDoctors(doctors, FILE_DOCTORS))      throw FileNotFoundException(FILE_DOCTORS);
        if (!FileHandler::loadAdmin(adminUser, FILE_ADMIN))        throw FileNotFoundException(FILE_ADMIN);
        if (!FileHandler::loadAppointments(appointments, FILE_APPOINTMENTS)) throw FileNotFoundException(FILE_APPOINTMENTS);
        if (!FileHandler::loadBills(bills, FILE_BILLS))        throw FileNotFoundException(FILE_BILLS);
        if (!FileHandler::loadPrescriptions(prescriptions, FILE_PRESCRIPTIONS))throw FileNotFoundException(FILE_PRESCRIPTIONS);
    }
    catch (FileNotFoundException& e) {
        (void)e;
    }
}

//  ScreenData — per-screen local state bundles
struct LoginState {
    InputField  idField;
    InputField  passField;
    Button      loginBtn;
    Button      backBtn;
    char        errorMsg[128];
    bool        dirty;

    void init(sf::Font& font, const char* title) {
        (void)title;
        float cx = WIN_W / 2.f;
        idField = InputField(cx - 180.f, 280.f, 360.f, 48.f, "ID (numbers only)", font, false, 16, 10);
        passField = InputField(cx - 180.f, 350.f, 360.f, 48.f, "Password", font, true, 16, 50);
        loginBtn = Button(cx - 180.f, 430.f, 360.f, 48.f, "LOGIN", font, Button::PRIMARY, 16);
        backBtn = Button(cx - 180.f, 496.f, 360.f, 40.f, "Back", font, Button::GHOST, 14);
        errorMsg[0] = '\0';
        dirty = false;
    }
};

//  SCR_MAIN_MENU
static Screen drawMainMenu(sf::RenderWindow& win, sf::Font& font,
    const Event& ev, sf::Vector2i mouse,
    Button buttons[3])
{
    RectangleShape bg(Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP);
    win.draw(bg);

    drawTopBar(win, font);

    Text hero = makeText("Welcome to MediCore", font, 42, C::TEXT_HI, 0.f, 130.f);
    centreX(hero, 0.f, (float)WIN_W);
    win.draw(hero);

    Text sub = makeText("Integrated Hospital Management Platform", font, 18, C::TEXT_DIM, 0.f, 186.f);
    centreX(sub, 0.f, (float)WIN_W);
    win.draw(sub);

    RectangleShape accent(Vector2f(80.f, 3.f));
    accent.setFillColor(C::TEAL);
    accent.setPosition(WIN_W / 2.f - 40.f, 220.f);
    win.draw(accent);

    // three role cards — positioned in a row
    float cardW = 280.f, cardH = 180.f;
    float gap = 40.f;
    float totalW = 3.f * cardW + 2.f * gap;
    float startX = (WIN_W - totalW) / 2.f;
    float cardY = 280.f;

    const char* roles[3] = { "Patient",       "Doctor",        "Admin" };
    const char* descs[3] = { "Book & manage\nappointments",
                                  "View schedule\n& prescribe",
                                  "Manage staff\n& reports" };
    Button::Style styles[3] = { Button::PRIMARY, Button::SECONDARY, Button::GHOST };

    Screen next = SCR_MAIN_MENU;

    for (int i = 0; i < 3; i++) {
        float cx = startX + i * (cardW + gap);

        drawCard(win, cx, cardY, cardW, cardH);

        // role label
        Text rTxt = makeText(roles[i], font, 22, C::TEXT_HI, 0.f, cardY + 28.f);
        centreX(rTxt, cx, cardW);
        win.draw(rTxt);

        const char* d = descs[i];
        float dy = cardY + 64.f;
        char line[64]; int li = 0;
        for (int ci = 0; ; ci++) {
            if (d[ci] == '\n' || d[ci] == '\0') {
                line[li] = '\0';
                sf::Text dTxt = makeText(line, font, 14, C::TEXT_DIM, 0.f, dy);
                centreX(dTxt, cx, cardW);
                win.draw(dTxt);
                dy += 22.f; li = 0;
                if (d[ci] == '\0') break;
            }
            else {
                line[li++] = d[ci];
            }
        }

        // button inside card
        buttons[i].setPosition(cx + 40.f, cardY + cardH - 56.f);
        buttons[i].setSize(cardW - 80.f, 40.f);
        buttons[i].setLabel(roles[i]);
        buttons[i].setStyle(styles[i]);
        buttons[i].update(mouse);
        if (buttons[i].handleEvent(ev, mouse)) {
            if (i == 0) next = SCR_PATIENT_LOGIN;
            if (i == 1) next = SCR_DOCTOR_LOGIN;
            if (i == 2) next = SCR_ADMIN_LOGIN;
        }
        buttons[i].draw(win);
    }

    Text footer = makeText("MediCore v1.0  |  BY HAMMAD ANIS", font, 12, C::TEXT_DIM,
        0.f, (float)WIN_H - 28.f);
    centreX(footer, 0.f, (float)WIN_W);
    win.draw(footer);

    return next;
}

//  SCR_ERROR
static Screen drawError(RenderWindow& win, Font& font,
    const sf::Event& ev, Vector2i mouse,
    Button& okBtn)
{
    RectangleShape bg(Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP);
    win.draw(bg);
    drawTopBar(win, font);

    float cx = WIN_W / 2.f;
    drawCard(win, cx - 280.f, 200.f, 560.f, 260.f);

    Text title = makeText("Error", font, 28, C::RED, 0.f, 230.f);
    centreX(title, cx - 280.f, 560.f);
    win.draw(title);
    Text msg = makeText(g_errorMsg, font, 16, C::TEXT_MID, 0.f, 290.f);
    centreX(msg, cx - 280.f, 560.f);
    win.draw(msg);

    okBtn.setPosition(cx - 80.f, 380.f);
    okBtn.setSize(160.f, 44.f);
    okBtn.setLabel("OK");
    okBtn.update(mouse);
    okBtn.draw(win);

    if (okBtn.handleEvent(ev, mouse)) return g_errorReturn;
    return SCR_ERROR;
}

//  Generic login screen (Patient / Doctor / Admin)
static Screen drawLoginScreen(sf::RenderWindow& win, sf::Font& font,
    const Event& ev, sf::Vector2i mouse,
    LoginState& ls, const char* roleTitle,
    Screen onSuccess, Screen onBack)
{
    RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP);
    win.draw(bg);
    drawTopBar(win, font);

    float cx = WIN_W / 2.f;
    drawCard(win, cx - 220.f, 180.f, 440.f, 380.f);

    Text title = makeText(roleTitle, font, 26, C::TEXT_HI, 0.f, 210.f);
    centreX(title, cx - 220.f, 440.f);
    win.draw(title);

    Text sub = makeText("Enter your credentials", font, 14, C::TEXT_DIM, 0.f, 248.f);
    centreX(sub, cx - 220.f, 440.f);
    win.draw(sub);

    if (ls.errorMsg[0] != '\0') {
        Text err = makeText(ls.errorMsg, font, 13, C::RED, 0.f, 418.f);
        centreX(err, cx - 220.f, 440.f);
        win.draw(err);
    }

    ls.idField.update();
    ls.passField.update();
    ls.idField.handleEvent(ev, mouse);
    ls.passField.handleEvent(ev, mouse);
    ls.idField.draw(win);
    ls.passField.draw(win);

    ls.loginBtn.update(mouse);
    ls.backBtn.update(mouse);
    ls.loginBtn.draw(win);
    ls.backBtn.draw(win);

    if (ls.backBtn.handleEvent(ev, mouse)) {
        ls.idField.clear();
        ls.passField.clear();
        ls.errorMsg[0] = '\0';
        return onBack;
    }

    // login attempt
    if (ls.loginBtn.handleEvent(ev, mouse) ||
        (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Return))
    {
        ls.dirty = true;
        // parse ID
        const char* idStr = ls.idField.getText();
        int id = 0;
        bool validId = (ls.idField.getLength() > 0);
        for (int i = 0; idStr[i]; i++) {
            if (idStr[i] < '0' || idStr[i] > '9') { validId = false; break; }
            id = id * 10 + (idStr[i] - '0');
        }
        if (!validId) {
            const char* e = "Please enter a valid numeric ID.";
            playErrorSound();
            int i = 0; while (e[i]) { ls.errorMsg[i] = e[i]; i++; } ls.errorMsg[i] = '\0';
            return SCR_COUNT; // stay — caller ignores SCR_COUNT
        }
        return onSuccess;     
    }
    return SCR_COUNT;         
}

//  SCR_PATIENT_MENU
static Screen drawPatientMenu(sf::RenderWindow& win, sf::Font& font,
    const Event& ev, Vector2i mouse,
    Button menuBtns[8])
{
    RectangleShape bg(Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP);
    win.draw(bg);
    drawTopBar(win, font);

    RectangleShape sidebar(Vector2f(260.f, (float)WIN_H - 56.f));
    sidebar.setPosition(0.f, 56.f);
    sidebar.setFillColor(C::BG_PANEL);
    win.draw(sidebar);

    drawCard(win, 16.f, 72.f, 228.f, 110.f);
    Text nameT = makeText(g_patient->getName(), font, 16, C::TEXT_HI, 28.f, 84.f);
    win.draw(nameT);
    Text roleT = makeText("Patient Account", font, 12, C::TEXT_DIM, 28.f, 108.f);
    win.draw(roleT);

    char balBuf[64];
    float bal = g_patient->getBalance();
    int whole = (int)bal;
    int frac = (int)((bal - whole) * 100.f);
    if (frac < 0) frac = -frac;
    // simple itoa-like
    char tmp[32]; int ti = 0;
    if (whole == 0) { tmp[ti++] = '0'; }
    else { int w2 = whole; char rev[16]; int ri = 0; while (w2) { rev[ri++] = ('0' + w2 % 10); w2 /= 10; } for (int r = ri - 1; r >= 0; r--) tmp[ti++] = rev[r]; }
    tmp[ti] = '\0';
    // build balBuf
    const char* prefix = "PKR ";
    int bi = 0;
    for (int i = 0; prefix[i]; i++) balBuf[bi++] = prefix[i];
    for (int i = 0; tmp[i]; i++)    balBuf[bi++] = tmp[i];
    balBuf[bi++] = '.';
    balBuf[bi++] = '0' + (frac / 10);
    balBuf[bi++] = '0' + (frac % 10);
    balBuf[bi] = '\0';

    Text balT = makeText(balBuf, font, 15, C::TEAL, 28.f, 130.f);
    win.draw(balT);

    const char* labels[8] = {
        "Book Appointment",
        "Cancel Appointment",
        "My Appointments",
        "Medical Records",
        "My Bills",
        "Pay Bill",
        "Top Up Balance",
        "Logout"
    };
    Button::Style styles[8] = {
        Button::PRIMARY, Button::PRIMARY, Button::SECONDARY,
        Button::SECONDARY, Button::SECONDARY, Button::PRIMARY,
        Button::GHOST, Button::DANGER
    };

    Screen targets[8] = {
        SCR_BOOK_APPT, SCR_CANCEL_APPT, SCR_VIEW_APPTS,
        SCR_VIEW_RECORDS, SCR_VIEW_BILLS, SCR_PAY_BILL,
        SCR_TOPUP, SCR_MAIN_MENU
    };

    float btnY = 200.f;
    for (int i = 0; i < 8; i++) {
        menuBtns[i].setPosition(16.f, btnY);
        menuBtns[i].setSize(228.f, 42.f);
        menuBtns[i].setLabel(labels[i]);
        menuBtns[i].setStyle(styles[i]);
        menuBtns[i].update(mouse);
        menuBtns[i].draw(win);
        if (menuBtns[i].handleEvent(ev, mouse)) {
            if (targets[i] == SCR_MAIN_MENU) { g_patient = nullptr; }
            return targets[i];
        }
        btnY += 50.f;
    }

    // main content area — welcome message
    Text wMsg = makeText("Select an option from the menu.", font, 18, C::TEXT_MID, 310.f, 300.f);
    win.draw(wMsg);

    return SCR_PATIENT_MENU;
}

//  SCR_DOCTOR_MENU
static Screen drawDoctorMenu(RenderWindow& win, Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    Button menuBtns[6])
{
    RectangleShape bg(Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP);
    win.draw(bg);
    drawTopBar(win, font);

    RectangleShape sidebar(sf::Vector2f(260.f, (float)WIN_H - 56.f));
    sidebar.setPosition(0.f, 56.f);
    sidebar.setFillColor(C::BG_PANEL);
    win.draw(sidebar);

    drawCard(win, 16.f, 72.f, 228.f, 90.f);
    Text nameT = makeText(g_doctor->getName(), font, 16, C::TEXT_HI, 28.f, 84.f);
    win.draw(nameT);
    Text specT = makeText(g_doctor->getSpecialization(), font, 12, C::TEAL, 28.f, 108.f);
    win.draw(specT);

    const char* labels[6] = {
        "Today's Appointments",
        "Mark Complete",
        "Mark No-Show",
        "Write Prescription",
        "Patient History",
        "Logout"
    };
    Button::Style styles[6] = {
        Button::PRIMARY, Button::PRIMARY, Button::SECONDARY,
        Button::PRIMARY, Button::SECONDARY, Button::DANGER
    };
    Screen targets[6] = {
        SCR_DOC_TODAY, SCR_DOC_COMPLETE, SCR_DOC_NOSHOW,
        SCR_DOC_PRESCRIBE, SCR_DOC_HISTORY, SCR_MAIN_MENU
    };

    float btnY = 185.f;
    for (int i = 0; i < 6; i++) {
        menuBtns[i].setPosition(16.f, btnY);
        menuBtns[i].setSize(228.f, 42.f);
        menuBtns[i].setLabel(labels[i]);
        menuBtns[i].setStyle(styles[i]);
        menuBtns[i].update(mouse);
        menuBtns[i].draw(win);
        if (menuBtns[i].handleEvent(ev, mouse)) {
            if (targets[i] == SCR_MAIN_MENU) { g_doctor = nullptr; }
            return targets[i];
        }
        btnY += 50.f;
    }

    Text wMsg = makeText("Select an option from the menu.", font, 18, C::TEXT_MID, 310.f, 300.f);
    win.draw(wMsg);
    return SCR_DOCTOR_MENU;
}

//  SCR_ADMIN_MENU
static Screen drawAdminMenu(sf::RenderWindow& win, sf::Font& font,
    const Event& ev, Vector2i mouse,
    Button menuBtns[12])
{
    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP);
    win.draw(bg);
    drawTopBar(win, font);
    RectangleShape sidebar(Vector2f(260.f, (float)WIN_H - 56.f));
    sidebar.setPosition(0.f, 56.f);
    sidebar.setFillColor(C::BG_PANEL);
    win.draw(sidebar);
    drawCard(win, 16.f, 72.f, 228.f, 60.f);
    sf::Text nameT = makeText("Administrator", font, 16, C::TEAL, 28.f, 88.f);
    win.draw(nameT);

    const char* labels[12] = {
        "Add Doctor",        "Remove Doctor",
        "All Patients",      "All Doctors",
        "All Appointments",  "Unpaid Bills",
        "Discharge Patient", "Security Log",
        "Daily Report",      "Add Patient",
        "Remove Patient",    "Logout"
    };
    Button::Style styles[12];
    for (int i = 0; i < 11; i++) styles[i] = Button::SECONDARY;
    styles[0] = Button::PRIMARY;
    styles[9] = Button::PRIMARY;
    styles[11] = Button::DANGER;

    Screen targets[12] = {
        SCR_ADMIN_ADD_DOCTOR,    SCR_ADMIN_REMOVE_DOCTOR,
        SCR_ADMIN_ALL_PATIENTS,  SCR_ADMIN_ALL_DOCTORS,
        SCR_ADMIN_ALL_APPTS,     SCR_ADMIN_UNPAID_BILLS,
        SCR_ADMIN_DISCHARGE,     SCR_ADMIN_SECURITY,
        SCR_ADMIN_REPORT,        SCR_ADMIN_ADD_PATIENT,
        SCR_ADMIN_REMOVE_PATIENT,SCR_MAIN_MENU
    };

    float btnY = 148.f;
    for (int i = 0; i < 12; i++) {
        menuBtns[i].setPosition(16.f, btnY);
        menuBtns[i].setSize(228.f, 38.f);
        menuBtns[i].setLabel(labels[i]);
        menuBtns[i].setStyle(styles[i]);
        menuBtns[i].update(mouse);
        menuBtns[i].draw(win);
        if (menuBtns[i].handleEvent(ev, mouse)) {
            return targets[i];
        }
        btnY += 44.f;
    }

    Text wMsg = makeText("Select an option from the panel.", font, 18, C::TEXT_MID, 310.f, 300.f);
    win.draw(wMsg);
    return SCR_ADMIN_MENU;
}

//  SCR_VIEW_APPTS  — patient's appointment list
static Screen drawViewAppointments(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    Button& backBtn)
{
    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP);
    win.draw(bg);
    drawTopBar(win, font);

    Text title = makeText("My Appointments", font, 26, C::TEXT_HI, 60.f, 80.f);
    win.draw(title);
    drawDivider(win, 118.f);

    // column headers
    float colX[6] = { 60.f, 140.f, 330.f, 540.f, 690.f, 860.f };
    const char* headers[6] = { "ID", "Doctor", "Specialization", "Date", "Slot", "Status" };
    for (int i = 0; i < 6; i++) {
        Text h = makeText(headers[i], font, 13, C::TEAL, colX[i], 128.f);
        win.draw(h);
    }
    drawDivider(win, 150.f);

    // collecting and sortin the patients
    int idx[200]; int cnt = 0;
    for (int i = 0; i < appointments.size() && cnt < 200; i++)
        if (appointments[i].getPatientId() == g_patient->getId())
            idx[cnt++] = i;

    // bubble sort used
    for (int i = 0; i < cnt - 1; i++)
        for (int j = 0; j < cnt - i - 1; j++)
            if (compareDates(appointments[idx[j]].getDate(),
                appointments[idx[j + 1]].getDate()) > 0)
            {
                int t = idx[j]; idx[j] = idx[j + 1]; idx[j + 1] = t;
            }

    float rowY = 162.f;
    int maxRows = 18;
    int startRow = g_scrollOffset;
    if (startRow > cnt - maxRows) startRow = cnt - maxRows;
    if (startRow < 0) startRow = 0;

    for (int i = startRow; i < cnt && i < startRow + maxRows; i++) {
        Appointment& a = appointments[idx[i]];
        Doctor* doc = doctors.findById(a.getDoctorId());

        Color rowCol = (i % 2 == 0) ? C::BG_CARD : C::BG_PANEL;
        RectangleShape row(sf::Vector2f((float)WIN_W - 80.f, 26.f));
        row.setPosition(40.f, rowY);
        row.setFillColor(rowCol);
        win.draw(row);

        char idBuf[16]; int iv = a.getId(), ii = 0;
        if (iv == 0) { idBuf[ii++] = '0'; }
        else { char r[12]; int ri = 0; while (iv) { r[ri++] = '0' + iv % 10; iv /= 10; } for (int k = ri - 1; k >= 0; k--) idBuf[ii++] = r[k]; }
        idBuf[ii] = '\0';

        Color statusCol = C::TEXT_MID;
        if (a.isCompleted()) statusCol = C::TEAL;
        if (a.isCancelled()) statusCol = C::RED;

        win.draw(makeText(idBuf, font, 13, C::TEXT_MID, colX[0], rowY + 5.f));
        win.draw(makeText(doc ? doc->getName() : "Unknown", font, 13, C::TEXT_HI, colX[1], rowY + 5.f));
        win.draw(makeText(doc ? doc->getSpecialization() : "?", font, 13, C::TEXT_MID, colX[2], rowY + 5.f));
        win.draw(makeText(a.getDate(), font, 13, C::TEXT_MID, colX[3], rowY + 5.f));
        win.draw(makeText(a.getTimeSlot(), font, 13, C::TEXT_MID, colX[4], rowY + 5.f));
        win.draw(makeText(a.getStatus(), font, 13, statusCol, colX[5], rowY + 5.f));

        rowY += 28.f;
    }
    if (cnt == 0) {
        Text empty = makeText("No appointments found.", font, 16, C::TEXT_DIM, 0.f, 300.f);
        centreX(empty, 0.f, (float)WIN_W);
        win.draw(empty);
    }

    if (cnt > maxRows) {
        Text scroll = makeText("Scroll: Up/Down arrow keys", font, 12, C::TEXT_DIM,
            (float)WIN_W - 260.f, (float)WIN_H - 32.f);
        win.draw(scroll);
    }

    backBtn.setPosition(60.f, (float)WIN_H - 60.f);
    backBtn.setSize(140.f, 40.f);
    backBtn.setLabel("< Back");
    backBtn.setStyle(Button::GHOST);
    backBtn.update(mouse);
    backBtn.draw(win);
    if (backBtn.handleEvent(ev, mouse)) return SCR_PATIENT_MENU;

    // keyboard scroll feature
    if (ev.type == Event::KeyPressed) {
        if (ev.key.code == Keyboard::Down) g_scrollOffset++;
        if (ev.key.code == Keyboard::Up && g_scrollOffset > 0) g_scrollOffset--;
    }

    return SCR_VIEW_APPTS;
}

#include "AdminScreens.h"
#include "PatientScreens.h"
#include "DoctorScreens.h"

int main()
{
    // ---- window -----
    RenderWindow window(
        VideoMode(WIN_W, WIN_H),
        TITLE,
        Style::Titlebar | Style::Close
    );
    window.setFramerateLimit(60);

    if (!g_musicInit) {
        if (!g_music.openFromFile("Background.ogg")) {
        }
        else {
            g_music.setLoop(true);
            g_music.setVolume(40);     
            g_music.play();
        }
        g_musicInit = true;
    }

    if (!g_errorInit) {
        if (!g_errorBuffer.loadFromFile("FAHH.ogg")) {
        }
        else {
            g_errorSound.setBuffer(g_errorBuffer);
            g_errorSound.setVolume(200);
        }
        g_errorInit = true;
    }

    // ---- font ----
    Font font;
    if (!font.loadFromFile(FONT_PATH)) {
        if (!font.loadFromFile("assets/arial.ttf") &&
            !font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            return 1;
        }
    }
  
    loadAllData();

    Button mainBtns[3];
    for (int i = 0; i < 3; i++)
        mainBtns[i] = Button(0, 0, 10, 10, "", font);

    Button errOkBtn(0, 0, 10, 10, "OK", font);

    LoginState patientLogin, doctorLogin, adminLogin;
    patientLogin.init(font, "Patient Login");
    doctorLogin.init(font, "Doctor Login");
    adminLogin.init(font, "Admin Login");
    int loginAttempts = 0;  // tracks failed attempts per login session

    // Patient menu
    Button patMenuBtns[8];
    for (int i = 0; i < 8; i++)
        patMenuBtns[i] = Button(0, 0, 10, 10, "", font);

    // Doctor menu
    Button docMenuBtns[6];
    for (int i = 0; i < 6; i++)
        docMenuBtns[i] = Button(0, 0, 10, 10, "", font);

    // Admin menu
    Button adminMenuBtns[12];
    for (int i = 0; i < 12; i++)
        adminMenuBtns[i] = Button(0, 0, 10, 10, "", font);

    Button genericBack(0, 0, 10, 10, "< Back", font, Button::GHOST);

    bool adminAddDocReady = false;
    bool adminRemDocReady = false;
    bool adminAddPatReady = false;
    bool adminRemPatReady = false;
    bool adminDischargeReady = false;

    // ---- main loop ----
    while (window.isOpen())
    {
        Event event;
        event.type = Event::Count; 
        Vector2i mouse = Mouse::getPosition(window);

        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed &&
                event.key.code == Keyboard::LControl) {
                g_patient = nullptr;
                g_doctor = nullptr;
                g_screen = SCR_MAIN_MENU;
                loginAttempts = 0;
            }
        }
        window.clear(C::BG_DEEP);
        Screen next = g_screen;

        switch (g_screen)
        {
            // ---- main menu -----
        case SCR_MAIN_MENU:
            next = drawMainMenu(window, font, event, mouse, mainBtns);
            if (next != SCR_MAIN_MENU) {
                loginAttempts = 0;
                patientLogin.idField.clear();
                patientLogin.passField.clear();
                patientLogin.errorMsg[0] = '\0';
                doctorLogin.idField.clear();
                doctorLogin.passField.clear();
                doctorLogin.errorMsg[0] = '\0';
                adminLogin.idField.clear();
                adminLogin.passField.clear();
                adminLogin.errorMsg[0] = '\0';
            }
            break;

        case SCR_ERROR:
            next = drawError(window, font, event, mouse, errOkBtn);
            break;

            // ---- patient login -----
        case SCR_PATIENT_LOGIN: {
            Screen r = drawLoginScreen(window, font, event, mouse,
                patientLogin, "Patient Login",
                SCR_PATIENT_MENU, SCR_MAIN_MENU);
            if (r == SCR_PATIENT_MENU) {
                // attempt authentication
                const char* idStr = patientLogin.idField.getText();
                int id = 0;
                for (int i = 0; idStr[i]; i++) id = id * 10 + (idStr[i] - '0');
                Patient* p = patients.findById(id);
                if (p && strEquals(p->getPassword(), patientLogin.passField.getText())) {
                    g_patient = p;
                    loginAttempts = 0;
                    next = SCR_PATIENT_MENU;
                }
                else {
                    playErrorSound();
                    loginAttempts++;
                    char buf[80] = "Invalid credentials. Attempts: ";
                    buf[31] = '0' + loginAttempts;
                    buf[32] = '/'; buf[33] = '3'; buf[34] = '\0';
                    int i = 0;
                    while (buf[i]) { patientLogin.errorMsg[i] = buf[i]; i++; }
                    patientLogin.errorMsg[i] = '\0';

                    char idBuf[10]; intToStr(id, idBuf, 10);
                    FileHandler::logSecurityEvent("Patient", idBuf, "FAILED", FILE_SECURITY);
                    if (loginAttempts >= 3) {
                        showError("Account locked after 3 failed attempts.", SCR_MAIN_MENU);
                        loginAttempts = 0;
                    }
                }
            }
            else if (r != SCR_COUNT) {
                next = r;
            }
            break;
        }

        case SCR_DOCTOR_LOGIN: {
            Screen r = drawLoginScreen(window, font, event, mouse,
                doctorLogin, "Doctor Login",
                SCR_DOCTOR_MENU, SCR_MAIN_MENU);
            if (r == SCR_DOCTOR_MENU) {
                const char* idStr = doctorLogin.idField.getText();
                int id = 0;
                for (int i = 0; idStr[i]; i++) id = id * 10 + (idStr[i] - '0');
                Doctor* d = doctors.findById(id);
                if (d && strEquals(d->getPassword(), doctorLogin.passField.getText())) {
                    g_doctor = d;
                    loginAttempts = 0;
                    next = SCR_DOCTOR_MENU;
                }
                else {
                    playErrorSound();
                    loginAttempts++;
                    char buf[80] = "Invalid credentials. Attempts: ";
                    buf[31] = '0' + loginAttempts;
                    buf[32] = '/'; buf[33] = '3'; buf[34] = '\0';
                    int i = 0;
                    while (buf[i]) { doctorLogin.errorMsg[i] = buf[i]; i++; }
                    doctorLogin.errorMsg[i] = '\0';

                    char idBuf[10]; intToStr(id, idBuf, 10);
                    FileHandler::logSecurityEvent("Doctor", idBuf, "FAILED", FILE_SECURITY);
                    if (loginAttempts >= 3) {
                        showError("Account locked after 3 failed attempts.", SCR_MAIN_MENU);
                        loginAttempts = 0;
                    }
                }
            }
            else if (r != SCR_COUNT) {
                next = r;
            }
            break;
        }

        case SCR_ADMIN_LOGIN: {
            Screen r = drawLoginScreen(window, font, event, mouse,
                adminLogin, "Admin Login",
                SCR_ADMIN_MENU, SCR_MAIN_MENU);
            if (r == SCR_ADMIN_MENU) {
                const char* idStr = adminLogin.idField.getText();
                int id = 0;
                for (int i = 0; idStr[i]; i++) id = id * 10 + (idStr[i] - '0');
                if (adminUser.getId() == id &&
                    strEquals(adminUser.getPassword(), adminLogin.passField.getText())) {
                    loginAttempts = 0;
                    next = SCR_ADMIN_MENU;
                }
                else {
                    playErrorSound();
                    loginAttempts++;
                    char buf[80] = "Invalid credentials. Attempts: ";
                    buf[31] = '0' + loginAttempts;
                    buf[32] = '/'; buf[33] = '3'; buf[34] = '\0';
                    int i = 0;
                    while (buf[i]) { adminLogin.errorMsg[i] = buf[i]; i++; }
                    adminLogin.errorMsg[i] = '\0';

                    char idBuf[10]; intToStr(id, idBuf, 10);
                    FileHandler::logSecurityEvent("Admin", idBuf, "FAILED", FILE_SECURITY);
                    if (loginAttempts >= 3) {
                        showError("Account locked after 3 failed attempts.", SCR_MAIN_MENU);
                        loginAttempts = 0;
                    }
                }
            }
            else if (r != SCR_COUNT) {
                next = r;
            }
            break;
        }

        case SCR_PATIENT_MENU:
            next = drawPatientMenu(window, font, event, mouse, patMenuBtns);
            if (next != SCR_PATIENT_MENU) g_scrollOffset = 0;
            break;

        case SCR_DOCTOR_MENU:
            next = drawDoctorMenu(window, font, event, mouse, docMenuBtns);
            break;

        case SCR_ADMIN_MENU:
            next = drawAdminMenu(window, font, event, mouse, adminMenuBtns);
            break;

        case SCR_VIEW_APPTS:
            next = drawViewAppointments(window, font, event, mouse, genericBack);
            break;

        case SCR_BOOK_APPT:
         next = drawBookAppt(window, font, event, mouse);
         break;
        case SCR_CANCEL_APPT:
         next = drawCancelAppt(window, font, event, mouse);
         break;
        case SCR_VIEW_RECORDS:
         next = drawViewRecords(window, font, event, mouse, genericBack);
         break;
        case SCR_VIEW_BILLS:
         next = drawViewBills(window, font, event, mouse, genericBack);
         break;
        case SCR_PAY_BILL:
         next = drawPayBill(window, font, event, mouse);
         break;
        case SCR_TOPUP:
         next = drawTopUp(window, font, event, mouse);
         break;

        case SCR_DOC_TODAY:
           next = drawDocToday(window, font, event, mouse, genericBack);
           break;
        case SCR_DOC_COMPLETE:
           next = drawDocMark(window, font, event, mouse, "completed", SCR_DOC_COMPLETE);
           break;
        case SCR_DOC_NOSHOW:
           next = drawDocMark(window, font, event, mouse, "no-show", SCR_DOC_NOSHOW);
           break;
        case SCR_DOC_PRESCRIBE:
           next = drawDocPrescribe(window, font, event, mouse);
           break;
        case SCR_DOC_HISTORY:
           next = drawDocHistory(window, font, event, mouse);
           break;

        case SCR_ADMIN_ADD_DOCTOR:
            next = drawAdminAddDoctor(window, font, event, mouse, adminAddDocReady);
            break;
        case SCR_ADMIN_REMOVE_DOCTOR:
            next = drawAdminRemoveDoctor(window, font, event, mouse, adminRemDocReady);
            break;
        case SCR_ADMIN_ALL_PATIENTS:
            next = drawAdminAllPatients(window, font, event, mouse, genericBack);
            break;
        case SCR_ADMIN_ALL_DOCTORS:
            next = drawAdminAllDoctors(window, font, event, mouse, genericBack);
            break;
        case SCR_ADMIN_ALL_APPTS:
            next = drawAdminAllAppts(window, font, event, mouse, genericBack);
            break;
        case SCR_ADMIN_UNPAID_BILLS:
            next = drawAdminUnpaidBills(window, font, event, mouse, genericBack);
            break;
        case SCR_ADMIN_DISCHARGE:
            next = drawAdminDischarge(window, font, event, mouse, adminDischargeReady);
            break;
        case SCR_ADMIN_SECURITY:
            next = drawAdminSecurity(window, font, event, mouse, genericBack);
            break;
        case SCR_ADMIN_REPORT:
            next = drawAdminReport(window, font, event, mouse, genericBack);
            break;
        case SCR_ADMIN_ADD_PATIENT:
            next = drawAdminAddPatient(window, font, event, mouse, adminAddPatReady);
            break;
        case SCR_ADMIN_REMOVE_PATIENT:
            next = drawAdminRemovePatient(window, font, event, mouse, adminRemPatReady);
            break;

        default:
            next = SCR_MAIN_MENU;
            break;
        }

         if (next != SCR_COUNT && next != g_screen) {
             // admin resets
             if (g_screen == SCR_ADMIN_ADD_DOCTOR)    { adminAddDocReady    = false; clearUIMsg(); }
             if (g_screen == SCR_ADMIN_REMOVE_DOCTOR) { adminRemDocReady    = false; clearUIMsg(); }
             if (g_screen == SCR_ADMIN_ADD_PATIENT)   { adminAddPatReady    = false; clearUIMsg(); }
             if (g_screen == SCR_ADMIN_REMOVE_PATIENT){ adminRemPatReady    = false; clearUIMsg(); }
             if (g_screen == SCR_ADMIN_DISCHARGE)     { adminDischargeReady = false; clearUIMsg(); }
             // patient resets
             if (g_screen == SCR_BOOK_APPT)   { g_book.reset(); g_bookStateInit = false; clearUIMsg(); }
             if (g_screen == SCR_CANCEL_APPT) { g_cancelField.clear(); g_cancelFieldInit = false; clearUIMsg(); }
             if (g_screen == SCR_PAY_BILL)    { g_payField.clear();    g_payFieldInit    = false; clearUIMsg(); }
             if (g_screen == SCR_TOPUP)       { g_topupField.clear();  g_topupFieldInit  = false; clearUIMsg(); }
             // doctor resets
             if (g_screen == SCR_DOC_COMPLETE || g_screen == SCR_DOC_NOSHOW)
                                              { g_markField.clear();   g_markFieldInit   = false; clearUIMsg(); }
             if (g_screen == SCR_DOC_PRESCRIBE){ g_prescribe.clear();  g_prescribeInit   = false; clearUIMsg(); }
             if (g_screen == SCR_DOC_HISTORY) { g_histField.clear();   g_histFieldInit   = false;
             g_histPatientId = -1;                    clearUIMsg();
             }
             g_screen = next;
             g_scrollOffset = 0;
             g_adminScroll  = 0;
         }
        window.display();
    }
}