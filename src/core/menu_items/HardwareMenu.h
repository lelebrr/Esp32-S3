#ifndef __HARDWARE_MENU_H__
#define __HARDWARE_MENU_H__

#include <MenuItemInterface.h>

class HardwareMenu : public MenuItemInterface {
public:
    HardwareMenu() : MenuItemInterface("Hardware") {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return true; }

private:
    void modulesMenu(void);
    void statusMenu(void);
    void conflictsMenu(void);
    void gpiosMenu(void);
    void detectMenu(void);
    void resetHwMenu(void);
};

#endif
