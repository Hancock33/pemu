//
// Created by cpasjuste on 28/09/18.
//

#ifndef PEMU_PNESUISTATEMENU_H
#define PEMU_PNESUISTATEMENU_H

#include "c2dui_ui_main.h"
#include "c2dui_ui_menu_state.h"

class PNESUIStateMenu : public c2dui::UIStateMenu {

public:
    PNESUIStateMenu(c2dui::UIMain *ui);

    bool loadStateCore(const char *path);

    bool saveStateCore(const char *path);

};

#endif //PEMU_PNESUISTATEMENU_H
