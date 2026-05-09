#pragma once
#include "Helpers.h"

enum BookStep { BK_SPEC = 0, BK_DOC, BK_DATE, BK_SLOT };

struct BookState {
    BookStep    step;
    char        spec[60];
    int         docId;
    char        date[15];
    InputField  inputField;
    bool        fieldInit;

    void reset() {
        step = BK_SPEC; spec[0] = '\0';
        docId = -1; date[0] = '\0'; fieldInit = false;
    }
};
static BookState g_book;
static bool      g_bookStateInit = false;

static Screen drawBookAppt(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse)
{
    if (!g_bookStateInit) { g_book.reset(); g_bookStateInit = true; }

    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawUIMsg(win, font);

    win.draw(makeText("Book Appointment", font, 26, C::TEXT_HI, 60.f, 72.f));
    sf::RectangleShape acc(sf::Vector2f(36.f, 3.f));
    acc.setPosition(60.f, 106.f); acc.setFillColor(C::TEAL); win.draw(acc);

    // Step breadcrumb
    const char* stepLabels[4] = { "1. Specialization", "2. Select Doctor",
                                   "3. Select Date",    "4. Select Slot" };
    float sx = 60.f;
    for (int i = 0; i < 4; i++) {
        bool done = i < (int)g_book.step;
        bool cur = i == (int)g_book.step;
        sf::Color c = done ? C::TEAL : cur ? C::TEXT_HI : C::TEXT_DIM;
        win.draw(makeText(stepLabels[i], font, 13, c, sx, 116.f));
        if (i < 3) {
            sf::RectangleShape sep(sf::Vector2f(20.f, 1.f));
            sep.setPosition(sx + 145.f, 124.f);
            sep.setFillColor(done ? C::TEAL_DIM : C::DIVIDER);
            win.draw(sep);
        }
        sx += 170.f;
    }
    drawDivider(win, 136.f);
    // STEP 0: Enter Specialization
    if (g_book.step == BK_SPEC) {
        if (!g_book.fieldInit) {
            g_book.inputField = InputField(60.f, 170.f, 420.f, 48.f,
                "e.g. Cardiology", font, false, 15, 50);
            g_book.fieldInit = true;
        }
        win.draw(makeText("Enter Specialization to Search:", font, 15, C::TEXT_DIM, 60.f, 152.f));
        g_book.inputField.update();
        g_book.inputField.handleEvent(ev, mouse);
        g_book.inputField.draw(win);

        bool nh = (mouse.x >= 60 && mouse.x <= 260 && mouse.y >= 236 && mouse.y <= 280);
        sf::RectangleShape nb(sf::Vector2f(200.f, 44.f));
        nb.setPosition(60.f, 236.f);
        nb.setFillColor(nh ? C::TEAL_DIM : sf::Color(0, 70, 60));
        nb.setOutlineThickness(1.5f); nb.setOutlineColor(C::TEAL);
        win.draw(nb);
        sf::Text nt = makeText("Search Doctors", font, 14, C::TEXT_HI, 0.f, 249.f);
        centreX(nt, 60.f, 200.f); win.draw(nt);

        if (nh && ev.type == sf::Event::MouseButtonReleased &&
            ev.mouseButton.button == sf::Mouse::Left)
        {
            if (g_book.inputField.getLength() == 0) {
                setUIMsg("Please enter a specialization.", true);
            }
            else {
                bool found = false;
                for (int i = 0; i < doctors.size(); i++)
                    if (strEqualsIC(doctors[i].getSpecialization(),
                        g_book.inputField.getText()))
                    {
                        found = true; break;
                    }
                if (!found) {
                    setUIMsg("No doctors found for that specialization.", true);
                    playErrorSound();
                }
                else {
                    copyStr(g_book.spec, g_book.inputField.getText(), 60);
                    g_book.inputField.clear();
                    g_book.fieldInit = false;
                    g_book.step = BK_DOC;
                    clearUIMsg();
                }
            }
        }
    }

    // STEP 1: Select Doctor
    else if (g_book.step == BK_DOC) {
        float colX[5] = { 60.f, 120.f, 310.f, 530.f, 720.f };
        const char* hdrs[5] = { "ID", "Name", "Specialization", "Contact", "Fee (PKR)" };
        drawTableHeader(win, font, colX, hdrs, 5, 148.f);

        int cnt = 0;
        float rowY = 176.f;
        char rowBufs[50][5][24];
        for (int i = 0; i < doctors.size() && cnt < 50; i++) {
            if (!strEqualsIC(doctors[i].getSpecialization(), g_book.spec)) continue;
            intToStr(doctors[i].getId(), rowBufs[cnt][0], 24);
            copyStr(rowBufs[cnt][1], doctors[i].getName(), 24);
            copyStr(rowBufs[cnt][2], doctors[i].getSpecialization(), 24);
            copyStr(rowBufs[cnt][3], doctors[i].getContact(), 24);
            ftoaBuf(doctors[i].getFee(), rowBufs[cnt][4], 24);

            const char* vals[5] = { rowBufs[cnt][0], rowBufs[cnt][1],
                rowBufs[cnt][2], rowBufs[cnt][3], rowBufs[cnt][4] };
            sf::Color cols[5] = { C::TEXT_DIM, C::TEXT_HI, C::TEAL,
                                  C::TEXT_MID, C::TEXT_MID };
            drawTableRow(win, font, colX, vals, cols, 5, rowY, cnt);
            rowY += 27.f; cnt++;
        }

        if (!g_book.fieldInit) {
            g_book.inputField = InputField(60.f, rowY + 20.f, 200.f, 44.f,
                "Doctor ID", font, false, 15, 6);
            g_book.fieldInit = true;
        }
        win.draw(makeText("Enter Doctor ID:", font, 14, C::TEXT_DIM, 60.f, rowY + 6.f));
        g_book.inputField.update();
        g_book.inputField.handleEvent(ev, mouse);
        g_book.inputField.draw(win);

        float bx = 278.f, by = rowY + 20.f;
        bool nh = (mouse.x >= (int)bx && mouse.x <= (int)(bx + 160) &&
            mouse.y >= (int)by && mouse.y <= (int)(by + 44));
        sf::RectangleShape nb(sf::Vector2f(160.f, 44.f));
        nb.setPosition(bx, by);
        nb.setFillColor(nh ? C::TEAL_DIM : sf::Color(0, 70, 60));
        nb.setOutlineThickness(1.5f); nb.setOutlineColor(C::TEAL);
        win.draw(nb);
        sf::Text nt = makeText("Select", font, 14, C::TEXT_HI, 0.f, by + 13.f);
        centreX(nt, bx, 160.f); win.draw(nt);

        if (nh && ev.type == sf::Event::MouseButtonReleased &&
            ev.mouseButton.button == sf::Mouse::Left)
        {
            int did;
            if (!parseInt(g_book.inputField.getText(), did)) {
                setUIMsg("Enter a valid numeric Doctor ID.", true);
                playErrorSound();
            }
            else {
                Doctor* d = doctors.findById(did);
                if (!d || !strEqualsIC(d->getSpecialization(), g_book.spec)) {
                    setUIMsg("Doctor not found in this specialization.", true);
                    playErrorSound();
                }
                else {
                    g_book.docId = did;
                    g_book.inputField.clear();
                    g_book.fieldInit = false;
                    g_book.step = BK_DATE;
                    clearUIMsg();
                }
            }
        }
    }

    // STEP 2: Enter Date
    else if (g_book.step == BK_DATE) {
        if (!g_book.fieldInit) {
            g_book.inputField = InputField(60.f, 170.f, 260.f, 48.f,
                "DD-MM-YYYY", font, false, 15, 10);
            g_book.fieldInit = true;
        }
        Doctor* doc = doctors.findById(g_book.docId);
        if (doc) {
            char info[128] = "Doctor: ";
            strAppend(info, doc->getName(), 128);
            strAppend(info, "  |  Fee: PKR ", 128);
            char fb[20]; ftoaBuf(doc->getFee(), fb, 20);
            strAppend(info, fb, 128);
            win.draw(makeText(info, font, 14, C::TEAL, 60.f, 148.f));
        }
        win.draw(makeText("Enter Appointment Date:", font, 15, C::TEXT_DIM, 60.f, 170.f));
        g_book.inputField.update();
        g_book.inputField.handleEvent(ev, mouse);
        g_book.inputField.draw(win);

        bool nh = (mouse.x >= 60 && mouse.x <= 260 && mouse.y >= 236 && mouse.y <= 280);
        sf::RectangleShape nb(sf::Vector2f(200.f, 44.f));
        nb.setPosition(60.f, 236.f);
        nb.setFillColor(nh ? C::TEAL_DIM : sf::Color(0, 70, 60));
        nb.setOutlineThickness(1.5f); nb.setOutlineColor(C::TEAL);
        win.draw(nb);
        sf::Text nt = makeText("Next", font, 14, C::TEXT_HI, 0.f, 249.f);
        centreX(nt, 60.f, 200.f); win.draw(nt);

        if (nh && ev.type == sf::Event::MouseButtonReleased &&
            ev.mouseButton.button == sf::Mouse::Left)
        {
            if (!Validator::validateDate(g_book.inputField.getText())) {
                setUIMsg("Invalid date. Use DD-MM-YYYY, current year or later.", true);
                playErrorSound();
            }
            else {
                copyStr(g_book.date, g_book.inputField.getText(), 15);
                g_book.inputField.clear();
                g_book.fieldInit = false;
                g_book.step = BK_SLOT;
                clearUIMsg();
            }
        }
    }

    // STEP 3: Select Slot 
    else if (g_book.step == BK_SLOT) {
        const char* slots[8] = {
            "09:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00"
        };
        Doctor* doc = doctors.findById(g_book.docId);
        char info[128] = "Doctor: ";
        if (doc) {
            strAppend(info, doc->getName(), 128);
            strAppend(info, "  |  Date: ", 128);
            strAppend(info, g_book.date, 128);
        }
        win.draw(makeText(info, font, 14, C::TEAL, 60.f, 148.f));
        win.draw(makeText("Select an available slot:", font, 15, C::TEXT_DIM, 60.f, 172.f));

        float slotX = 60.f, slotY = 200.f;
        for (int s = 0; s < 8; s++) {
            bool taken = false;
            for (int i = 0; i < appointments.size(); i++) {
                Appointment& a = appointments[i];
                if (a.getDoctorId() == g_book.docId &&
                    strEquals(a.getDate(), g_book.date) &&
                    strEquals(a.getTimeSlot(), slots[s]) &&
                    !a.isCancelled())
                {
                    taken = true; break;
                }
            }

            bool hover = (!taken &&
                mouse.x >= (int)slotX && mouse.x <= (int)(slotX + 120) &&
                mouse.y >= (int)slotY && mouse.y <= (int)(slotY + 48));

            sf::Color fill = taken ? sf::Color(20, 30, 55)
                : hover ? C::TEAL_DIM
                : sf::Color(0, 60, 50);
            sf::Color border = taken ? C::DIVIDER : C::TEAL;

            sf::RectangleShape slot(sf::Vector2f(120.f, 48.f));
            slot.setPosition(slotX, slotY);
            slot.setFillColor(fill);
            slot.setOutlineThickness(1.5f);
            slot.setOutlineColor(border);
            win.draw(slot);

            sf::Color txtCol = taken ? C::TEXT_DIM : C::TEXT_HI;
            sf::Text slotTxt = makeText(slots[s], font, 15, txtCol, 0.f, slotY + 14.f);
            centreX(slotTxt, slotX, 120.f); win.draw(slotTxt);
            if (taken) {
                sf::Text x = makeText("Taken", font, 11, C::TEXT_DIM, 0.f, slotY + 32.f);
                centreX(x, slotX, 120.f); win.draw(x);
            }

            if (hover && ev.type == sf::Event::MouseButtonReleased &&
                ev.mouseButton.button == sf::Mouse::Left)
            {
                try {
                    Appointment probe(0, g_patient->getId(), g_book.docId,
                        g_book.date, slots[s], "pending");
                    for (int i = 0; i < appointments.size(); i++)
                        if (appointments[i] == probe)
                            throw SlotUnavailableException(slots[s], g_book.date);

                    int newAid = appointments.getMaxId() + 1;
                    int newBid = bills.getMaxId() + 1;
                    char today[15]; getTodayDate(today, 15);

                    Appointment appt(newAid, g_patient->getId(),
                        g_book.docId, g_book.date, slots[s], "pending");
                    appointments.add(appt);
                    FileHandler::appendAppointment(appt, FILE_APPOINTMENTS);

                    Bill bill(newBid, g_patient->getId(), newAid,
                        doc->getFee(), "unpaid", today);
                    bills.add(bill);
                    FileHandler::appendBill(bill, FILE_BILLS);

                    g_book.reset(); g_bookStateInit = false;
                    clearUIMsg();
                    return SCR_PATIENT_MENU;
                }
                catch (SlotUnavailableException& e) { setUIMsg(e.what(), true); }
                catch (InsufficientFundsException& e) { setUIMsg(e.what(), true); }
            }

            slotX += 136.f;
            if (s == 3) { slotX = 60.f; slotY += 64.f; }
        }
    }

    if (drawBackButton(win, font, mouse, ev)) {
        if (g_book.step == BK_SPEC) {
            g_book.reset(); g_bookStateInit = false;
            clearUIMsg();
            return SCR_PATIENT_MENU;
        }
        g_book.step = (BookStep)((int)g_book.step - 1);
        g_book.fieldInit = false;
        clearUIMsg();
    }

    return SCR_BOOK_APPT;
}

//  2. SCR_CANCEL_APPT
static InputField g_cancelField;
static bool       g_cancelFieldInit = false;

static Screen drawCancelAppt(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse)
{
    if (!g_cancelFieldInit) {
        g_cancelField = InputField(60.f, 560.f, 220.f, 44.f,
            "Appointment ID", font, false, 15, 6);
        g_cancelFieldInit = true;
    }

    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawUIMsg(win, font);

    win.draw(makeText("Cancel Appointment", font, 26, C::TEXT_HI, 60.f, 72.f));
    sf::RectangleShape acc(sf::Vector2f(36.f, 3.f));
    acc.setPosition(60.f, 106.f); acc.setFillColor(C::TEAL); win.draw(acc);

    float colX[5] = { 60.f, 120.f, 310.f, 500.f, 660.f };
    const char* hdrs[5] = { "ID", "Doctor", "Specialization", "Date", "Slot" };
    drawTableHeader(win, font, colX, hdrs, 5, 120.f);

    int cnt = 0;
    float rowY = 148.f;
    char rowBufs[100][5][32];

    for (int i = 0; i < appointments.size() && cnt < 100; i++) {
        Appointment& a = appointments[i];
        if (a.getPatientId() != g_patient->getId() || !a.isPending()) continue;
        Doctor* doc = doctors.findById(a.getDoctorId());

        intToStr(a.getId(), rowBufs[cnt][0], 32);
        copyStr(rowBufs[cnt][1], doc ? doc->getName() : "Unknown", 32);
        copyStr(rowBufs[cnt][2], doc ? doc->getSpecialization() : "?", 32);
        copyStr(rowBufs[cnt][3], a.getDate(), 32);
        copyStr(rowBufs[cnt][4], a.getTimeSlot(), 32);

        const char* vals[5] = {
            rowBufs[cnt][0], rowBufs[cnt][1], rowBufs[cnt][2],
            rowBufs[cnt][3], rowBufs[cnt][4]
        };
        sf::Color cols[5] = { C::TEXT_DIM, C::TEXT_HI, C::TEAL,
                              C::TEXT_MID, C::TEXT_MID };
        drawTableRow(win, font, colX, vals, cols, 5, rowY, cnt);
        rowY += 27.f; cnt++;
    }

    if (cnt == 0) {
        sf::Text e = makeText("No pending appointments.", font, 16, C::TEXT_DIM, 0.f, 300.f);
        centreX(e, 0.f, (float)WIN_W); win.draw(e);
    }

    win.draw(makeText("Enter Appointment ID to cancel:", font, 14,
        C::TEXT_DIM, 60.f, 542.f));
    g_cancelField.update();
    g_cancelField.handleEvent(ev, mouse);
    g_cancelField.draw(win);

    bool ch = (mouse.x >= 296 && mouse.x <= 530 &&
        mouse.y >= 560 && mouse.y <= 604);
    sf::RectangleShape cb(sf::Vector2f(234.f, 44.f));
    cb.setPosition(296.f, 560.f);
    cb.setFillColor(ch ? sf::Color(160, 30, 30) : sf::Color(80, 15, 15));
    cb.setOutlineThickness(1.5f); cb.setOutlineColor(C::RED);
    win.draw(cb);
    sf::Text ct = makeText("CANCEL APPOINTMENT", font, 13, C::TEXT_HI, 0.f, 573.f);
    centreX(ct, 296.f, 234.f); win.draw(ct);

    if (ch && ev.type == sf::Event::MouseButtonReleased &&
        ev.mouseButton.button == sf::Mouse::Left)
    {
        int aid;
        if (!parseInt(g_cancelField.getText(), aid)) {
            setUIMsg("Enter a valid numeric Appointment ID.", true);
            playErrorSound();
        }
        else {
            Appointment* appt = appointments.findById(aid);
            if (!appt || appt->getPatientId() != g_patient->getId() || !appt->isPending()) {
                setUIMsg("Invalid appointment ID.", true);
                playErrorSound();
            }
            else {
                Doctor* doc = doctors.findById(appt->getDoctorId());
                float   fee = doc ? doc->getFee() : 0.f;

                appt->setStatus("cancelled");
                FileHandler::rewriteAppointments(appointments, FILE_APPOINTMENTS);

                *g_patient += fee;
                FileHandler::rewritePatients(patients, FILE_PATIENTS);

                for (int i = 0; i < bills.size(); i++) {
                    if (bills[i].getAppointmentId() == aid && bills[i].isUnpaid()) {
                        bills[i].setStatus("cancelled"); break;
                    }
                }
                FileHandler::rewriteBills(bills, FILE_BILLS);

                char m[64] = "Cancelled. PKR ";
                char fb[20]; ftoaBuf(fee, fb, 20);
                strAppend(m, fb, 64); strAppend(m, " refunded.", 64);
                setUIMsg(m, false);
                g_cancelField.clear();
            }
        }
    }

    if (drawBackButton(win, font, mouse, ev)) {
        g_cancelField.clear(); g_cancelFieldInit = false;
        clearUIMsg(); return SCR_PATIENT_MENU;
    }
    return SCR_CANCEL_APPT;
}

//  3. SCR_VIEW_RECORDS
static Screen drawViewRecords(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    Button& backBtn)
{
    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);

    win.draw(makeText("My Medical Records", font, 26, C::TEXT_HI, 60.f, 72.f));
    drawDivider(win, 110.f);

    int idx[200]; int cnt = 0;
    for (int i = 0; i < prescriptions.size() && cnt < 200; i++)
        if (prescriptions[i].getPatientId() == g_patient->getId())
            idx[cnt++] = i;

    for (int i = 0; i < cnt - 1; i++)
        for (int j = 0; j < cnt - i - 1; j++)
            if (compareDates(prescriptions[idx[j]].getDate(),
                prescriptions[idx[j + 1]].getDate()) < 0)
            {
                int t = idx[j]; idx[j] = idx[j + 1]; idx[j + 1] = t;
            }

    float colX[4] = { 60.f, 160.f, 360.f, 730.f };
    const char* hdrs[4] = { "Date", "Doctor", "Medicines", "Notes" };
    drawTableHeader(win, font, colX, hdrs, 4, 120.f);

    int visible = 18, startRow = g_scrollOffset;
    if (startRow > cnt - visible) startRow = cnt - visible;
    if (startRow < 0) startRow = 0;

    float rowY = 148.f;
    for (int i = startRow; i < cnt && i < startRow + visible; i++) {
        Prescription& pr = prescriptions[idx[i]];
        Doctor* doc = doctors.findById(pr.getDoctorId());

        char dateBuf[15], docBuf[50], medBuf[80], notesBuf[80];
        copyStr(dateBuf, pr.getDate(), 15);
        copyStr(docBuf, doc ? doc->getName() : "Unknown", 50);
        copyStr(medBuf, pr.getMedicines(), 80);
        copyStr(notesBuf, pr.getNotes(), 80);

        const char* vals[4] = { dateBuf, docBuf, medBuf, notesBuf };
        sf::Color   cols[4] = { C::TEXT_MID, C::TEXT_HI, C::TEAL, C::TEXT_DIM };
        drawTableRow(win, font, colX, vals, cols, 4, rowY, i);
        rowY += 27.f;
    }

    if (cnt == 0) {
        sf::Text e = makeText("No medical records found.", font, 16, C::TEXT_DIM, 0.f, 300.f);
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
    if (backBtn.handleEvent(ev, mouse)) return SCR_PATIENT_MENU;

    return SCR_VIEW_RECORDS;
}

//  4. SCR_VIEW_BILLS
static Screen drawViewBills(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse,
    Button& backBtn)
{
    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);

    win.draw(makeText("My Bills", font, 26, C::TEXT_HI, 60.f, 72.f));
    drawDivider(win, 110.f);

    float colX[5] = { 60.f, 140.f, 310.f, 480.f, 640.f };
    const char* hdrs[5] = { "Bill ID", "Appt ID", "Amount (PKR)", "Status", "Date" };
    drawTableHeader(win, font, colX, hdrs, 5, 120.f);

    int cnt = 0; float totalUnpaid = 0.f;
    float rowY = 148.f;
    char rowBufs[200][5][32];

    for (int i = 0; i < bills.size() && cnt < 200; i++) {
        Bill& b = bills[i];
        if (b.getPatientId() != g_patient->getId()) continue;

        intToStr(b.getId(), rowBufs[cnt][0], 32);
        intToStr(b.getAppointmentId(), rowBufs[cnt][1], 32);
        ftoaBuf(b.getAmount(), rowBufs[cnt][2], 32);
        copyStr(rowBufs[cnt][3], b.getStatus(), 32);
        copyStr(rowBufs[cnt][4], b.getDate(), 32);

        sf::Color statusCol = b.isPaid() ? C::TEAL
            : b.isCancelled() ? C::TEXT_DIM
            : C::RED;
        const char* vals[5] = {
            rowBufs[cnt][0], rowBufs[cnt][1], rowBufs[cnt][2],
            rowBufs[cnt][3], rowBufs[cnt][4]
        };
        sf::Color cols[5] = { C::TEXT_DIM, C::TEXT_DIM, C::TEXT_MID,
                              statusCol, C::TEXT_DIM };
        drawTableRow(win, font, colX, vals, cols, 5, rowY, cnt);

        if (b.isUnpaid()) totalUnpaid += b.getAmount();
        rowY += 27.f; cnt++;
    }

    if (cnt == 0) {
        sf::Text e = makeText("No bills found.", font, 16, C::TEXT_DIM, 0.f, 300.f);
        centreX(e, 0.f, (float)WIN_W); win.draw(e);
    }

    char totBuf[64] = "Total Outstanding: PKR ";
    char fb[20]; ftoaBuf(totalUnpaid, fb, 20);
    strAppend(totBuf, fb, 64);
    win.draw(makeText(totBuf, font, 15,
        totalUnpaid > 0 ? C::RED : C::TEAL,
        60.f, (float)WIN_H - 82.f));

    backBtn.setPosition(60.f, (float)WIN_H - 60.f);
    backBtn.setSize(140.f, 40.f);
    backBtn.setLabel("< Back");
    backBtn.setStyle(Button::GHOST);
    backBtn.update(mouse); backBtn.draw(win);
    if (backBtn.handleEvent(ev, mouse)) return SCR_PATIENT_MENU;

    return SCR_VIEW_BILLS;
}

//  5. SCR_PAY_BILL
static InputField g_payField;
static bool       g_payFieldInit = false;

static Screen drawPayBill(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse)
{
    if (!g_payFieldInit) {
        g_payField = InputField(60.f, 560.f, 200.f, 44.f,
            "Bill ID", font, false, 15, 6);
        g_payFieldInit = true;
    }

    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawUIMsg(win, font);

    win.draw(makeText("Pay Bill", font, 26, C::TEXT_HI, 60.f, 72.f));
    sf::RectangleShape acc(sf::Vector2f(36.f, 3.f));
    acc.setPosition(60.f, 106.f); acc.setFillColor(C::TEAL); win.draw(acc);

    char balBuf[64] = "Current Balance: PKR ";
    char fb[20]; ftoaBuf(g_patient->getBalance(), fb, 20);
    strAppend(balBuf, fb, 64);
    win.draw(makeText(balBuf, font, 14, C::TEAL, 60.f, 116.f));

    float colX[4] = { 60.f, 140.f, 320.f, 500.f };
    const char* hdrs[4] = { "Bill ID", "Appt ID", "Amount (PKR)", "Date" };
    drawTableHeader(win, font, colX, hdrs, 4, 140.f);

    int cnt = 0;
    float rowY = 168.f;
    char rowBufs[100][4][32];

    for (int i = 0; i < bills.size() && cnt < 100; i++) {
        Bill& b = bills[i];
        if (b.getPatientId() != g_patient->getId() || !b.isUnpaid()) continue;

        intToStr(b.getId(), rowBufs[cnt][0], 32);
        intToStr(b.getAppointmentId(), rowBufs[cnt][1], 32);
        ftoaBuf(b.getAmount(), rowBufs[cnt][2], 32);
        copyStr(rowBufs[cnt][3], b.getDate(), 32);

        const char* vals[4] = {
            rowBufs[cnt][0], rowBufs[cnt][1],
            rowBufs[cnt][2], rowBufs[cnt][3]
        };
        sf::Color cols[4] = { C::TEXT_DIM, C::TEXT_DIM, C::RED, C::TEXT_MID };
        drawTableRow(win, font, colX, vals, cols, 4, rowY, cnt);
        rowY += 27.f; cnt++;
    }
    if (cnt == 0) {
        sf::Text e = makeText("No unpaid bills.", font, 16, C::TEAL, 0.f, 300.f);
        centreX(e, 0.f, (float)WIN_W); win.draw(e);
    }

    win.draw(makeText("Enter Bill ID to pay:", font, 14, C::TEXT_DIM, 60.f, 542.f));
    g_payField.update();
    g_payField.handleEvent(ev, mouse);
    g_payField.draw(win);

    bool ph = (mouse.x >= 276 && mouse.x <= 476 &&
        mouse.y >= 560 && mouse.y <= 604);
    sf::RectangleShape pb(sf::Vector2f(200.f, 44.f));
    pb.setPosition(276.f, 560.f);
    pb.setFillColor(ph ? C::TEAL_DIM : sf::Color(0, 70, 60));
    pb.setOutlineThickness(1.5f); pb.setOutlineColor(C::TEAL);
    win.draw(pb);
    sf::Text pt = makeText("PAY NOW", font, 14, C::TEXT_HI, 0.f, 573.f);
    centreX(pt, 276.f, 200.f); win.draw(pt);

    if (ph && ev.type == sf::Event::MouseButtonReleased &&
        ev.mouseButton.button == sf::Mouse::Left)
    {
        int bid;
        if (!parseInt(g_payField.getText(), bid)) {
            setUIMsg("Enter a valid numeric Bill ID.", true);
            playErrorSound();
        }
        else {
            Bill* bill = bills.findById(bid);
            if (!bill || bill->getPatientId() != g_patient->getId() || !bill->isUnpaid()) {
                setUIMsg("Invalid bill ID.", true);
                playErrorSound();
            }
            else {
                try {
                    if (g_patient->getBalance() < bill->getAmount())
                        throw InsufficientFundsException(
                            g_patient->getBalance(), bill->getAmount());

                    *g_patient -= bill->getAmount();
                    bill->setStatus("paid");
                    FileHandler::rewritePatients(patients, FILE_PATIENTS);
                    FileHandler::rewriteBills(bills, FILE_BILLS);

                    char m[64] = "Paid. Remaining balance: PKR ";
                    ftoaBuf(g_patient->getBalance(), fb, 20);
                    strAppend(m, fb, 64);
                    setUIMsg(m, false);
                    g_payField.clear();
                }
                catch (InsufficientFundsException& e) { setUIMsg(e.what(), true); }
            }
        }
    }
    if (drawBackButton(win, font, mouse, ev)) {
        g_payField.clear(); g_payFieldInit = false;
        clearUIMsg(); return SCR_PATIENT_MENU;
    }
    return SCR_PAY_BILL;
}

//  6. SCR_TOPUP
static InputField g_topupField;
static bool       g_topupFieldInit = false;

static Screen drawTopUp(sf::RenderWindow& win, sf::Font& font,
    const sf::Event& ev, sf::Vector2i mouse)
{
    if (!g_topupFieldInit) {
        g_topupField = InputField((float)WIN_W / 2.f - 180.f, 320.f, 360.f, 48.f,
            "Amount (PKR)", font, false, 15, 10);
        g_topupFieldInit = true;
    }

    sf::RectangleShape bg(sf::Vector2f((float)WIN_W, (float)WIN_H));
    bg.setFillColor(C::BG_DEEP); win.draw(bg);
    drawTopBar(win, font);
    drawUIMsg(win, font);

    float cx = (float)WIN_W / 2.f;
    drawCard(win, cx - 220.f, 140.f, 440.f, 380.f);

    sf::Text title = makeText("Top Up Balance", font, 24, C::TEXT_HI, 0.f, 168.f);
    centreX(title, cx - 220.f, 440.f); win.draw(title);

    char balBuf[64] = "Current Balance: PKR ";
    char fb[20]; ftoaBuf(g_patient->getBalance(), fb, 20);
    strAppend(balBuf, fb, 64);
    sf::Text balTxt = makeText(balBuf, font, 15, C::TEAL, 0.f, 214.f);
    centreX(balTxt, cx - 220.f, 440.f); win.draw(balTxt);

    win.draw(makeText("Enter amount to add:", font, 14, C::TEXT_DIM,
        cx - 180.f, 302.f));

    g_topupField.update();
    g_topupField.handleEvent(ev, mouse);
    g_topupField.draw(win);

    bool ah = (mouse.x >= (int)(cx - 180) && mouse.x <= (int)(cx + 180) &&
        mouse.y >= 386 && mouse.y <= 430);
    sf::RectangleShape ab(sf::Vector2f(360.f, 44.f));
    ab.setPosition(cx - 180.f, 386.f);
    ab.setFillColor(ah ? C::TEAL_DIM : sf::Color(0, 70, 60));
    ab.setOutlineThickness(1.5f); ab.setOutlineColor(C::TEAL);
    win.draw(ab);
    sf::Text at = makeText("ADD FUNDS", font, 15, C::TEXT_HI, 0.f, 399.f);
    centreX(at, cx - 180.f, 360.f); win.draw(at);

    bool bh = (mouse.x >= (int)(cx - 180) && mouse.x <= (int)(cx + 180) &&
        mouse.y >= 440 && mouse.y <= 476);
    sf::RectangleShape bb(sf::Vector2f(360.f, 36.f));
    bb.setPosition(cx - 180.f, 440.f);
    bb.setFillColor(bh ? C::BG_CARD : sf::Color(0, 0, 0, 0));
    bb.setOutlineThickness(1.f); bb.setOutlineColor(C::SLATE);
    win.draw(bb);
    sf::Text bt = makeText("Back", font, 13, C::TEXT_DIM, 0.f, 450.f);
    centreX(bt, cx - 180.f, 360.f); win.draw(bt);

    if (ah && ev.type == sf::Event::MouseButtonReleased &&
        ev.mouseButton.button == sf::Mouse::Left)
    {
        float amount;
        if (!parseFloat(g_topupField.getText(), amount)) {
            setUIMsg("Enter a valid amount.", true);
            playErrorSound();
        }
        else {
            try {
                if (!Validator::validatePositiveFloat(amount))
                    throw InvalidInputException("Amount must be greater than 0.");
                *g_patient += amount;
                FileHandler::rewritePatients(patients, FILE_PATIENTS);
                char m[64] = "Balance updated. New balance: PKR ";
                ftoaBuf(g_patient->getBalance(), fb, 20);
                strAppend(m, fb, 64);
                setUIMsg(m, false);
                g_topupField.clear();
            }
            catch (InvalidInputException& e) { setUIMsg(e.what(), true); }
        }
    }

    if (bh && ev.type == sf::Event::MouseButtonReleased &&
        ev.mouseButton.button == sf::Mouse::Left)
    {
        g_topupField.clear(); g_topupFieldInit = false;
        clearUIMsg(); return SCR_PATIENT_MENU;
    }

    return SCR_TOPUP;
}