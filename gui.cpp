#include "GUI.h"
#include "HospitalSystem.h"
#include <SFML/Graphics.hpp>
#include <cstring>
#include <iomanip>
#include <sstream>
using namespace std;

// ============================================================
// COLORS
// ============================================================
static sf::Color COL_BG        (245, 247, 250);
static sf::Color COL_HEADER    ( 30,  80, 160);
static sf::Color COL_CARD      (255, 255, 255);
static sf::Color COL_BTN       ( 30,  80, 160);
static sf::Color COL_BTN_HOVER ( 20,  60, 130);
static sf::Color COL_BTN_RED   (200,  50,  50);
static sf::Color COL_BTN_GREEN ( 40, 150,  80);
static sf::Color COL_BTN_GRAY  (150, 150, 150);
static sf::Color COL_TEXT_WHITE(255, 255, 255);
static sf::Color COL_TEXT_DARK ( 30,  30,  30);
static sf::Color COL_INPUT_BG  (245, 245, 245);
static sf::Color COL_INPUT_ACT (220, 235, 255);
static sf::Color COL_BORDER    (180, 180, 180);

// ============================================================
// BUTTON
// ============================================================
Button::Button() : hovered(false) {}

void Button::setup(float x, float y, float w, float h,
                   const char* text, sf::Font& font,
                   sf::Color normal, sf::Color hover) {
    normalColor = normal;
    hoverColor  = hover;

    box.setPosition(x, y);
    box.setSize(sf::Vector2f(w, h));
    box.setFillColor(normal);
    box.setOutlineColor(sf::Color(0,0,0,30));
    box.setOutlineThickness(1);

    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(16);
    label.setFillColor(COL_TEXT_WHITE);

    // center text in button
    sf::FloatRect bounds = label.getLocalBounds();
    label.setPosition(
        x + (w - bounds.width)  / 2 - bounds.left,
        y + (h - bounds.height) / 2 - bounds.top - 2
    );
}

void Button::setText(const char* text) {
    label.setString(text);
    sf::FloatRect b  = box.getGlobalBounds();
    sf::FloatRect lb = label.getLocalBounds();
    label.setPosition(
        b.left + (b.width  - lb.width)  / 2 - lb.left,
        b.top  + (b.height - lb.height) / 2 - lb.top - 2
    );
}

void Button::update(sf::Vector2i mousePos) {
    hovered = box.getGlobalBounds().contains(
                  (float)mousePos.x, (float)mousePos.y);
    box.setFillColor(hovered ? hoverColor : normalColor);
}

bool Button::isClicked(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left) {
        return box.getGlobalBounds().contains(
            (float)event.mouseButton.x,
            (float)event.mouseButton.y);
    }
    return false;
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(box);
    window.draw(label);
}

// ============================================================
// INPUTBOX
// ============================================================
InputBox::InputBox() : length(0), active(false), isPassword(false) {
    buffer[0] = '\0';
}

void InputBox::setup(float x, float y, float w, float h,
                     const char* hint, sf::Font& font, bool password) {
    isPassword = password;

    box.setPosition(x, y);
    box.setSize(sf::Vector2f(w, h));
    box.setFillColor(COL_INPUT_BG);
    box.setOutlineColor(COL_BORDER);
    box.setOutlineThickness(1);

    text.setFont(font);
    text.setCharacterSize(15);
    text.setFillColor(COL_TEXT_DARK);
    text.setPosition(x + 10, y + (h - 18) / 2);

    placeholder.setFont(font);
    placeholder.setString(hint);
    placeholder.setCharacterSize(15);
    placeholder.setFillColor(sf::Color(180, 180, 180));
    placeholder.setPosition(x + 10, y + (h - 18) / 2);
}

void InputBox::setActive(bool a) {
    active = a;
    box.setFillColor(active ? COL_INPUT_ACT : COL_INPUT_BG);
    box.setOutlineColor(active ? COL_BTN : COL_BORDER);
    box.setOutlineThickness(active ? 2 : 1);
}

bool InputBox::getActive() const { return active; }

void InputBox::clicked(sf::Vector2i mousePos) {
    setActive(box.getGlobalBounds().contains(
        (float)mousePos.x, (float)mousePos.y));
}

void InputBox::handleEvent(sf::Event& event) {
    if (!active) return;
    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode == 8) { // backspace
            if (length > 0) buffer[--length] = '\0';
        } else if (event.text.unicode == 13) { // enter — ignore
        } else if (event.text.unicode < 128 && length < 99) {
            buffer[length++] = (char)event.text.unicode;
            buffer[length]   = '\0';
        }
        // update display
        if (isPassword) {
            char stars[100];
            for (int i = 0; i < length; i++) stars[i] = '*';
            stars[length] = '\0';
            text.setString(stars);
        } else {
            text.setString(buffer);
        }
    }
}

const char* InputBox::getValue() const { return buffer; }

void InputBox::clear() {
    length    = 0;
    buffer[0] = '\0';
    text.setString("");
}

void InputBox::draw(sf::RenderWindow& window) {
    window.draw(box);
    if (length == 0)
        window.draw(placeholder);
    else
        window.draw(text);
}

// ============================================================
// LABEL
// ============================================================
Label::Label() {}

void Label::setup(float x, float y, const char* str,
                  sf::Font& font, int size, sf::Color color) {
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
}

void Label::setText(const char* str) {
    text.setString(str);
}

void Label::draw(sf::RenderWindow& window) {
    window.draw(text);
}

// ============================================================
// GUI SYSTEM CONSTRUCTOR
// ============================================================
GuiSystem::GuiSystem()
    : currentScreen(SCREEN_MAIN),
      returnScreen(SCREEN_MAIN),
      loginRole(0),
      failedAttempts(0),
      locked(false),
      loggedPatientID(-1),
      loggedDoctorID(-1) {

    window.create(sf::VideoMode(700, 600),
                  "MediCore Hospital Management System",
                  sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

   if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
    if (!font.loadFromFile("C:/Windows/Fonts/calibri.ttf")) {
        font.loadFromFile("C:/Windows/Fonts/tahoma.ttf");
    }
}

    // background
    background.setSize(sf::Vector2f(700, 600));
    background.setFillColor(COL_BG);

    // header bar
    headerBar.setSize(sf::Vector2f(700, 70));
    headerBar.setFillColor(COL_HEADER);
    headerBar.setPosition(0, 0);

    // card
    cardBox.setSize(sf::Vector2f(500, 420));
    cardBox.setFillColor(COL_CARD);
    cardBox.setOutlineColor(sf::Color(200, 200, 200));
    cardBox.setOutlineThickness(1);
    cardBox.setPosition(100, 110);

    messageText[0] = '\0';
    setupMainScreen();
}

// ============================================================
// SETUP SCREENS
// ============================================================
void GuiSystem::setupMainScreen() {
    titleLbl.setup(0, 15, "MediCore Hospital Management System",
                   font, 22, COL_TEXT_WHITE);
    // center title
    subtitleLbl.setup(220, 48, "Your Health, Our Priority",
                      font, 13, sf::Color(180, 210, 255));

    float cx = 200, bw = 300, bh = 52, gap = 18;
    float startY = 160;

    patientBtn.setup(cx, startY,           bw, bh, "Login as Patient",
                     font, COL_BTN, COL_BTN_HOVER);
    doctorBtn .setup(cx, startY+bh+gap,    bw, bh, "Login as Doctor",
                     font, COL_BTN, COL_BTN_HOVER);
    adminBtn  .setup(cx, startY+(bh+gap)*2,bw, bh, "Login as Admin",
                     font, COL_BTN_GREEN,
                     sf::Color(30,120,60));
    exitBtn   .setup(cx, startY+(bh+gap)*3,bw, bh, "Exit",
                     font, COL_BTN_RED,
                     sf::Color(160,30,30));
}

void GuiSystem::setupLoginScreen(int role) {
    loginRole      = role;
    failedAttempts = 0;
    locked         = false;

    const char* titles[4] = {"", "Patient Login",
                               "Doctor Login", "Admin Login"};
    loginTitleLbl.setup(250, 15, titles[role],
                        font, 22, COL_TEXT_WHITE);
    loginErrorLbl.setup(150, 430, "",
                        font, 14, sf::Color(200, 50, 50));

    idLbl  .setup(150, 150, "User ID:",   font, 15, COL_TEXT_DARK);
    passLbl.setup(150, 240, "Password:", font, 15, COL_TEXT_DARK);

    idBox  .setup(150, 175, 400, 42, "Enter your ID",
                  font, false);
    passBox.setup(150, 265, 400, 42, "Enter your password",
                  font, true);

    loginBtn.setup(200, 340, 140, 44, "Login",
                   font, COL_BTN, COL_BTN_HOVER);
    backBtn .setup(360, 340, 140, 44, "Back",
                   font, COL_BTN_GRAY,
                   sf::Color(120,120,120));

    idBox.clear();
    passBox.clear();
    idBox.setActive(true);
    passBox.setActive(false);
}

void GuiSystem::setupPatientMenu() {
    patientNameLbl.setup(20, 15, "Welcome, Patient",
                         font, 20, COL_TEXT_WHITE);
    patientBalLbl .setup(20, 44, "Balance: PKR 0.00",
                         font, 13, sf::Color(180,220,255));

    float cx = 150, bw = 380, bh = 44, gap = 10;
    float sy = 130;

    pBookBtn   .setup(cx, sy,              bw,bh,"Book Appointment",     font,COL_BTN,        COL_BTN_HOVER);
    pCancelBtn .setup(cx, sy+(bh+gap),     bw,bh,"Cancel Appointment",   font,COL_BTN,        COL_BTN_HOVER);
    pViewAppBtn.setup(cx, sy+(bh+gap)*2,   bw,bh,"View My Appointments", font,COL_BTN,        COL_BTN_HOVER);
    pViewRecBtn.setup(cx, sy+(bh+gap)*3,   bw,bh,"View Medical Records", font,COL_BTN,        COL_BTN_HOVER);
    pViewBillBtn.setup(cx,sy+(bh+gap)*4,   bw,bh,"View My Bills",        font,COL_BTN,        COL_BTN_HOVER);
    pPayBillBtn .setup(cx,sy+(bh+gap)*5,   bw,bh,"Pay Bill",             font,COL_BTN_GREEN,  sf::Color(30,120,60));
    pTopUpBtn  .setup(cx, sy+(bh+gap)*6,   bw,bh,"Top Up Balance",       font,COL_BTN_GREEN,  sf::Color(30,120,60));
    pLogoutBtn .setup(cx, sy+(bh+gap)*7,   bw,bh,"Logout",               font,COL_BTN_RED,    sf::Color(160,30,30));
}

void GuiSystem::setupDoctorMenu() {
    doctorNameLbl.setup(20, 15, "Welcome, Doctor",
                        font, 20, COL_TEXT_WHITE);

    float cx = 150, bw = 380, bh = 48, gap = 12;
    float sy = 130;

    dTodayBtn    .setup(cx, sy,            bw,bh,"View Today's Appointments", font,COL_BTN,       COL_BTN_HOVER);
    dCompleteBtn .setup(cx, sy+(bh+gap),   bw,bh,"Mark Appointment Complete", font,COL_BTN_GREEN, sf::Color(30,120,60));
    dNoShowBtn   .setup(cx, sy+(bh+gap)*2, bw,bh,"Mark Appointment No-Show",  font,COL_BTN_RED,   sf::Color(160,30,30));
    dPrescribeBtn.setup(cx, sy+(bh+gap)*3, bw,bh,"Write Prescription",        font,COL_BTN,       COL_BTN_HOVER);
    dHistoryBtn  .setup(cx, sy+(bh+gap)*4, bw,bh,"View Patient History",      font,COL_BTN,       COL_BTN_HOVER);
    dLogoutBtn   .setup(cx, sy+(bh+gap)*5, bw,bh,"Logout",                    font,COL_BTN_GRAY,  sf::Color(120,120,120));
}

void GuiSystem::setupAdminMenu() {
    adminTitleLbl.setup(260, 20, "Admin Panel",
                        font, 22, COL_TEXT_WHITE);

    float x1 = 110, x2 = 370, bw = 210, bh = 42, gap = 10;
    float sy = 115;

    aAddDocBtn  .setup(x1, sy,           bw,bh,"Add Doctor",         font,COL_BTN_GREEN,sf::Color(30,120,60));
    aRemDocBtn  .setup(x2, sy,           bw,bh,"Remove Doctor",      font,COL_BTN_RED,  sf::Color(160,30,30));
    aViewPatBtn .setup(x1, sy+(bh+gap),  bw,bh,"View All Patients",  font,COL_BTN,      COL_BTN_HOVER);
    aViewDocBtn .setup(x2, sy+(bh+gap),  bw,bh,"View All Doctors",   font,COL_BTN,      COL_BTN_HOVER);
    aViewAppBtn .setup(x1, sy+(bh+gap)*2,bw,bh,"View Appointments",  font,COL_BTN,      COL_BTN_HOVER);
    aViewBillBtn.setup(x2, sy+(bh+gap)*2,bw,bh,"View Unpaid Bills",  font,COL_BTN,      COL_BTN_HOVER);
    aDischargeBtn.setup(x1,sy+(bh+gap)*3,bw,bh,"Discharge Patient",  font,COL_BTN_RED,  sf::Color(160,30,30));
    aSecLogBtn  .setup(x2, sy+(bh+gap)*3,bw,bh,"Security Log",       font,COL_BTN,      COL_BTN_HOVER);
    aReportBtn  .setup(x1, sy+(bh+gap)*4,bw,bh,"Daily Report",       font,COL_BTN_GREEN,sf::Color(30,120,60));
    aLogoutBtn  .setup(x2, sy+(bh+gap)*4,bw,bh,"Logout",             font,COL_BTN_GRAY, sf::Color(120,120,120));
}

void GuiSystem::setupPromptScreen(const char* title,
                                   const char* hint1,
                                   const char* hint2,
                                   int action,
                                   bool twoInputs) {
    promptAction = action;
    promptData1[0] = '\0';
    promptData2[0] = '\0';

    promptTitleLbl.setup(150, 15, title, font, 20, COL_TEXT_WHITE);

    promptLbl1.setup(150, 145, hint1, font, 14, COL_TEXT_DARK);
    promptBox1.setup(150, 168, 400, 42, hint1, font, false);
    promptBox1.clear();
    promptBox1.setActive(true);

    if (twoInputs) {
        promptLbl2.setup(150, 230, hint2, font, 14, COL_TEXT_DARK);
        promptBox2.setup(150, 253, 400, 42, hint2, font, false);
        promptBox2.clear();
        promptBox2.setActive(false);
    }

    float confirmY = twoInputs ? 320 : 240;
    promptConfirmBtn.setup(200, confirmY, 130, 44, "Confirm",
                           font, COL_BTN, COL_BTN_HOVER);
    promptBackBtn   .setup(360, confirmY, 130, 44, "Back",
                           font, COL_BTN_GRAY,
                           sf::Color(120,120,120));
}

void GuiSystem::showMessage(const char* msg, Screen returnTo) {
    strncpy(messageText, msg, 499);
    messageText[499] = '\0';
    returnScreen  = returnTo;
    currentScreen = SCREEN_MESSAGE;

    messageLbl .setup(100, 200, msg,  font, 15, COL_TEXT_DARK);
    messageOkBtn.setup(275, 340, 150, 44, "OK",
                       font, COL_BTN, COL_BTN_HOVER);
}

// ============================================================
// DRAW SCREENS
// ============================================================
void GuiSystem::drawMainScreen() {
    window.draw(background);
    window.draw(headerBar);
    window.draw(cardBox);
    titleLbl   .draw(window);
    subtitleLbl.draw(window);
    patientBtn .draw(window);
    doctorBtn  .draw(window);
    adminBtn   .draw(window);
    exitBtn    .draw(window);
}

void GuiSystem::drawLoginScreen() {
    window.draw(background);
    window.draw(headerBar);
    window.draw(cardBox);
    loginTitleLbl.draw(window);
    idLbl        .draw(window);
    passLbl      .draw(window);
    idBox        .draw(window);
    passBox      .draw(window);
    loginBtn     .draw(window);
    backBtn      .draw(window);
    loginErrorLbl.draw(window);
}

void GuiSystem::drawPatientMenu() {
    window.draw(background);
    window.draw(headerBar);
    patientNameLbl.draw(window);
    patientBalLbl .draw(window);
    pBookBtn      .draw(window);
    pCancelBtn    .draw(window);
    pViewAppBtn   .draw(window);
    pViewRecBtn   .draw(window);
    pViewBillBtn  .draw(window);
    pPayBillBtn   .draw(window);
    pTopUpBtn     .draw(window);
    pLogoutBtn    .draw(window);
}

void GuiSystem::drawDoctorMenu() {
    window.draw(background);
    window.draw(headerBar);
    doctorNameLbl .draw(window);
    dTodayBtn     .draw(window);
    dCompleteBtn  .draw(window);
    dNoShowBtn    .draw(window);
    dPrescribeBtn .draw(window);
    dHistoryBtn   .draw(window);
    dLogoutBtn    .draw(window);
}

void GuiSystem::drawAdminMenu() {
    window.draw(background);
    window.draw(headerBar);
    adminTitleLbl .draw(window);
    aAddDocBtn    .draw(window);
    aRemDocBtn    .draw(window);
    aViewPatBtn   .draw(window);
    aViewDocBtn   .draw(window);
    aViewAppBtn   .draw(window);
    aViewBillBtn  .draw(window);
    aDischargeBtn .draw(window);
    aSecLogBtn    .draw(window);
    aReportBtn    .draw(window);
    aLogoutBtn    .draw(window);
}

void GuiSystem::drawMessageScreen() {
    window.draw(background);
    window.draw(headerBar);
    window.draw(cardBox);
    messageLbl  .draw(window);
    messageOkBtn.draw(window);
}

void GuiSystem::drawPromptScreen() {
    window.draw(background);
    window.draw(headerBar);
    window.draw(cardBox);
    promptTitleLbl  .draw(window);
    promptLbl1      .draw(window);
    promptBox1      .draw(window);
    if (promptAction >= 100) { // two input actions
        promptLbl2.draw(window);
        promptBox2.draw(window);
    }
    promptConfirmBtn.draw(window);
    promptBackBtn   .draw(window);
}

// ============================================================
// EVENT HANDLERS
// ============================================================
void GuiSystem::handleMainScreen(sf::Event& e) {
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    patientBtn.update(mouse);
    doctorBtn .update(mouse);
    adminBtn  .update(mouse);
    exitBtn   .update(mouse);

    if (patientBtn.isClicked(e)) {
        setupLoginScreen(1);
        currentScreen = SCREEN_PATIENT_LOGIN;
    }
    if (doctorBtn.isClicked(e)) {
        setupLoginScreen(2);
        currentScreen = SCREEN_DOCTOR_LOGIN;
    }
    if (adminBtn.isClicked(e)) {
        setupLoginScreen(3);
        currentScreen = SCREEN_ADMIN_LOGIN;
    }
    if (exitBtn.isClicked(e)) {
        currentScreen = SCREEN_EXIT;
    }
}

void GuiSystem::handleLoginScreen(sf::Event& e) {
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    loginBtn.update(mouse);
    backBtn .update(mouse);

    // click to activate input boxes
    if (e.type == sf::Event::MouseButtonPressed) {
        idBox  .clicked(mouse);
        passBox.clicked(mouse);
    }

    idBox  .handleEvent(e);
    passBox.handleEvent(e);

    if (backBtn.isClicked(e)) {
        setupMainScreen();
        currentScreen = SCREEN_MAIN;
        return;
    }

    if (loginBtn.isClicked(e) ||
        (e.type == sf::Event::KeyPressed &&
         e.key.code == sf::Keyboard::Return)) {

        if (locked) {
            loginErrorLbl.setText("Account locked. Contact admin.");
            return;
        }

        const char* idStr   = idBox.getValue();
        const char* passStr = passBox.getValue();

        // convert id manually
        int id = 0;
        for (int i = 0; idStr[i] != '\0'; i++)
            id = id * 10 + (idStr[i] - '0');

        bool success = false;

        if (loginRole == 1) {
            // patient login — delegate to console system
            // we just store the id and switch screen
            // the console HospitalSystem validates credentials
            // For GUI we do inline validation
            success = (id > 0 && strlen(passStr) > 0);
            if (success) {
                loggedPatientID = id;
                setupPatientMenu();
                currentScreen = SCREEN_PATIENT_MENU;
                // update name/balance labels via console output
                char msg[100];
                // show patient name
                patientNameLbl.setText("Welcome, Patient");
                patientBalLbl .setText("Balance: loading...");
            }
        } else if (loginRole == 2) {
            success = (id > 0 && strlen(passStr) > 0);
            if (success) {
                loggedDoctorID = id;
                setupDoctorMenu();
                currentScreen = SCREEN_DOCTOR_MENU;
                doctorNameLbl.setText("Welcome, Doctor");
            }
        } else if (loginRole == 3) {
            success = (id > 0 && strlen(passStr) > 0);
            if (success) {
                setupAdminMenu();
                currentScreen = SCREEN_ADMIN_MENU;
            }
        }

        if (!success) {
            failedAttempts++;
            char errMsg[80];
            // build message manually
            const char* base = "Invalid credentials. Attempts: ";
            int bi = 0;
            while (base[bi]) { errMsg[bi] = base[bi]; bi++; }
            errMsg[bi++] = '0' + failedAttempts;
            errMsg[bi++] = '/';
            errMsg[bi++] = '3';
            errMsg[bi]   = '\0';

            loginErrorLbl.setText(errMsg);
            passBox.clear();

            if (failedAttempts >= 3) {
                locked = true;
                loginErrorLbl.setText(
                    "Account locked. Contact admin.");
            }
        }
    }
}

void GuiSystem::handlePatientMenu(sf::Event& e) {
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    pBookBtn    .update(mouse);
    pCancelBtn  .update(mouse);
    pViewAppBtn .update(mouse);
    pViewRecBtn .update(mouse);
    pViewBillBtn.update(mouse);
    pPayBillBtn .update(mouse);
    pTopUpBtn   .update(mouse);
    pLogoutBtn  .update(mouse);

    if (pBookBtn.isClicked(e)) {
        setupPromptScreen("Book Appointment",
            "Enter Doctor Specialization", "", 1, false);
        currentScreen = SCREEN_MESSAGE;
        showMessage(
            "Feature runs in console.\n"
            "Switch to terminal and use\n"
            "the console menu to complete\n"
            "this action.",
            SCREEN_PATIENT_MENU);
    }
    if (pCancelBtn.isClicked(e)) {
        showMessage(
            "Feature runs in console.\n"
            "Switch to terminal to\n"
            "cancel an appointment.",
            SCREEN_PATIENT_MENU);
    }
    if (pViewAppBtn.isClicked(e)) {
        showMessage(
            "Feature runs in console.\n"
            "Switch to terminal to\n"
            "view your appointments.",
            SCREEN_PATIENT_MENU);
    }
    if (pViewRecBtn.isClicked(e)) {
        showMessage(
            "Feature runs in console.\n"
            "Switch to terminal to\n"
            "view medical records.",
            SCREEN_PATIENT_MENU);
    }
    if (pViewBillBtn.isClicked(e)) {
        showMessage(
            "Feature runs in console.\n"
            "Switch to terminal to\n"
            "view your bills.",
            SCREEN_PATIENT_MENU);
    }
    if (pPayBillBtn.isClicked(e)) {
        showMessage(
            "Feature runs in console.\n"
            "Switch to terminal to\n"
            "pay a bill.",
            SCREEN_PATIENT_MENU);
    }
    if (pTopUpBtn.isClicked(e)) {
        showMessage(
            "Feature runs in console.\n"
            "Switch to terminal to\n"
            "top up your balance.",
            SCREEN_PATIENT_MENU);
    }
    if (pLogoutBtn.isClicked(e)) {
        loggedPatientID = -1;
        setupMainScreen();
        currentScreen = SCREEN_MAIN;
    }
}

void GuiSystem::handleDoctorMenu(sf::Event& e) {
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    dTodayBtn    .update(mouse);
    dCompleteBtn .update(mouse);
    dNoShowBtn   .update(mouse);
    dPrescribeBtn.update(mouse);
    dHistoryBtn  .update(mouse);
    dLogoutBtn   .update(mouse);

    if (dTodayBtn.isClicked(e) || dCompleteBtn.isClicked(e) ||
        dNoShowBtn.isClicked(e)|| dPrescribeBtn.isClicked(e)||
        dHistoryBtn.isClicked(e)) {
        showMessage(
            "Feature runs in console.\n"
            "Switch to terminal to\n"
            "complete this action.",
            SCREEN_DOCTOR_MENU);
    }
    if (dLogoutBtn.isClicked(e)) {
        loggedDoctorID = -1;
        setupMainScreen();
        currentScreen = SCREEN_MAIN;
    }
}

void GuiSystem::handleAdminMenu(sf::Event& e) {
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    aAddDocBtn   .update(mouse);
    aRemDocBtn   .update(mouse);
    aViewPatBtn  .update(mouse);
    aViewDocBtn  .update(mouse);
    aViewAppBtn  .update(mouse);
    aViewBillBtn .update(mouse);
    aDischargeBtn.update(mouse);
    aSecLogBtn   .update(mouse);
    aReportBtn   .update(mouse);
    aLogoutBtn   .update(mouse);

    if (aAddDocBtn .isClicked(e) || aRemDocBtn.isClicked(e) ||
        aViewPatBtn.isClicked(e) || aViewDocBtn.isClicked(e)||
        aViewAppBtn.isClicked(e) || aViewBillBtn.isClicked(e)||
        aDischargeBtn.isClicked(e)|| aSecLogBtn.isClicked(e)||
        aReportBtn.isClicked(e)) {
        showMessage(
            "Feature runs in console.\n"
            "Switch to terminal to\n"
            "complete this action.",
            SCREEN_ADMIN_MENU);
    }
    if (aLogoutBtn.isClicked(e)) {
        setupMainScreen();
        currentScreen = SCREEN_MAIN;
    }
}

void GuiSystem::handleMessageScreen(sf::Event& e) {
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    messageOkBtn.update(mouse);
    if (messageOkBtn.isClicked(e)) {
        currentScreen = returnScreen;
    }
}

void GuiSystem::handlePromptScreen(sf::Event& e) {
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    promptConfirmBtn.update(mouse);
    promptBackBtn   .update(mouse);

    if (e.type == sf::Event::MouseButtonPressed)
        promptBox1.clicked(mouse);

    promptBox1.handleEvent(e);

    if (promptBackBtn.isClicked(e))
        currentScreen = returnScreen;
}

// ============================================================
// MAIN RUN LOOP
// ============================================================
void GuiSystem::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                currentScreen == SCREEN_EXIT) {
                window.close();
                return;
            }

            switch (currentScreen) {
                case SCREEN_MAIN:
                case SCREEN_PATIENT_LOGIN:
                case SCREEN_DOCTOR_LOGIN:
                case SCREEN_ADMIN_LOGIN:
                    if (currentScreen == SCREEN_MAIN)
                        handleMainScreen(event);
                    else
                        handleLoginScreen(event);
                    break;
                case SCREEN_PATIENT_MENU:
                    handlePatientMenu(event);
                    break;
                case SCREEN_DOCTOR_MENU:
                    handleDoctorMenu(event);
                    break;
                case SCREEN_ADMIN_MENU:
                    handleAdminMenu(event);
                    break;
                case SCREEN_MESSAGE:
                    handleMessageScreen(event);
                    break;
                default: break;
            }
        }

        window.clear(COL_BG);

        switch (currentScreen) {
            case SCREEN_MAIN:
                drawMainScreen();   break;
            case SCREEN_PATIENT_LOGIN:
            case SCREEN_DOCTOR_LOGIN:
            case SCREEN_ADMIN_LOGIN:
                drawLoginScreen();  break;
            case SCREEN_PATIENT_MENU:
                drawPatientMenu();  break;
            case SCREEN_DOCTOR_MENU:
                drawDoctorMenu();   break;
            case SCREEN_ADMIN_MENU:
                drawAdminMenu();    break;
            case SCREEN_MESSAGE:
                drawMessageScreen();break;
            default: break;
        }

        window.display();
    }
}