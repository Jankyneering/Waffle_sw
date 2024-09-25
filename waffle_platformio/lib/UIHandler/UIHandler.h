/*
 * UIHandler.h
 */

#ifndef UIHANDLER_H
#define UIHANDLER_H

#include "esp_log.h"
#include "u8g2.h"
#include <stdio.h>
#include <string.h>
// #include <list>

typedef struct Menu {
    int id;
    const char *name;
    void (*action)(void);
} MenuItem;

typedef struct SubMenu {
    int id;
    const char *name;
    void (*action)(void);
} SubMenu;


class UIHandler {
public:
    UIHandler();
    int init(int PIN_SDA, int PIN_SCL, esp_log_level_t LOG_LEVEL);
    int sleep();
    int wake();
    int redraw();

    int splashScreen();

    int addMenu(int id, const char *name);
    int removeMenu(int id);
    int selectMenu(int id);
    int addMenuAction(int id, void (*action)(void));

    int addSubMenu(int menuId, int id, const char *name);
    int removeSubMenu(int menuId, int id);
    int selectSubMenu(int menuId, int id);
    int addSubMenuAction(int menuId, int id, void (*action)(void));

private:
    static const char *TAG_UI;
    // std::list<MenuItem> menus;
    // std::list<SubMenu> subMenus;

    u8g2_t u8g2; // a structure which will contain all the data for one display
};

#endif