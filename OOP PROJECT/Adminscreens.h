#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "Helpers.h"

static int g_adminScroll = 0;

static inline void adminDrawMsg(sf::RenderWindow& win, sf::Font& font) {
    drawUIMsg(win, font, 300.f, (float)(WIN_W - 320));
}

static inline void adminTableHeader(sf::RenderWindow& win, sf::Font& font,
    const float* colX, const char** labels, int numCols, float y)
{
    drawTableHeader(win, font, colX, labels, numCols, y, 300.f, (float)(WIN_W - 320));
}

static inline void adminTableRow(sf::RenderWindow& win, sf::Font& font,
    const float* colX, const char** vals,
    const sf::Color* cols, int numCols,
    float y, int rowIdx)
{
    drawTableRow(win, font, colX, vals, cols, numCols, y, rowIdx, 300.f, (float)(WIN_W - 320));
}

static inline void adminHandleScroll(const sf::Event& ev, int total, int visible) {
    handleScroll(ev, total, visible, g_adminScroll);
}


static void drawInfoRow(sf::RenderWindow& win, sf::Font& font,
    float x, float y,
    const char* label, const char* value,
    sf::Color valCol = C::TEXT_HI)
{
    win.draw(makeText(label, font, 14, C::TEXT_DIM, x, y));
    win.draw(makeText(value, font, 14, valCol, x + 160.f, y));
}

static void drawAdminContentHeader(sf::RenderWindow& win, sf::Font& font,
    const char* title)
{
    sf::RectangleShape area(sf::Vector2f((float)WIN_W - 280.f, (float)WIN_H - 56.f));
    area.setPosition(270.f, 56.f);
    area.setFillColor(C::BG_DEEP);
    win.draw(area);

    win.draw(makeText(title, font, 24, C::TEXT_HI, 300.f, 76.f));

    sf::RectangleShape accent(sf::Vector2f(40.f, 3.f));
    accent.setPosition(300.f, 108.f);
    accent.setFillColor(C::TEAL);
    win.draw(accent);
}

//  1. SCR_ADMIN_ALL_PATIENTS
static Screen drawAdminAllPatients(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    Button& backBtn)
{
    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawAdminContentHeader(win, font, "All Patients");

    char countBuf[32] = "Total: ";
    char nb[16]; intToStr(patients.size(), nb, 16);
    strAppend(countBuf, nb, 32);
    win.draw(makeText(countBuf, font, 13, C::TEXT_DIM, (float)WIN_W - 200.f, 80.f));

    float colX[6] = { 305.f, 365.f, 520.f, 620.f, 720.f, 860.f };
    const char* hdr[6] = { "ID", "Name", "Age", "Gender", "Contact", "Balance (PKR)" };
    adminTableHeader(win, font, colX, hdr, 6, 160.f);

    int total = patients.size(), visible = 17;
    float rowY = 190.f;

    for (int i = g_adminScroll; i < total && i < g_adminScroll + visible; i++) {
        Patient& p = patients[i];
        char idB[12], ageB[8], balB[20];
        intToStr(p.getId(), idB, 12);
        intToStr(p.getAge(), ageB, 8);
        ftoaBuf(p.getBalance(), balB, 20);

        const char* vals[6] = { idB, p.getName(), ageB, p.getGender(), p.getContact(), balB };
        sf::Color   cols[6];
        for (int c = 0; c < 5; c++) cols[c] = C::TEXT_MID;
        cols[5] = (p.getBalance() < 500.f) ? C::RED : C::TEAL;

        adminTableRow(win, font, colX, vals, cols, 6, rowY, i);
        rowY += 27.f;
    }

    if (total == 0) {
        sf::Text e = makeText("No patients registered.", font, 16, C::TEXT_DIM, 0.f, 350.f);
        centreX(e, 270.f, (float)WIN_W - 270.f); win.draw(e);
    }
    adminHandleScroll(ev, total, visible);
    if (total > visible) {
        char sh[64] = "Row ";
        char a[8], b[8], tot[8];
        intToStr(g_adminScroll + 1, a, 8);
        intToStr(g_adminScroll + visible, b, 8);
        intToStr(total, tot, 8);
        strAppend(sh, a, 64); strAppend(sh, "-", 64); strAppend(sh, b, 64);
        strAppend(sh, " of ", 64); strAppend(sh, tot, 64);
        win.draw(makeText(sh, font, 12, C::TEXT_DIM, (float)WIN_W - 260.f, (float)WIN_H - 30.f));
    }
    backBtn.setPosition(300.f, (float)WIN_H - 58.f);
    backBtn.setSize(130.f, 38.f);
    backBtn.setLabel("< Back");
    backBtn.setStyle(Button::GHOST);
    backBtn.update(mouse); backBtn.draw(win);
    if (backBtn.handleEvent(ev, mouse)) { g_adminScroll = 0; return SCR_ADMIN_MENU; }
    return SCR_ADMIN_ALL_PATIENTS;
}

//  2. SCR_ADMIN_ALL_DOCTORS
static Screen drawAdminAllDoctors(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    Button& backBtn)
{
    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawAdminContentHeader(win, font, "All Doctors");

    float colX[5] = { 305.f, 370.f, 560.f, 730.f, 900.f };
    const char* hdr[5] = { "ID", "Name", "Specialization", "Contact", "Fee (PKR)" };
    adminTableHeader(win, font, colX, hdr, 5, 160.f);

    int total = doctors.size(), visible = 17;
    float rowY = 190.f;

    for (int i = g_adminScroll; i < total && i < g_adminScroll + visible; i++) {
        Doctor& d = doctors[i];
        char idB[12], feeB[20];
        intToStr(d.getId(), idB, 12);
        ftoaBuf(d.getFee(), feeB, 20);

        const char* vals[5] = { idB, d.getName(), d.getSpecialization(), d.getContact(), feeB };
        sf::Color   cols[5] = { C::TEXT_DIM, C::TEXT_HI, C::TEAL, C::TEXT_MID, C::TEXT_MID };
        adminTableRow(win, font, colX, vals, cols, 5, rowY, i);
        rowY += 27.f;
    }

    if (total == 0) {
        sf::Text e = makeText("No doctors registered.", font, 16, C::TEXT_DIM, 0.f, 350.f);
        centreX(e, 270.f, (float)WIN_W - 270.f); win.draw(e);
    }

    adminHandleScroll(ev, total, visible);
    backBtn.setPosition(300.f, (float)WIN_H - 58.f);
    backBtn.setSize(130.f, 38.f);
    backBtn.setLabel("< Back");
    backBtn.setStyle(Button::GHOST);
    backBtn.update(mouse); backBtn.draw(win);
    if (backBtn.handleEvent(ev, mouse)) { g_adminScroll = 0; return SCR_ADMIN_MENU; }
    return SCR_ADMIN_ALL_DOCTORS;
}

//  3. SCR_ADMIN_ALL_APPTS
static Screen drawAdminAllAppts(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    Button& backBtn)
{
    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawAdminContentHeader(win, font, "All Appointments");

    static int idx[500]; int cnt = appointments.size();
    if (cnt > 500) cnt = 500;
    for (int i = 0; i < cnt; i++) idx[i] = i;
    for (int i = 0; i < cnt - 1; i++)
        for (int j = 0; j < cnt - i - 1; j++)
            if (compareDates(appointments[idx[j]].getDate(),
                appointments[idx[j + 1]].getDate()) < 0)
            {
                int t = idx[j]; idx[j] = idx[j + 1]; idx[j + 1] = t;
            }

    float colX[6] = { 305.f, 360.f, 510.f, 680.f, 820.f, 940.f };
    const char* hdr[6] = { "ID", "Patient", "Doctor", "Date", "Slot", "Status" };
    adminTableHeader(win, font, colX, hdr, 6, 160.f);

    int visible = 17;
    float rowY = 190.f;

    for (int i = g_adminScroll; i < cnt && i < g_adminScroll + visible; i++) {
        Appointment& a = appointments[idx[i]];
        Patient* pat = patients.findById(a.getPatientId());
        Doctor* doc = doctors.findById(a.getDoctorId());

        char idB[12]; intToStr(a.getId(), idB, 12);
        const char* vals[6] = {
            idB,
            pat ? pat->getName() : "Unknown",
            doc ? doc->getName() : "Unknown",
            a.getDate(), a.getTimeSlot(), a.getStatus()
        };
        sf::Color statusCol = C::TEXT_MID;
        if (a.isCompleted()) statusCol = C::TEAL;
        if (a.isCancelled()) statusCol = C::RED;
        if (a.isNoShow())    statusCol = sf::Color(200, 150, 50);

        sf::Color cols[6] = { C::TEXT_DIM, C::TEXT_HI, C::TEXT_MID,
                              C::TEXT_MID, C::TEXT_MID, statusCol };
        adminTableRow(win, font, colX, vals, cols, 6, rowY, i);
        rowY += 27.f;
    }

    if (cnt == 0) {
        sf::Text e = makeText("No appointments found.", font, 16, C::TEXT_DIM, 0.f, 350.f);
        centreX(e, 270.f, (float)WIN_W - 270.f); win.draw(e);
    }

    adminHandleScroll(ev, cnt, visible);

    backBtn.setPosition(300.f, (float)WIN_H - 58.f);
    backBtn.setSize(130.f, 38.f);
    backBtn.setLabel("< Back");
    backBtn.setStyle(Button::GHOST);
    backBtn.update(mouse); backBtn.draw(win);
    if (backBtn.handleEvent(ev, mouse)) { g_adminScroll = 0; return SCR_ADMIN_MENU; }
    return SCR_ADMIN_ALL_APPTS;
}

//  4. SCR_ADMIN_UNPAID_BILLS
static Screen drawAdminUnpaidBills(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    Button& backBtn)
{
    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawAdminContentHeader(win, font, "Unpaid Bills");

    static int idx[500]; int cnt = 0;
    for (int i = 0; i < bills.size() && cnt < 500; i++)
        if (bills[i].isUnpaid()) idx[cnt++] = i;

    float total = 0.f;
    for (int i = 0; i < cnt; i++) total += bills[idx[i]].getAmount();

    char totBuf[64] = "Outstanding: PKR ";
    char fv[20]; ftoaBuf(total, fv, 20);
    strAppend(totBuf, fv, 64);
    win.draw(makeText(totBuf, font, 14, C::RED, (float)WIN_W - 340.f, 78.f));

    float colX[5] = { 305.f, 380.f, 570.f, 740.f, 900.f };
    const char* hdr[5] = { "Bill ID", "Patient", "Amount (PKR)", "Date", "Overdue?" };
    adminTableHeader(win, font, colX, hdr, 5, 160.f);
    int visible = 17;
    float rowY = 190.f;
    time_t nowTime = time(nullptr);

    for (int i = g_adminScroll; i < cnt && i < g_adminScroll + visible; i++) {
        Bill& b = bills[idx[i]];
        Patient* pat = patients.findById(b.getPatientId());

        char idB[12], amtB[20];
        intToStr(b.getId(), idB, 12);
        ftoaBuf(b.getAmount(), amtB, 20);

        // Overdue check (>7 days)
        const char* d = b.getDate();
        tm billTm = {};
        char d2[3] = { d[0], d[1], '\0' };
        char m2[3] = { d[3], d[4], '\0' };
        char y4[5] = { d[6], d[7], d[8], d[9], '\0' };
        billTm.tm_mday = strToInt(d2);
        billTm.tm_mon = strToInt(m2) - 1;
        billTm.tm_year = strToInt(y4) - 1900;
        time_t billTime = mktime(&billTm);
        bool overdue = difftime(nowTime, billTime) / 86400.0 > 7.0;

        const char* overdueTxt = overdue ? "YES" : "no";
        const char* vals[5] = { idB, pat ? pat->getName() : "Unknown",
                                 amtB, b.getDate(), overdueTxt };
        sf::Color overdueCol = overdue ? C::RED : C::TEXT_DIM;
        sf::Color cols[5] = { C::TEXT_DIM, C::TEXT_HI, C::TEXT_MID, C::TEXT_MID, overdueCol };
        adminTableRow(win, font, colX, vals, cols, 5, rowY, i);
        rowY += 27.f;
    }

    if (cnt == 0) {
        sf::Text e = makeText("No unpaid bills.", font, 16, C::TEAL, 0.f, 350.f);
        centreX(e, 270.f, (float)WIN_W - 270.f); win.draw(e);
    }

    adminHandleScroll(ev, cnt, visible);

    backBtn.setPosition(300.f, (float)WIN_H - 58.f);
    backBtn.setSize(130.f, 38.f);
    backBtn.setLabel("< Back");
    backBtn.setStyle(Button::GHOST);
    backBtn.update(mouse); backBtn.draw(win);
    if (backBtn.handleEvent(ev, mouse)) { g_adminScroll = 0; return SCR_ADMIN_MENU; }
    return SCR_ADMIN_UNPAID_BILLS;
}

//  5. SCR_ADMIN_SECURITY
static Screen drawAdminSecurity(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    Button& backBtn)
{
    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawAdminContentHeader(win, font, "Security Log");

    static char lines[300][128];
    static int  lineCount = 0;
    static bool loaded = false;

    if (!loaded) {
        lineCount = 0;
        FILE* f = fopen(FILE_SECURITY, "r");
        if (f) {
            char buf[128];
            while (fgets(buf, 128, f) && lineCount < 300) {
                int len = strLength(buf);
                if (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r'))
                    buf[len - 1] = '\0';
                copyStr(lines[lineCount++], buf, 128);
            }
            fclose(f);
        }
        loaded = true;
    }

    int visible = 20;
    float rowY = 162.f;

    for (int i = g_adminScroll; i < lineCount && i < g_adminScroll + visible; i++) {
        bool failed = false;
        for (int k = 0; lines[i][k]; k++)
            if (lines[i][k] == 'F' && lines[i][k + 1] == 'A' && lines[i][k + 2] == 'I' &&
                lines[i][k + 3] == 'L' && lines[i][k + 4] == 'E' && lines[i][k + 5] == 'D')
            {
                failed = true; break;
            }

        win.draw(makeText(lines[i], font, 13,
            failed ? C::RED : C::TEXT_MID, 305.f, rowY));
        rowY += 24.f;
    }

    if (lineCount == 0) {
        sf::Text e = makeText("Security log is empty.", font, 16, C::TEXT_DIM, 0.f, 350.f);
        centreX(e, 270.f, (float)WIN_W - 270.f); win.draw(e);
    }

    adminHandleScroll(ev, lineCount, visible);

    backBtn.setPosition(300.f, (float)WIN_H - 58.f);
    backBtn.setSize(130.f, 38.f);
    backBtn.setLabel("< Back");
    backBtn.setStyle(Button::GHOST);
    backBtn.update(mouse); backBtn.draw(win);
    if (backBtn.handleEvent(ev, mouse)) {
        g_adminScroll = 0; loaded = false;
        return SCR_ADMIN_MENU;
    }
    return SCR_ADMIN_SECURITY;
}

//  6. SCR_ADMIN_REPORT — Daily Report
static Screen drawAdminReport(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    Button& backBtn)
{
    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);

    char today[15]; getTodayDate(today, 15);
    char titleBuf[64] = "Daily Report  -  ";
    strAppend(titleBuf, today, 64);
    drawAdminContentHeader(win, font, titleBuf);

    int total = 0, pending = 0, completed = 0, noshow = 0, cancelled = 0;
    float revenue = 0.f;
    for (int i = 0; i < appointments.size(); i++) {
        if (!strEquals(appointments[i].getDate(), today)) continue;
        total++;
        if (appointments[i].isPending())   pending++;
        else if (appointments[i].isCompleted()) completed++;
        else if (appointments[i].isNoShow())    noshow++;
        else if (appointments[i].isCancelled()) cancelled++;
    }
    for (int i = 0; i < bills.size(); i++)
        if (bills[i].isPaid() && strEquals(bills[i].getDate(), today))
            revenue += bills[i].getAmount();

    // Summary cards
    float cardY = 160.f, cardW = 160.f, cardH = 80.f, gap = 18.f, startX = 260.f;
    struct { const char* label; int value; sf::Color col; } cards[5] = {
        { "Total",     total,     C::TEXT_HI  },
        { "Pending",   pending,   sf::Color(200,150,50) },
        { "Completed", completed, C::TEAL     },
        { "No-Show",   noshow,    C::RED      },
        { "Cancelled", cancelled, C::SLATE    }
    };
    for (int i = 0; i < 5; i++) {
        float cx = startX + i * (cardW + gap);
        drawCard(win, cx, cardY, cardW, cardH, C::BG_CARD);
        char vb[16]; intToStr(cards[i].value, vb, 16);
        win.draw(makeText(vb, font, 28, cards[i].col, cx + 16.f, cardY + 10.f));
        win.draw(makeText(cards[i].label, font, 13, C::TEXT_DIM, cx + 16.f, cardY + 52.f));
    }
    // Revenue card
    float rcx = startX + 5 * (cardW + gap);
    drawCard(win, rcx, cardY, cardW + 30.f, cardH, C::BG_CARD);
    char revB[32] = "PKR "; char rv[20]; ftoaBuf(revenue, rv, 20); strAppend(revB, rv, 32);
    win.draw(makeText(revB, font, 18, C::TEAL, rcx + 12.f, cardY + 14.f));
    win.draw(makeText("Revenue", font, 13, C::TEXT_DIM, rcx + 12.f, cardY + 52.f));

    // Patients with outstanding bills
    float secY = cardY + cardH + 32.f;
    win.draw(makeText("Patients with outstanding bills:", font, 15, C::TEXT_HI, 305.f, secY));
    secY += 26.f;

    float colX2[3] = { 310.f, 560.f, 760.f };
    const char* hdr2[3] = { "Patient", "Contact", "Owed (PKR)" };
    adminTableHeader(win, font, colX2, hdr2, 3, secY);
    secY += 30.f;

    int rowIdx = 0;
    for (int i = 0; i < patients.size(); i++) {
        float owed = 0.f;
        for (int j = 0; j < bills.size(); j++)
            if (bills[j].getPatientId() == patients[i].getId() && bills[j].isUnpaid())
                owed += bills[j].getAmount();
        if (owed <= 0.f) continue;

        char owedB[20]; ftoaBuf(owed, owedB, 20);
        const char* vals[3] = { patients[i].getName(), patients[i].getContact(), owedB };
        sf::Color   cols[3] = { C::TEXT_HI, C::TEXT_MID, C::RED };
        adminTableRow(win, font, colX2, vals, cols, 3, secY, rowIdx++);
        secY += 27.f;
        if (secY > (float)WIN_H - 120.f) break;
    }
    if (rowIdx == 0) {
        win.draw(makeText("No outstanding bills today.", font, 14, C::TEAL, 310.f, secY));
        secY += 24.f;
    }

    // Doctor-wise summary
    secY += 10.f;
    if (secY < (float)WIN_H - 160.f) {
        win.draw(makeText("Doctor-wise summary (today):", font, 15, C::TEXT_HI, 305.f, secY));
        secY += 26.f;

        float colX3[4] = { 310.f, 560.f, 680.f, 800.f };
        const char* hdr3[4] = { "Doctor", "Completed", "Pending", "No-Show" };
        adminTableHeader(win, font, colX3, hdr3, 4, secY);
        secY += 30.f;

        int dri = 0;
        for (int i = 0; i < doctors.size(); i++) {
            int dc = 0, dp = 0, dn = 0;
            for (int j = 0; j < appointments.size(); j++) {
                if (appointments[j].getDoctorId() != doctors[i].getId()) continue;
                if (!strEquals(appointments[j].getDate(), today))        continue;
                if (appointments[j].isCompleted()) dc++;
                else if (appointments[j].isPending())   dp++;
                else if (appointments[j].isNoShow())    dn++;
            }
            if (dc + dp + dn == 0) continue;

            char dcB[8], dpB[8], dnB[8];
            intToStr(dc, dcB, 8); intToStr(dp, dpB, 8); intToStr(dn, dnB, 8);
            const char* vals[4] = { doctors[i].getName(), dcB, dpB, dnB };
            sf::Color   cols[4] = { C::TEXT_HI, C::TEAL, sf::Color(200,150,50), C::RED };
            adminTableRow(win, font, colX3, vals, cols, 4, secY, dri++);
            secY += 27.f;
            if (secY > (float)WIN_H - 80.f) break;
        }
    }

    backBtn.setPosition(300.f, (float)WIN_H - 58.f);
    backBtn.setSize(130.f, 38.f);
    backBtn.setLabel("< Back");
    backBtn.setStyle(Button::GHOST);
    backBtn.update(mouse); backBtn.draw(win);
    if (backBtn.handleEvent(ev, mouse)) return SCR_ADMIN_MENU;
    return SCR_ADMIN_REPORT;
}

//  7. SCR_ADMIN_ADD_DOCTOR
struct AddDoctorForm {
    InputField nameF, specF, contactF, passF, feeF;
    Button     submitBtn, backBtn;
    bool       ready;

    void init(sf::Font& font) {
        float cx = 620.f, fw = 400.f, fh = 44.f, gap = 58.f, sy = 170.f;
        nameF = InputField(cx, sy, fw, fh, "Full name", font, false, 15, 50);
        specF = InputField(cx, sy + gap, fw, fh, "Specialization", font, false, 15, 50);
        contactF = InputField(cx, sy + 2 * gap, fw, fh, "Contact (11 digits)", font, false, 15, 11);
        passF = InputField(cx, sy + 3 * gap, fw, fh, "Password (min 6 chars)", font, true, 15, 50);
        feeF = InputField(cx, sy + 4 * gap, fw, fh, "Consultation fee (PKR)", font, false, 15, 10);
        submitBtn = Button(cx, sy + 5 * gap, fw, fh, "ADD DOCTOR", font, Button::PRIMARY, 15);
        backBtn = Button(cx, sy + 5 * gap + 54, fw, 36, "Cancel", font, Button::GHOST, 14);
        ready = true;
    }
};
static AddDoctorForm g_addDocForm;

static Screen drawAdminAddDoctor(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    bool& formReady)
{
    if (!formReady) { g_addDocForm.init(font); formReady = true; }

    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawAdminContentHeader(win, font, "Add Doctor");
    adminDrawMsg(win, font);

    float lx = 305.f, sy = 178.f, gap = 58.f;
    const char* labels[5] = { "Name", "Specialization", "Contact", "Password", "Fee (PKR)" };
    for (int i = 0; i < 5; i++)
        win.draw(makeText(labels[i], font, 14, C::TEXT_DIM, lx, sy + i * gap + 14.f));

    AddDoctorForm& f = g_addDocForm;
    f.nameF.update();    f.nameF.handleEvent(ev, mouse);    f.nameF.draw(win);
    f.specF.update();    f.specF.handleEvent(ev, mouse);    f.specF.draw(win);
    f.contactF.update(); f.contactF.handleEvent(ev, mouse); f.contactF.draw(win);
    f.passF.update();    f.passF.handleEvent(ev, mouse);    f.passF.draw(win);
    f.feeF.update();     f.feeF.handleEvent(ev, mouse);     f.feeF.draw(win);
    f.submitBtn.update(mouse); f.submitBtn.draw(win);
    f.backBtn.update(mouse);   f.backBtn.draw(win);

    if (f.backBtn.handleEvent(ev, mouse)) {
        clearUIMsg();
        f.nameF.clear(); f.specF.clear();
        f.contactF.clear(); f.passF.clear(); f.feeF.clear();
        return SCR_ADMIN_MENU;
    }

    if (f.submitBtn.handleEvent(ev, mouse)) {
        if (!Validator::validateName(f.nameF.getText(), 50)) { setUIMsg("Invalid name.", true);                          playErrorSound(); return SCR_ADMIN_ADD_DOCTOR; }
        if (!Validator::validateName(f.specF.getText(), 50)) { setUIMsg("Invalid specialization.", true);                playErrorSound(); return SCR_ADMIN_ADD_DOCTOR; }
        if (!Validator::validateContact(f.contactF.getText())) { setUIMsg("Contact must be exactly 11 digits.", true);     playErrorSound(); return SCR_ADMIN_ADD_DOCTOR; }
        if (!Validator::validatePassword(f.passF.getText())) { setUIMsg("Password must be at least 6 characters.", true); playErrorSound(); return SCR_ADMIN_ADD_DOCTOR; }

        float fee = 0.f;
        if (!parseFloat(f.feeF.getText(), fee) || !Validator::validatePositiveFloat(fee)) {
            setUIMsg("Fee must be a positive number.", true); playErrorSound(); return SCR_ADMIN_ADD_DOCTOR;
        }

        int newId = doctors.getMaxId() + 1;
        Doctor d(newId, f.nameF.getText(), f.specF.getText(),
            f.contactF.getText(), f.passF.getText(), fee);
        doctors.add(d);
        FileHandler::appendDoctor(d, FILE_DOCTORS);

        char msg[64] = "Doctor added. ID: ";
        char idB[12]; intToStr(newId, idB, 12); strAppend(msg, idB, 64);
        setUIMsg(msg, false);

        f.nameF.clear(); f.specF.clear();
        f.contactF.clear(); f.passF.clear(); f.feeF.clear();
    }

    return SCR_ADMIN_ADD_DOCTOR;
}

//  8. SCR_ADMIN_REMOVE_DOCTOR
struct RemoveDoctorForm {
    InputField idField;
    Button     confirmBtn, backBtn;
    bool       ready;
    int        foundId;
    char       foundName[100];

    void init(sf::Font& font) {
        float cx = 620.f;
        idField = InputField(cx, 200.f, 360.f, 48.f, "Doctor ID to remove", font, false, 15, 6);
        confirmBtn = Button(cx, 270.f, 360.f, 48.f, "REMOVE DOCTOR", font, Button::DANGER, 15);
        backBtn = Button(cx, 330.f, 360.f, 38.f, "Cancel", font, Button::GHOST, 14);
        ready = true; foundId = -1; foundName[0] = '\0';
    }
};
static RemoveDoctorForm g_remDocForm;

static Screen drawAdminRemoveDoctor(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    bool& formReady)
{
    if (!formReady) { g_remDocForm.init(font); formReady = true; }

    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawAdminContentHeader(win, font, "Remove Doctor");
    adminDrawMsg(win, font);

    win.draw(makeText("Existing Doctors:", font, 14, C::TEXT_DIM, 305.f, 165.f));
    float rowY = 188.f;
    for (int i = 0; i < doctors.size() && rowY < (float)WIN_H - 60.f; i++) {
        char idB[12]; intToStr(doctors[i].getId(), idB, 12);
        char row[128] = {};
        strAppend(row, idB, 128); strAppend(row, "  -  ", 128);
        strAppend(row, doctors[i].getName(), 128);
        strAppend(row, "  (", 128); strAppend(row, doctors[i].getSpecialization(), 128);
        strAppend(row, ")", 128);
        win.draw(makeText(row, font, 13, C::TEXT_MID, 310.f, rowY));
        rowY += 24.f;
    }

    RemoveDoctorForm& f = g_remDocForm;
    f.idField.update();    f.idField.handleEvent(ev, mouse);    f.idField.draw(win);
    f.confirmBtn.update(mouse); f.confirmBtn.draw(win);
    f.backBtn.update(mouse);    f.backBtn.draw(win);

    if (f.backBtn.handleEvent(ev, mouse)) {
        clearUIMsg(); f.idField.clear(); f.foundId = -1; f.foundName[0] = '\0';
        return SCR_ADMIN_MENU;
    }

    if (f.confirmBtn.handleEvent(ev, mouse)) {
        int did;
        if (!parseInt(f.idField.getText(), did)) {
            setUIMsg("Enter a valid numeric Doctor ID.", true); playErrorSound(); return SCR_ADMIN_REMOVE_DOCTOR;
        }
        for (int i = 0; i < appointments.size(); i++) {
            if (appointments[i].getDoctorId() == did && appointments[i].isPending()) {
                setUIMsg("Cannot remove: doctor has pending appointments.", true);
                playErrorSound(); return SCR_ADMIN_REMOVE_DOCTOR;
            }
        }
        if (doctors.removeById(did)) {
            FileHandler::rewriteDoctors(doctors, FILE_DOCTORS);
            setUIMsg("Doctor removed successfully.", false);
            f.idField.clear();
        }
        else {
            setUIMsg("Doctor ID not found.", true); playErrorSound();
        }
    }

    return SCR_ADMIN_REMOVE_DOCTOR;
}

//  9. SCR_ADMIN_ADD_PATIENT
struct AddPatientForm {
    InputField nameF, ageF, genderF, contactF, passF, balF;
    Button     submitBtn, backBtn;
    bool       ready;

    void init(sf::Font& font) {
        float cx = 620.f, fw = 400.f, fh = 44.f, gap = 52.f, sy = 162.f;
        nameF = InputField(cx, sy, fw, fh, "Full name", font, false, 15, 50);
        ageF = InputField(cx, sy + gap, fw, fh, "Age", font, false, 15, 3);
        genderF = InputField(cx, sy + 2 * gap, fw, fh, "Gender (Male/Female/Other)", font, false, 15, 20);
        contactF = InputField(cx, sy + 3 * gap, fw, fh, "Contact (11 digits)", font, false, 15, 11);
        passF = InputField(cx, sy + 4 * gap, fw, fh, "Password (min 6 chars)", font, true, 15, 50);
        balF = InputField(cx, sy + 5 * gap, fw, fh, "Initial balance (PKR)", font, false, 15, 10);
        submitBtn = Button(cx, sy + 6 * gap, fw, fh, "ADD PATIENT", font, Button::PRIMARY, 15);
        backBtn = Button(cx, sy + 6 * gap + 54.f, fw, 36, "Cancel", font, Button::GHOST, 14);
        ready = true;
    }
};
static AddPatientForm g_addPatForm;

static Screen drawAdminAddPatient(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    bool& formReady)
{
    if (!formReady) { g_addPatForm.init(font); formReady = true; }

    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawAdminContentHeader(win, font, "Add Patient");
    adminDrawMsg(win, font);

    float lx = 305.f, sy = 170.f, gap = 52.f;
    const char* labels[6] = { "Name","Age","Gender","Contact","Password","Balance" };
    for (int i = 0; i < 6; i++)
        win.draw(makeText(labels[i], font, 14, C::TEXT_DIM, lx, sy + i * gap + 12.f));

    AddPatientForm& f = g_addPatForm;
    f.nameF.update();    f.nameF.handleEvent(ev, mouse);    f.nameF.draw(win);
    f.ageF.update();     f.ageF.handleEvent(ev, mouse);     f.ageF.draw(win);
    f.genderF.update();  f.genderF.handleEvent(ev, mouse);  f.genderF.draw(win);
    f.contactF.update(); f.contactF.handleEvent(ev, mouse); f.contactF.draw(win);
    f.passF.update();    f.passF.handleEvent(ev, mouse);    f.passF.draw(win);
    f.balF.update();     f.balF.handleEvent(ev, mouse);     f.balF.draw(win);
    f.submitBtn.update(mouse); f.submitBtn.draw(win);
    f.backBtn.update(mouse);   f.backBtn.draw(win);

    if (f.backBtn.handleEvent(ev, mouse)) {
        clearUIMsg();
        f.nameF.clear(); f.ageF.clear(); f.genderF.clear();
        f.contactF.clear(); f.passF.clear(); f.balF.clear();
        return SCR_ADMIN_MENU;
    }

    if (f.submitBtn.handleEvent(ev, mouse)) {
        if (!Validator::validateName(f.nameF.getText(), 50)) { setUIMsg("Invalid name.", true);                          playErrorSound(); return SCR_ADMIN_ADD_PATIENT; }
        if (!Validator::validateContact(f.contactF.getText())) { setUIMsg("Contact must be exactly 11 digits.", true);     playErrorSound(); return SCR_ADMIN_ADD_PATIENT; }
        if (!Validator::validatePassword(f.passF.getText())) { setUIMsg("Password must be at least 6 characters.", true); playErrorSound(); return SCR_ADMIN_ADD_PATIENT; }
        if (f.genderF.getLength() == 0) { setUIMsg("Gender cannot be empty.", true);                playErrorSound(); return SCR_ADMIN_ADD_PATIENT; }

        int age = strToInt(f.ageF.getText());
        if (!isNumeric(f.ageF.getText()) || age <= 0 || age > 130) {
            setUIMsg("Enter a valid age (1-130).", true); playErrorSound(); return SCR_ADMIN_ADD_PATIENT;
        }

        float bal = 0.f;
        if (!parseFloat(f.balF.getText(), bal) || bal < 0.f) {
            setUIMsg("Balance must be a non-negative number.", true); playErrorSound(); return SCR_ADMIN_ADD_PATIENT;
        }

        int newId = patients.getMaxId() + 1;
        Patient p(newId, f.nameF.getText(), age, f.genderF.getText(),
            f.contactF.getText(), f.passF.getText(), bal);
        patients.add(p);
        FileHandler::appendPatient(p, FILE_PATIENTS);

        char msg[64] = "Patient added. ID: ";
        char idB[12]; intToStr(newId, idB, 12); strAppend(msg, idB, 64);
        setUIMsg(msg, false);

        f.nameF.clear(); f.ageF.clear(); f.genderF.clear();
        f.contactF.clear(); f.passF.clear(); f.balF.clear();
    }

    return SCR_ADMIN_ADD_PATIENT;
}

//  10. SCR_ADMIN_REMOVE_PATIENT
struct RemovePatientForm {
    InputField idField;
    Button     confirmBtn, backBtn;
    bool       ready;

    void init(sf::Font& font) {
        float cx = 620.f;
        idField = InputField(cx, 200.f, 360.f, 48.f, "Patient ID to remove", font, false, 15, 6);
        confirmBtn = Button(cx, 270.f, 360.f, 48.f, "REMOVE PATIENT", font, Button::DANGER, 15);
        backBtn = Button(cx, 330.f, 360.f, 38.f, "Cancel", font, Button::GHOST, 14);
        ready = true;
    }
};
static RemovePatientForm g_remPatForm;

static Screen drawAdminRemovePatient(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    bool& formReady)
{
    if (!formReady) { g_remPatForm.init(font); formReady = true; }

    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawAdminContentHeader(win, font, "Remove Patient");
    adminDrawMsg(win, font);

    win.draw(makeText("Existing Patients:", font, 14, C::TEXT_DIM, 305.f, 165.f));
    float rowY = 188.f;
    for (int i = 0; i < patients.size() && rowY < (float)WIN_H - 60.f; i++) {
        char idB[12]; intToStr(patients[i].getId(), idB, 12);
        char row[128] = {};
        strAppend(row, idB, 128); strAppend(row, "  -  ", 128);
        strAppend(row, patients[i].getName(), 128);
        win.draw(makeText(row, font, 13, C::TEXT_MID, 310.f, rowY));
        rowY += 24.f;
    }

    RemovePatientForm& f = g_remPatForm;
    f.idField.update();    f.idField.handleEvent(ev, mouse);    f.idField.draw(win);
    f.confirmBtn.update(mouse); f.confirmBtn.draw(win);
    f.backBtn.update(mouse);    f.backBtn.draw(win);

    if (f.backBtn.handleEvent(ev, mouse)) {
        clearUIMsg(); f.idField.clear(); return SCR_ADMIN_MENU;
    }

    if (f.confirmBtn.handleEvent(ev, mouse)) {
        int pid;
        if (!parseInt(f.idField.getText(), pid)) {
            setUIMsg("Enter a valid numeric Patient ID.", true); playErrorSound(); return SCR_ADMIN_REMOVE_PATIENT;
        }
        for (int i = 0; i < appointments.size(); i++) {
            if (appointments[i].getPatientId() == pid && appointments[i].isPending()) {
                setUIMsg("Cannot remove: patient has pending appointments.", true);
                playErrorSound(); return SCR_ADMIN_REMOVE_PATIENT;
            }
        }
        if (patients.removeById(pid)) {
            FileHandler::rewritePatients(patients, FILE_PATIENTS);
            setUIMsg("Patient removed successfully.", false);
            f.idField.clear();
        }
        else {
            setUIMsg("Patient ID not found.", true); playErrorSound();
        }
    }

    return SCR_ADMIN_REMOVE_PATIENT;
}

//  11. SCR_ADMIN_DISCHARGE
struct DischargeForm {
    InputField idField;
    Button     confirmBtn, backBtn;
    bool       ready;

    void init(sf::Font& font) {
        float cx = 620.f;
        idField = InputField(cx, 200.f, 360.f, 48.f, "Patient ID to discharge", font, false, 15, 6);
        confirmBtn = Button(cx, 270.f, 360.f, 48.f, "DISCHARGE PATIENT", font, Button::DANGER, 15);
        backBtn = Button(cx, 330.f, 360.f, 38.f, "Cancel", font, Button::GHOST, 14);
        ready = true;
    }
};
static DischargeForm g_dischargeForm;

static Screen drawAdminDischarge(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    bool& formReady)
{
    if (!formReady) { g_dischargeForm.init(font); formReady = true; }

    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawAdminContentHeader(win, font, "Discharge Patient");
    adminDrawMsg(win, font);

    const char* info[3] = {
        "Discharge archives the patient record and removes them from the system.",
        "Requirements: no unpaid bills, no pending appointments.",
        "This action cannot be undone."
    };
    float iy = 162.f;
    for (int i = 0; i < 3; i++) {
        win.draw(makeText(info[i], font, 13, i == 2 ? C::RED : C::TEXT_DIM, 305.f, iy));
        iy += 20.f;
    }
    win.draw(makeText("Patient ID:", font, 14, C::TEXT_DIM, 305.f, 214.f));

    DischargeForm& f = g_dischargeForm;
    f.idField.update();    f.idField.handleEvent(ev, mouse);    f.idField.draw(win);
    f.confirmBtn.update(mouse); f.confirmBtn.draw(win);
    f.backBtn.update(mouse);    f.backBtn.draw(win);

    if (f.backBtn.handleEvent(ev, mouse)) {
        clearUIMsg(); f.idField.clear(); return SCR_ADMIN_MENU;
    }

    if (f.confirmBtn.handleEvent(ev, mouse)) {
        int pid;
        if (!parseInt(f.idField.getText(), pid)) {
            setUIMsg("Enter a valid numeric Patient ID.", true); playErrorSound(); return SCR_ADMIN_DISCHARGE;
        }

        Patient* pat = patients.findById(pid);
        if (!pat) { setUIMsg("Patient ID not found.", true); playErrorSound(); return SCR_ADMIN_DISCHARGE; }

        for (int i = 0; i < bills.size(); i++)
            if (bills[i].getPatientId() == pid && bills[i].isUnpaid()) {
                setUIMsg("Cannot discharge: patient has unpaid bills.", true);
                playErrorSound(); return SCR_ADMIN_DISCHARGE;
            }

        for (int i = 0; i < appointments.size(); i++)
            if (appointments[i].getPatientId() == pid && appointments[i].isPending()) {
                setUIMsg("Cannot discharge: patient has pending appointments.", true);
                playErrorSound(); return SCR_ADMIN_DISCHARGE;
            }

        FileHandler::appendPatient(*pat, FILE_DISCHARGED);
        patients.removeById(pid);
        FileHandler::rewritePatients(patients, FILE_PATIENTS);

        for (int i = bills.size() - 1; i >= 0; i--)
            if (bills[i].getPatientId() == pid) bills.removeById(bills[i].getId());
        FileHandler::rewriteBills(bills, FILE_BILLS);

        for (int i = appointments.size() - 1; i >= 0; i--)
            if (appointments[i].getPatientId() == pid) appointments.removeById(appointments[i].getId());
        FileHandler::rewriteAppointments(appointments, FILE_APPOINTMENTS);

        for (int i = prescriptions.size() - 1; i >= 0; i--)
            if (prescriptions[i].getPatientId() == pid) prescriptions.removeById(prescriptions[i].getId());
        FileHandler::rewritePrescriptions(prescriptions, FILE_PRESCRIPTIONS);

        setUIMsg("Patient discharged and archived successfully.", false);
        f.idField.clear();
    }
    return SCR_ADMIN_DISCHARGE;
}