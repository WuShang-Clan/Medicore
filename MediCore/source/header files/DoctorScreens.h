#pragma once
#include "Helpers.h"

static void drawDocHeader(sf::RenderWindow& win, sf::Font& font, const char* title) {
    win.draw(makeText(title, font, 26, C::TEXT_HI, 60.f, 72.f));
    sf::RectangleShape acc(sf::Vector2f(36.f, 3.f));
    acc.setPosition(60.f, 106.f);
    acc.setFillColor(C::TEAL);
    win.draw(acc);
}

//  1. SCR_DOC_TODAY — Today's Appointments
static Screen drawDocToday(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    Button& backBtn)
{
    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawDocHeader(win, font, "Today's Appointments");

    char today[15]; getTodayDate(today, 15);
    char dateBadge[40] = "Date: ";
    strAppend(dateBadge, today, 40);
    win.draw(makeText(dateBadge, font, 13, C::TEXT_DIM, 60.f, 110.f));

    int idx[200]; int cnt = 0;
    for (int i = 0; i < appointments.size() && cnt < 200; i++) {
        if (appointments[i].getDoctorId() == g_doctor->getId() &&
            strEquals(appointments[i].getDate(), today))
            idx[cnt++] = i;
    }

    // Sort ascending by time slot
    for (int i = 0; i < cnt - 1; i++)
        for (int j = 0; j < cnt - i - 1; j++) {
            const char* s1 = appointments[idx[j]].getTimeSlot();
            const char* s2 = appointments[idx[j + 1]].getTimeSlot();
            bool sw = false;
            for (int k = 0; s1[k] && s2[k]; k++) {
                if (s1[k] > s2[k]) { sw = true; break; }
                if (s1[k] < s2[k]) break;
            }
            if (sw) { int t = idx[j]; idx[j] = idx[j + 1]; idx[j + 1] = t; }
        }

    float colX[5] = { 60.f, 130.f, 380.f, 540.f, 700.f };
    const char* hdrs[5] = { "ID", "Patient", "Contact", "Slot", "Status" };
    drawTableHeader(win, font, colX, hdrs, 5, 128.f);

    int visible = 18, startRow = g_scrollOffset;
    if (startRow > cnt - visible) startRow = cnt - visible;
    if (startRow < 0) startRow = 0;

    float rowY = 156.f;
    char rowBufs[200][5][32];

    for (int i = startRow; i < cnt && i < startRow + visible; i++) {
        Appointment& a = appointments[idx[i]];
        Patient* pat = patients.findById(a.getPatientId());

        intToStr(a.getId(), rowBufs[i][0], 32);
        copyStr(rowBufs[i][1], pat ? pat->getName() : "Unknown", 32);
        copyStr(rowBufs[i][2], pat ? pat->getContact() : "?", 32);
        copyStr(rowBufs[i][3], a.getTimeSlot(), 32);
        copyStr(rowBufs[i][4], a.getStatus(), 32);

        sf::Color statusCol = a.isCompleted() ? C::TEAL
            : a.isCancelled() ? C::RED
            : a.isNoShow() ? sf::Color(200, 150, 50)
            : C::TEXT_MID;

        const char* vals[5] = {
            rowBufs[i][0], rowBufs[i][1], rowBufs[i][2],
            rowBufs[i][3], rowBufs[i][4]
        };
        sf::Color cols[5] = { C::TEXT_DIM, C::TEXT_HI, C::TEXT_MID,
                              C::TEXT_MID, statusCol };
        drawTableRow(win, font, colX, vals, cols, 5, rowY, i);
        rowY += 27.f;
    }
    if (cnt == 0) {
        sf::Text e = makeText("No appointments scheduled for today.", font, 16, C::TEXT_DIM, 0.f, 320.f);
        centreX(e, 0.f, (float)WIN_W); win.draw(e);
    }

    if (cnt > visible)
        win.draw(makeText("Up/Down to scroll", font, 12, C::TEXT_DIM,
            (float)WIN_W - 200.f, (float)WIN_H - 30.f));
    handleScroll(ev, cnt, visible, g_scrollOffset);

    backBtn.setPosition(60.f, (float)WIN_H - 60.f);
    backBtn.setSize(140.f, 40.f);
    backBtn.setLabel("< Back");
    backBtn.setStyle(Button::GHOST);
    backBtn.update(mouse); backBtn.draw(win);
    if (backBtn.handleEvent(ev, mouse)) return SCR_DOCTOR_MENU;

    return SCR_DOC_TODAY;
}

//  2. SCR_DOC_COMPLETE  /  3. SCR_DOC_NOSHOW
static InputField g_markField;
static bool       g_markFieldInit = false;

static Screen drawDocMark(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    const char* newStatus, Screen thisScreen)
{
    bool isNoShow = strEquals(newStatus, "no-show");

    if (!g_markFieldInit) {
        g_markField = InputField((float)WIN_W / 2.f - 180.f, 300.f, 360.f, 48.f,
            "Appointment ID", font, false, 15, 6);
        g_markFieldInit = true;
    }

    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawUIMsg(win, font);

    float cx = (float)WIN_W / 2.f;
    drawCard(win, cx - 240.f, 130.f, 480.f, 380.f);

    char title[60] = "Mark as: ";
    strAppend(title, newStatus, 60);
    sf::Text tt = makeText(title, font, 24, C::TEXT_HI, 0.f, 160.f);
    centreX(tt, cx - 240.f, 480.f); win.draw(tt);

    const char* infoText = isNoShow
        ? "Marks appointment as no-show. Bill will be cancelled (no refund)."
        : "Marks appointment as completed. You can then write a prescription.";
    sf::Text info = makeText(infoText, font, 13, C::TEXT_DIM, 0.f, 202.f);
    centreX(info, cx - 240.f, 480.f); win.draw(info);

    // Today's pending appointments (reference list)
    char today[15]; getTodayDate(today, 15);
    win.draw(makeText("Today's pending appointments:", font, 13, C::TEXT_DIM, cx - 220.f, 228.f));

    float ry = 248.f;
    for (int i = 0; i < appointments.size() && ry < 300.f; i++) {
        Appointment& a = appointments[i];
        if (a.getDoctorId() != g_doctor->getId() ||
            !strEquals(a.getDate(), today) || !a.isPending()) continue;

        Patient* pat = patients.findById(a.getPatientId());
        char row[128] = {};
        char idB[12]; intToStr(a.getId(), idB, 12);
        strAppend(row, idB, 128);
        strAppend(row, "  ", 128);
        strAppend(row, pat ? pat->getName() : "?", 128);
        strAppend(row, "  @ ", 128);
        strAppend(row, a.getTimeSlot(), 128);

        win.draw(makeText(row, font, 13, C::TEXT_MID, cx - 220.f, ry));
        ry += 20.f;
    }

    win.draw(makeText("Appointment ID:", font, 14, C::TEXT_DIM, cx - 180.f, 284.f));
    g_markField.update();
    g_markField.handleEvent(ev, mouse);
    g_markField.draw(win);

    // Confirm button
    sf::Color btnFill = isNoShow ? sf::Color(80, 15, 15) : sf::Color(0, 70, 60);
    sf::Color btnHover = isNoShow ? sf::Color(160, 30, 30) : C::TEAL_DIM;
    sf::Color btnBorder = isNoShow ? C::RED : C::TEAL;

    bool ch = (mouse.x >= (int)(cx - 180) && mouse.x <= (int)(cx + 180) &&
        mouse.y >= 366 && mouse.y <= 410);
    sf::RectangleShape cb(sf::Vector2f(360.f, 44.f));
    cb.setPosition(cx - 180.f, 366.f);
    cb.setFillColor(ch ? btnHover : btnFill);
    cb.setOutlineThickness(1.5f); cb.setOutlineColor(btnBorder);
    win.draw(cb);
    char btnLabel[40] = "MARK AS ";
    strAppend(btnLabel, newStatus, 40);
    sf::Text ct = makeText(btnLabel, font, 14, C::TEXT_HI, 0.f, 379.f);
    centreX(ct, cx - 180.f, 360.f); win.draw(ct);

    // Back button
    bool bh = (mouse.x >= (int)(cx - 180) && mouse.x <= (int)(cx + 180) &&
        mouse.y >= 420 && mouse.y <= 456);
    sf::RectangleShape bb(sf::Vector2f(360.f, 36.f));
    bb.setPosition(cx - 180.f, 420.f);
    bb.setFillColor(bh ? C::BG_CARD : sf::Color(0, 0, 0, 0));
    bb.setOutlineThickness(1.f); bb.setOutlineColor(C::SLATE);
    win.draw(bb);
    sf::Text bt = makeText("Back", font, 13, C::TEXT_DIM, 0.f, 430.f);
    centreX(bt, cx - 180.f, 360.f); win.draw(bt);

    // Handle confirm
    if (ch && ev.type == sf::Event::MouseButtonReleased &&
        ev.mouseButton.button == sf::Mouse::Left)
    {
        int aid;
        if (!parseInt(g_markField.getText(), aid)) {
            setUIMsg("Enter a valid numeric Appointment ID.", true);
            playErrorSound();
        }
        else {
            char today2[15]; getTodayDate(today2, 15);
            Appointment* a = appointments.findById(aid);
            if (!a || a->getDoctorId() != g_doctor->getId() ||
                !a->isPending() || !strEquals(a->getDate(), today2))
            {
                setUIMsg("Invalid ID, or appointment is not today / not pending.", true);
                playErrorSound();
            }
            else {
                if (!isNoShow && strEquals(newStatus, "completed")) {
                    Bill* bill = nullptr;
                    for (int i = 0; i < bills.size(); i++) {
                        if (bills[i].getAppointmentId() == aid) {
                            bill = &bills[i]; break;
                        }
                    }
                    if (!bill || !strEqualsIC(bill->getStatus(), "paid")) {
                        setUIMsg("Cannot complete appointment. Bill is not paid.", true);
                        playErrorSound();
                        return thisScreen;
                    }
                }
                a->setStatus(newStatus);
                FileHandler::rewriteAppointments(appointments, FILE_APPOINTMENTS);

                if (isNoShow) {
                    for (int i = 0; i < bills.size(); i++) {
                        if (bills[i].getAppointmentId() == aid) {
                            bills[i].setStatus("cancelled"); break;
                        }
                    }
                    FileHandler::rewriteBills(bills, FILE_BILLS);
                }
                setUIMsg("Status updated successfully.", false);
                g_markField.clear();
            }
        }
    }

    if (bh && ev.type == sf::Event::MouseButtonReleased &&
        ev.mouseButton.button == sf::Mouse::Left)
    {
        g_markField.clear(); g_markFieldInit = false;
        clearUIMsg(); return SCR_DOCTOR_MENU;
    }

    return thisScreen;
}

//  4. SCR_DOC_PRESCRIBE — Write Prescription
struct PrescribeState {
    InputField apptField;
    InputField medField;
    InputField notesField;
    bool       ready;

    void init(sf::Font& font) {
        float fx = 500.f, fw = 620.f;
        apptField = InputField(fx, 168.f, fw, 44.f,
            "Appointment ID (must be completed)", font, false, 15, 6);
        medField = InputField(fx, 238.f, fw, 44.f,
            "Medicines e.g. Paracetamol 500mg; Amoxicillin 250mg", font, false, 15, 200);
        notesField = InputField(fx, 308.f, fw, 44.f,
            "Notes / instructions", font, false, 15, 200);
        ready = true;
    }
    void clear() {
        apptField.clear(); medField.clear(); notesField.clear(); ready = false;
    }
};
static PrescribeState g_prescribe;
static bool           g_prescribeInit = false;

static Screen drawDocPrescribe(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse)
{
    if (!g_prescribeInit) { g_prescribe.init(font); g_prescribeInit = true; }

    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawUIMsg(win, font);
    drawDocHeader(win, font, "Write Prescription");

    win.draw(makeText("Appointment ID:", font, 14, C::TEXT_DIM, 60.f, 180.f));
    win.draw(makeText("Medicines:", font, 14, C::TEXT_DIM, 60.f, 250.f));
    win.draw(makeText("Notes:", font, 14, C::TEXT_DIM, 60.f, 320.f));

    g_prescribe.apptField.update();  g_prescribe.apptField.handleEvent(ev, mouse);  g_prescribe.apptField.draw(win);
    g_prescribe.medField.update();   g_prescribe.medField.handleEvent(ev, mouse);   g_prescribe.medField.draw(win);
    g_prescribe.notesField.update(); g_prescribe.notesField.handleEvent(ev, mouse); g_prescribe.notesField.draw(win);

    bool sh = (mouse.x >= 500 && mouse.x <= 860 &&
        mouse.y >= 374 && mouse.y <= 418);
    sf::RectangleShape sb(sf::Vector2f(360.f, 44.f));
    sb.setPosition(500.f, 374.f);
    sb.setFillColor(sh ? C::TEAL_DIM : sf::Color(0, 70, 60));
    sb.setOutlineThickness(1.5f); sb.setOutlineColor(C::TEAL);
    win.draw(sb);
    sf::Text st = makeText("SAVE PRESCRIPTION", font, 14, C::TEXT_HI, 0.f, 387.f);
    centreX(st, 500.f, 360.f); win.draw(st);

    if (sh && ev.type == sf::Event::MouseButtonReleased &&
        ev.mouseButton.button == sf::Mouse::Left)
    {
        int aid;
        if (!parseInt(g_prescribe.apptField.getText(), aid)) {
            setUIMsg("Enter a valid numeric Appointment ID.", true);
            playErrorSound();
        }
        else {
            Appointment* a = appointments.findById(aid);
            if (!a || a->getDoctorId() != g_doctor->getId() || !a->isCompleted()) {
                setUIMsg("Appointment must be completed and belong to you.", true);
                playErrorSound();
            }
            else if (g_prescribe.medField.getLength() == 0) {
                setUIMsg("Enter at least one medicine.", true);
                playErrorSound();
            }
            else {
                bool dup = false;
                for (int i = 0; i < prescriptions.size(); i++) {
                    if (prescriptions[i].getAppointmentId() == aid) { dup = true; break; }
                }
                if (dup) {
                    setUIMsg("Prescription already written for this appointment.", true);
                    playErrorSound();
                }
                else {
                    int newPid = prescriptions.getMaxId() + 1;
                    Prescription pr(newPid, aid, a->getPatientId(),
                        g_doctor->getId(), a->getDate(),
                        g_prescribe.medField.getText(),
                        g_prescribe.notesField.getText());
                    prescriptions.add(pr);
                    FileHandler::appendPrescription(pr, FILE_PRESCRIPTIONS);
                    setUIMsg("Prescription saved successfully.", false);
                    g_prescribe.clear(); g_prescribeInit = false;
                }
            }
        }
    }

    if (drawBackButton(win, font, mouse, ev)) {
        g_prescribe.clear(); g_prescribeInit = false;
        clearUIMsg(); return SCR_DOCTOR_MENU;
    }

    return SCR_DOC_PRESCRIBE;
}

//  5. SCR_DOC_HISTORY — Patient Medical History
static InputField g_histField;
static bool       g_histFieldInit = false;
static int        g_histPatientId = -1;

static Screen drawDocHistory(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse)
{
    if (!g_histFieldInit) {
        g_histField = InputField(300.f, 150.f, 260.f, 44.f,
            "Patient ID", font, false, 15, 6);
        g_histFieldInit = true;
    }

    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawUIMsg(win, font);
    drawDocHeader(win, font, "Patient Medical History");

    win.draw(makeText(
        "You can only view records of your own patients "
        "(must have a completed appointment with them).",
        font, 13, C::TEXT_DIM, 60.f, 112.f));

    win.draw(makeText("Patient ID:", font, 14, C::TEXT_DIM, 60.f, 162.f));
    g_histField.update();
    g_histField.handleEvent(ev, mouse);
    g_histField.draw(win);

    bool srch = (mouse.x >= 578 && mouse.x <= 738 &&
        mouse.y >= 150 && mouse.y <= 194);
    sf::RectangleShape sb(sf::Vector2f(160.f, 44.f));
    sb.setPosition(578.f, 150.f);
    sb.setFillColor(srch ? C::TEAL_DIM : sf::Color(0, 70, 60));
    sb.setOutlineThickness(1.5f); sb.setOutlineColor(C::TEAL);
    win.draw(sb);
    sf::Text st = makeText("SEARCH", font, 14, C::TEXT_HI, 0.f, 163.f);
    centreX(st, 578.f, 160.f); win.draw(st);

    if (srch && ev.type == sf::Event::MouseButtonReleased &&
        ev.mouseButton.button == sf::Mouse::Left)
    {
        int pid;
        if (!parseInt(g_histField.getText(), pid)) {
            setUIMsg("Enter a valid numeric Patient ID.", true);
            playErrorSound();
            g_histPatientId = -1;
        }
        else {
            Patient* pat = patients.findById(pid);
            if (!pat) {
                setUIMsg("Patient not found.", true);
                playErrorSound();
                g_histPatientId = -1;
            }
            else {
                bool hasAccess = false;
                for (int i = 0; i < appointments.size(); i++) {
                    if (appointments[i].getPatientId() == pid &&
                        appointments[i].getDoctorId() == g_doctor->getId() &&
                        appointments[i].isCompleted())
                    {
                        hasAccess = true; break;
                    }
                }
                if (!hasAccess) {
                    setUIMsg("Access denied: no completed appointments with this patient.", true);
                    playErrorSound();
                    g_histPatientId = -1;
                }
                else {
                    g_histPatientId = pid;
                    clearUIMsg();
                }
            }
        }
    }

    if (g_histPatientId > 0) {
        Patient* pat = patients.findById(g_histPatientId);
        if (pat) {
            char infoLine[128] = "Showing records for: ";
            strAppend(infoLine, pat->getName(), 128);
            win.draw(makeText(infoLine, font, 14, C::TEAL, 60.f, 208.f));
        }

        int idx[200]; int cnt = 0;
        for (int i = 0; i < prescriptions.size() && cnt < 200; i++) {
            if (prescriptions[i].getPatientId() == g_histPatientId &&
                prescriptions[i].getDoctorId() == g_doctor->getId())
                idx[cnt++] = i;
        }

        for (int i = 0; i < cnt - 1; i++)
            for (int j = 0; j < cnt - i - 1; j++)
                if (compareDates(prescriptions[idx[j]].getDate(),
                    prescriptions[idx[j + 1]].getDate()) < 0)
                {
                    int t = idx[j]; idx[j] = idx[j + 1]; idx[j + 1] = t;
                }

        float colX[3] = { 60.f, 180.f, 560.f };
        const char* hdrs[3] = { "Date", "Medicines", "Notes" };
        drawTableHeader(win, font, colX, hdrs, 3, 228.f);

        int visible = 14, startRow = g_scrollOffset;
        if (startRow > cnt - visible) startRow = cnt - visible;
        if (startRow < 0) startRow = 0;

        float rowY = 256.f;
        char rowBufs[200][3][80];

        for (int i = startRow; i < cnt && i < startRow + visible; i++) {
            Prescription& pr = prescriptions[idx[i]];
            copyStr(rowBufs[i][0], pr.getDate(), 80);
            copyStr(rowBufs[i][1], pr.getMedicines(), 80);
            copyStr(rowBufs[i][2], pr.getNotes(), 80);

            const char* vals[3] = { rowBufs[i][0], rowBufs[i][1], rowBufs[i][2] };
            sf::Color   cols[3] = { C::TEXT_MID, C::TEAL, C::TEXT_DIM };
            drawTableRow(win, font, colX, vals, cols, 3, rowY, i);
            rowY += 27.f;
        }

        if (cnt == 0) {
            win.draw(makeText("No prescription records found for this patient.",
                font, 15, C::TEXT_DIM, 60.f, 290.f));
        }

        if (cnt > visible)
            win.draw(makeText("Up/Down to scroll", font, 12, C::TEXT_DIM,
                (float)WIN_W - 200.f, (float)WIN_H - 30.f));
        handleScroll(ev, cnt, visible, g_scrollOffset);
    }

    if (drawBackButton(win, font, mouse, ev)) {
        g_histField.clear(); g_histFieldInit = false;
        g_histPatientId = -1; clearUIMsg();
        return SCR_DOCTOR_MENU;
    }
    return SCR_DOC_HISTORY;
}