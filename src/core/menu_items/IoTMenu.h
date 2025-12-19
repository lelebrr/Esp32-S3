#ifndef __IOT_MENU_H__
#define __IOT_MENU_H__

#include <MenuItemInterface.h>

extern "C" void iotMenuFunction();
void updateIoTInterface();

class IoTMenu : public MenuItemInterface {
public:
    IoTMenu();
    void optionsMenu() override;
    void drawIcon(float scale) override;
    void drawIconImg() override;
    bool getTheme() override;
};

extern IoTMenu iotMenu;

#endif
