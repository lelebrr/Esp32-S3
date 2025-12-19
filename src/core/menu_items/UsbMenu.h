/**
 * @file UsbMenu.h
 * @brief Menu de Ataques USB
 * @author Lele Origin Team
 * @version 1.0
 */

#ifndef __USB_MENU_H__
#define __USB_MENU_H__

#include <MenuItemInterface.h>

class UsbMenu : public MenuItemInterface {
public:
    UsbMenu() : MenuItemInterface("USB") {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return leleConfig.theme.usb; }
};

#endif
