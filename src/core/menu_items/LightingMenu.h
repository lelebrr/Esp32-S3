#ifndef __LIGHTING_MENU_H__
#define __LIGHTING_MENU_H__

#include <MenuItemInterface.h>

class LightingMenu : public MenuItemInterface {
public:
    LightingMenu() : MenuItemInterface("Iluminacao") {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return leleConfig.theme.others; }

private:
    void colorMenu();
    void selectLedMenu();
    void effectsMenu();
    void brightnessMenu();
    void notificationsMenu();
};

#endif // __LIGHTING_MENU_H__
