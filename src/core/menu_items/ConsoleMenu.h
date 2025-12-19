#ifndef __CONSOLE_MENU_H__
#define __CONSOLE_MENU_H__

#include <MenuItemInterface.h>

extern "C" void consoleMenuFunction();
void updateConsoleInterface();

class ConsoleMenu : public MenuItemInterface {
public:
    ConsoleMenu();
    void optionsMenu() override;
    void drawIcon(float scale) override;
    void drawIconImg() override;
    bool getTheme() override;
};

extern ConsoleMenu consoleMenu;

#endif
