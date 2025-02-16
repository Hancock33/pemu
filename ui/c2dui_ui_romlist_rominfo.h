//
// Created by cpasjuste on 12/05/2020.
//

#ifndef PEMU_C2DUI_UI_ROMLIST_ROMINFO_H
#define PEMU_C2DUI_UI_ROMLIST_ROMINFO_H

#include "c2dui.h"

class UIRomInfo : public c2d::Rectangle {

public:

    UIRomInfo(c2dui::UiMain *u, c2dui::UIRomList *uiRList, c2d::Font *fnt, int fntSize);

    ~UIRomInfo() override;

    c2d::Text *addInfoBoxText(const std::vector<std::string> &tree);

    void showText(c2d::Text *text, const std::string &msg);

    void hideText(c2d::Text *text);

    bool loadVideo(const ss_api::Game &game);

    bool loadTexture(const ss_api::Game &game);

    void load(const ss_api::Game &game = ss_api::Game());

    c2dui::UiMain *main = nullptr;
    c2dui::UIRomList *uiRomList = nullptr;
    c2d::C2DTexture *texture = nullptr;
    c2d::Font *font;
    int fontSize;
    //
    c2d::RectangleShape *synoBox = nullptr;
    c2d::Text *synoText = nullptr;
    //
    c2d::RectangleShape *previewBox = nullptr;
    //
    c2d::RectangleShape *infoBox = nullptr;
    c2d::Text *systemText = nullptr;
    c2d::Text *developerText = nullptr;
    c2d::Text *editorText = nullptr;
    c2d::Text *dateText = nullptr;
    c2d::Text *genreText = nullptr;
    c2d::Text *playersText = nullptr;
    c2d::Text *ratingText = nullptr;
    c2d::Text *rotationText = nullptr;
    c2d::Text *resolutionText = nullptr;
    c2d::Text *cloneofText = nullptr;
    c2d::Text *filenameText = nullptr;
#ifdef __MPV__
    Mpv *mpv = nullptr;
    MpvTexture *mpvTexture = nullptr;
#endif
};

#endif //PEMU_C2DUI_UI_ROMLIST_ROMINFO_H
