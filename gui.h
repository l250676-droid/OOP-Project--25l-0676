#ifndef GUI_H
#define GUI_H

#include <SFML/Graphics.hpp>
#include <cstring>
#include <iostream>
using namespace std;

// ============================================================
// BUTTON CLASS
// a clickable rectangle with text inside
// ============================================================
class Button {
    sf::RectangleShape box;
    sf::Text           label;
    sf::Color          normalColor;
    sf::Color          hoverColor;
    bool               hovered;
public:
    Button();
    void setup(float x, float y, float w, float h,
               const char* text, sf::Font& font,
               sf::Color normal, sf::Color hover);

    void draw(sf::RenderWindow& window);
    void update(sf::Vector2i mousePos);
    bool isClicked(sf::Event& event);
    void setText(const char* text);
};

// ============================================================
// INPUTBOX CLASS
// a box the user can type into
// ============================================================
class InputBox {
    sf::RectangleShape box;
    sf::Text           text;
    sf::Text           placeholder;
    char               buffer[100];
    int                length;
    bool               active;
    bool               isPassword; // shows * instead of chars
public:
    InputBox();
    void setup(float x, float y, float w, float h,
               const char* hint, sf::Font& font,
               bool password = false);

    void draw(sf::RenderWindow& window);
    void handleEvent(sf::Event& event);
    void setActive(bool a);
    bool getActive() const;
    const char* getValue() const;
    void clear();
    void clicked(sf::Vector2i mousePos);
};

// ============================================================
// LABEL CLASS
// simple text display
// ============================================================
class Label {
    sf::Text text;
public:
    Label();
    void setup(float x, float y, const char* str,
               sf::Font& font, int size, sf::Color color);
    void setText(const char* str);
    void draw(sf::RenderWindow& window);
};

// ============================================================
// SCREEN ENUM
// tracks which screen is currently showing
// ============================================================
enum Screen {
    SCREEN_MAIN,
    SCREEN_PATIENT_LOGIN,
    SCREEN_DOCTOR_LOGIN,
    SCREEN_ADMIN_LOGIN,
    SCREEN_PATIENT_MENU,
    SCREEN_DOCTOR_MENU,
    SCREEN_ADMIN_MENU,
    SCREEN_MESSAGE,       // shows a message then goes back
    SCREEN_EXIT
};

// ============================================================
// GUI SYSTEM
// manages the window and all screens
// ============================================================
class GuiSystem {
    sf::RenderWindow window;
    sf::Font         font;
    Screen           currentScreen;
    Screen           returnScreen; // where to go after message

    // message screen
    char    messageText[500];
    Button  messageOkBtn;
    Label   messageLbl;

    // --- main screen ---
    Label  titleLbl;
    Label  subtitleLbl;
    Button patientBtn;
    Button doctorBtn;
    Button adminBtn;
    Button exitBtn;

    // --- login screens ---
    Label    loginTitleLbl;
    Label    loginErrorLbl;
    Label    idLbl;
    Label    passLbl;
    InputBox idBox;
    InputBox passBox;
    Button   loginBtn;
    Button   backBtn;
    int      loginRole; // 1=patient 2=doctor 3=admin
    int      failedAttempts;
    bool     locked;

    // --- patient menu ---
    Label  patientNameLbl;
    Label  patientBalLbl;
    Button pBookBtn;
    Button pCancelBtn;
    Button pViewAppBtn;
    Button pViewRecBtn;
    Button pViewBillBtn;
    Button pPayBillBtn;
    Button pTopUpBtn;
    Button pLogoutBtn;

    // --- doctor menu ---
    Label  doctorNameLbl;
    Button dTodayBtn;
    Button dCompleteBtn;
    Button dNoShowBtn;
    Button dPrescribeBtn;
    Button dHistoryBtn;
    Button dLogoutBtn;

    // --- admin menu ---
    Label  adminTitleLbl;
    Button aAddDocBtn;
    Button aRemDocBtn;
    Button aViewPatBtn;
    Button aViewDocBtn;
    Button aViewAppBtn;
    Button aViewBillBtn;
    Button aDischargeBtn;
    Button aSecLogBtn;
    Button aReportBtn;
    Button aLogoutBtn;

    // --- input prompt screen ---
    // used for any action needing text input
    Label    promptTitleLbl;
    Label    promptLbl1;
    Label    promptLbl2;
    InputBox promptBox1;
    InputBox promptBox2;
    Button   promptConfirmBtn;
    Button   promptBackBtn;
    int      promptAction;  // which action triggered the prompt
    char     promptData1[100]; // stores first input result
    char     promptData2[100]; // stores second input result

    // logged in user IDs
    int loggedPatientID;
    int loggedDoctorID;

    // helper to show a message and return to a screen
    void showMessage(const char* msg, Screen returnTo);

    // screen setup functions
    void setupMainScreen();
    void setupLoginScreen(int role);
    void setupPatientMenu();
    void setupDoctorMenu();
    void setupAdminMenu();
    void setupPromptScreen(const char* title,
                           const char* hint1,
                           const char* hint2,
                           int action,
                           bool twoInputs = false);

    // draw functions
    void drawMainScreen();
    void drawLoginScreen();
    void drawPatientMenu();
    void drawDoctorMenu();
    void drawAdminMenu();
    void drawMessageScreen();
    void drawPromptScreen();

    // event handlers
    void handleMainScreen(sf::Event& e);
    void handleLoginScreen(sf::Event& e);
    void handlePatientMenu(sf::Event& e);
    void handleDoctorMenu(sf::Event& e);
    void handleAdminMenu(sf::Event& e);
    void handleMessageScreen(sf::Event& e);
    void handlePromptScreen(sf::Event& e);

    // background
    sf::RectangleShape background;
    sf::RectangleShape headerBar;
    sf::RectangleShape cardBox;

public:
    GuiSystem();
    void run();
};

#endif