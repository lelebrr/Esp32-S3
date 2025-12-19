#ifndef __WIFI_MENU_H__
#define __WIFI_MENU_H__

#include <MenuItemInterface.h>

class WifiMenu : public MenuItemInterface {
public:
    WifiMenu() : MenuItemInterface("WiFi") {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return leleConfig.theme.wifi; }

private:
    void configMenu(void);
    void advancedAttacksMenu(void);
    void cveAttacksMenu(void);
    void captureAttacksMenu(void);
};

#endif
