#include "HospitalSystem.h"
#include "GUI.h"
#include <SFML/Graphics.hpp>
#include <thread>

// the console system runs in a separate thread
// so the GUI window stays open while you use the console
void runConsole() {
    HospitalSystem hospital;
    hospital.run();
}

int main() {
    // start console system in background thread
    std::thread consoleThread(runConsole);

    // run GUI window on main thread
    GuiSystem gui;
    gui.run();

    // wait for console thread to finish
    consoleThread.join();

    return 0;
}