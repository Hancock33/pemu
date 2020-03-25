//
// Created by cpasjuste on 01/06/18.
//

#ifndef PFBA_UIEMU_H
#define PFBA_UIEMU_H

#include <string>

class PFBAGuiEmu : public c2dui::UIEmu {

public:

    explicit PFBAGuiEmu(c2dui::UIMain *ui);

    int load(const ss_api::Game &game) override;

    void stop() override;

private:

    int getSekCpuCore();

    bool onInput(c2d::Input::Player *players) override;

    void onUpdate() override;

    void updateFb();

    void updateFrame();

    void renderFrame(bool draw = true);

    bool audio_sync = false;

    c2d::FloatRect textureRect;
};

#endif //PFBA_UIEMU_H
