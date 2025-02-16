//
// Created by cpasjuste on 22/11/16.
//
#include <algorithm>
#include "c2dui.h"
#include "ss_api.h"

using namespace c2d;
using namespace c2dui;
using namespace ss_api;

UIRomList::UIRomList(UiMain *main, RomList *romList, const c2d::Vector2f &size)
        : SkinnedRectangle(main, {"SKIN_CONFIG", "MAIN"}) {
    printf("UIRomList\n");
    pMain = main;
    pRomList = romList;
    Skin *skin = pMain->getSkin();

    // add title image if available
    auto *title = new SkinnedRectangle(pMain, {"SKIN_CONFIG", "MAIN", "TITLE"});
    UIRomList::add(title);

    // add help
    auto *help = new UiHelp(pMain);
    UIRomList::add(help);

    // add rom info ui
    pRomInfo = new UIRomInfo(pMain, this, skin->getFont(), pMain->getFontSize());
    UIRomList::add(pRomInfo);

    // add rom list title (system text)
    if (!(pMain->getConfig()->get(Option::Id::GUI_FILTER_SYSTEM)->getFlags() & Option::Flags::HIDDEN)) {
        pTitleText = new SkinnedText(pMain, {"SKIN_CONFIG", "MAIN", "ROM_LIST", "SYSTEM_TEXT"});
        if (pTitleText->available) {
            UIRomList::add(pTitleText);
        } else {
            delete (pTitleText);
        }
    }

    // add rom listing ui
    Skin::TextGroup textGroup = skin->getText({"SKIN_CONFIG", "MAIN", "ROM_LIST", "TEXT"});
    config::Group *grp = skin->getConfig()->getGroup("ROM_LIST")->getGroup("TEXT");
    config::Option *opt = grp->getOption("color_missing");
    Color colorMissing;
    if (opt->getType() == config::Option::Type::String) {
        opt = skin->getConfig()->getOption("COLORS", opt->getString());
        if (opt) {
            colorMissing = opt->getColor();
        }
    } else {
        colorMissing = grp->getOption("color_missing")->getColor();
    }

    bool highlightUseFileColors = grp->getOption("highlight_use_text_color")->getInteger() == 1;

    // add rom list ui
    Skin::RectangleShapeGroup romListGroup = skin->getRectangleShape({"MAIN", "ROM_LIST"});
    bool use_icons = false;
#if !(defined(__PSP2__) || defined(__3DS__)) // two slow
    use_icons = pMain->getConfig()->get(Option::Id::GUI_SHOW_ICONS)->getValueBool();
#endif
    pListBox = new UIListBox(pMain, skin->getFont(), (int) textGroup.size,
                             romListGroup.rect, mGameList.games, use_icons);
    pListBox->colorMissing = colorMissing;
    pListBox->colorAvailable = textGroup.color;
    pListBox->setFillColor(romListGroup.color);
    pListBox->setOutlineColor(romListGroup.outlineColor);
    pListBox->setOutlineThickness((float) romListGroup.outlineSize);
    pListBox->setSelection(0);
    // rom item
    pListBox->setTextOutlineColor(textGroup.outlineColor);
    pListBox->setTextOutlineThickness((float) textGroup.outlineSize);
    // hihglight
    Skin::RectangleShapeGroup rectShape = skin->getRectangleShape({"SKIN_CONFIG", "HIGHLIGHT"});
    pListBox->getHighlight()->setFillColor(rectShape.color);
    pListBox->getHighlight()->setOutlineColor(rectShape.outlineColor);
    pListBox->getHighlight()->setOutlineThickness((float) rectShape.outlineSize);
    pListBox->setHighlightUseFileColor(highlightUseFileColors);
    UIRomList::add(pListBox);

    // add blur
    pBlur = new RectangleShape(UIRomList::getLocalBounds());
    pBlur->setFillColor(Color::Gray);
    pBlur->add(new TweenAlpha(0, 230, 0.2));
    pBlur->setVisibility(Visibility::Hidden);
    UIRomList::add(pBlur);

    int delay = pMain->getConfig()->get(Option::Id::GUI_VIDEO_SNAP_DELAY)->getValueInt();
    UIRomList::setVideoSnapDelay(delay);

    // filter roms
    UIRomList::updateRomList();
}

void UIRomList::updateRomList() {
    filterRomList();
    sortRomList();

    if (pTitleText && pTitleText->available) {
        std::string sys = pMain->getConfig()->get(Option::Id::GUI_FILTER_SYSTEM)->getValueString();
        pTitleText->setString(sys);
    }

    if (pListBox) {
        pListBox->setGames(mGameList.games);
    }

    if (pRomInfo) {
        pRomInfo->load(Game());
        mTimerLoadInfoDone = 0;
        mTimerLoadInfo.restart();
        mTimerLoadVideoDone = 0;
        mTimerLoadVideo.restart();
    }
}

std::string UIRomList::getPreview(const Game &game, UIRomList::PreviewType type) {
    std::string mediaType = type == Tex ? "mixrbv2" : "video";
    std::string mediaExt = type == Tex ? ".png" : ".mp4";

    // try media path from database
    std::string path = game.romsPath + game.getMedia(mediaType).url;
    printf("getPreview(%s)\n", path.c_str());
    if (pMain->getIo()->getFile(path).isFile()) {
        return path;
    }

    // try from filename
    path = game.romsPath + "media/" + mediaType + "/" + Utility::removeExt(game.path) + mediaExt;
    printf("getPreview(%s)\n", path.c_str());
    if (pMain->getIo()->getFile(path).isFile()) {
        return path;
    }

    // for non arcade game, search for a "screenscraper" game with same name
    if (!game.isClone()) {
        std::vector<Game> clones = pRomList->gameList->findGamesByName(game);
        for (const auto &g: clones) {
            path = g.romsPath + g.getMedia(mediaType).url;
            printf("getPreview(%s)\n", path.c_str());
            if (pMain->getIo()->getFile(path).isFile()) {
                return path;
            }
        }
    }

    // now try parent image if rom is a clone
    if (game.isClone()) {
        Game parent = pRomList->gameList->findGameByPath(game.cloneOf);
        path = parent.romsPath + parent.getMedia(mediaType).url;
        printf("getPreview(%s)\n", path.c_str());
        if (pMain->getIo()->getFile(path).isFile()) {
            return path;
        }
    }

    return {};
}

void UIRomList::filterRomList() {
    Option *opt = pMain->getConfig()->get(Option::Id::GUI_SHOW_ALL);
    ss_api::GameList *list = opt->getValueString() == "FAVORITES" ? pRomList->gameListFav : pRomList->gameList;

    bool available = opt->getValueString() == "FAVORITES" ? false : opt->getValueString() == "AVAILABLE";
    bool showClones = pMain->getConfig()->get(Option::Id::GUI_FILTER_CLONES)->getValueBool();
    std::string system = pMain->getConfig()->get(Option::Id::GUI_FILTER_SYSTEM)->getValueString();
    int systemId;
    // custom arcade system (mame sscrap id 75)
    if (system == "ARCADE") {
        systemId = 9999;
    } else {
        systemId = system == "ALL" ? -1 : list->systemList.findByName(system).id;
    }
    std::string editor = pMain->getConfig()->get(Option::Id::GUI_FILTER_EDITOR)->getValueString();
    int editorId = editor == "ALL" ? -1 : list->findEditorByName(editor).id;
    std::string dev = pMain->getConfig()->get(Option::Id::GUI_FILTER_DEVELOPER)->getValueString();
    int devId = dev == "ALL" ? -1 : list->findDeveloperByName(dev).id;
    int players = Utility::parseInt(pMain->getConfig()->get(Option::Id::GUI_FILTER_PLAYERS)->getValueString(), -1);
    int rating = Utility::parseInt(pMain->getConfig()->get(Option::Id::GUI_FILTER_RATING)->getValueString(), -1);
    int rotation = Utility::parseInt(pMain->getConfig()->get(Option::Id::GUI_FILTER_ROTATION)->getValueString(), -1);
    std::string genre = pMain->getConfig()->get(Option::Id::GUI_FILTER_GENRE)->getValueString();
    int genreId = genre == "ALL" ? -1 : list->findGenreByName(genre).id;

    mGameList = list->filter(
            available, showClones, systemId == 9999 ? -1 : systemId, systemId == 9999 ? 75 : -1,
            editorId, devId, players, rating, rotation, genreId,
            pMain->getConfig()->get(Option::Id::GUI_FILTER_RESOLUTION)->getValueString(),
            pMain->getConfig()->get(Option::Id::GUI_FILTER_DATE)->getValueString()
    );
}

void UIRomList::sortRomList() {
    bool byZipName = pMain->getConfig()->get(Option::Id::GUI_SHOW_ZIP_NAMES)->getValueBool();
    mGameList.sortAlpha(byZipName);
}

void UIRomList::setVideoSnapDelay(int delay) {
    mTimerLoadVideoDelay = delay * 1000;
}

Game UIRomList::getSelection() {
    return pListBox->getSelection();
}

RomList *UIRomList::getRomList() {
    return pRomList;
}

void UIRomList::setVisibility(c2d::Visibility visibility, bool tweenPlay) {
    if (visibility == c2d::Visibility::Hidden) {
        pRomInfo->load();
        mTimerLoadInfoDone = 0;
        mTimerLoadVideoDone = 0;
    } else {
        mTimerLoadInfo.restart();
        mTimerLoadVideo.restart();
    }

    RectangleShape::setVisibility(visibility, tweenPlay);
}

bool UIRomList::onInput(c2d::Input::Player *players) {
    if (pMain->getUiMenu()->isVisible()
        || pMain->getUiStateMenu()->isVisible()
        || pMain->getUiProgressBox()->isVisible()) {
        return false;
    }

    unsigned int buttons = players[0].buttons;
    if (buttons & Input::Button::Up) {
        pListBox->up();
        pRomInfo->load();
        mTimerLoadInfoDone = 0;
        mTimerLoadVideoDone = 0;
    } else if (buttons & Input::Button::Down) {
        pListBox->down();
        pRomInfo->load();
        mTimerLoadInfoDone = 0;
        mTimerLoadVideoDone = 0;
    } else if (buttons & Input::Button::Right) {
        pListBox->setSelection(pListBox->getIndex() + pListBox->getMaxLines());
        pRomInfo->load();
        mTimerLoadInfoDone = 0;
        mTimerLoadVideoDone = 0;
    } else if (buttons & Input::Button::Left) {
        pListBox->setSelection(pListBox->getIndex() - pListBox->getMaxLines());
        pRomInfo->load();
        mTimerLoadInfoDone = 0;
        mTimerLoadVideoDone = 0;
    } else if (buttons & Input::Button::A) {
        Game game = getSelection();
        if (game.available) {
#ifdef __MPV__
            pRomInfo->mpvTexture->setVisibility(c2d::Visibility::Hidden);
            pRomInfo->mpv->stop();
#endif
            pMain->getConfig()->load(game);
            pMain->getUiEmu()->load(game);
            return true;
        }
    } else if (buttons & Input::Button::X) {
        // add to favorites
        Game game = getSelection();
        if (game.id > 0 && !pRomList->gameListFav->exist(game.id)) {
            int res = pMain->getUiMessageBox()->show(
                    "FAVORITES", "Add to favorites ?", "OK", "CANCEL");
            if (res == MessageBox::LEFT) {
                pRomList->addFav(game);
            }
        } else if (game.id > 0 && pRomList->gameListFav->exist(game.id)) {
            int res = pMain->getUiMessageBox()->show(
                    "FAVORITES", "Remove from favorites ?", "OK", "CANCEL");
            if (res == MessageBox::LEFT) {
                pRomList->removeFav(game);
                Option *opt = pMain->getConfig()->get(Option::Id::GUI_SHOW_ALL);
                if (opt->getValueString() == "FAVORITES") {
                    // update list if we are in favorites
                    updateRomList();
                }
            }
        }
    } else if (buttons & Input::Button::Menu1) {
        pMain->getUiMenu()->load();
    } else if (buttons & Input::Button::Menu2) {
        if (getSelection().id > 0) {
            pMain->getUiMenu()->load(true);
        }
    }

    // only allow system switch if skin contains romlist title
    if (pTitleText && pTitleText->available) {
        if (buttons & Input::Button::LT) {
            Option *sysOpt = pMain->getConfig()->get(Option::Id::GUI_FILTER_SYSTEM);
            size_t sysCount = sysOpt->getValues()->size();
            if (sysCount > 1) {
                sysOpt->prev();
                updateRomList();
            }
        } else if (buttons & Input::Button::RT) {
            Option *sysOpt = pMain->getConfig()->get(Option::Id::GUI_FILTER_SYSTEM);
            size_t sysCount = sysOpt->getValues()->size();
            if (sysCount > 1) {
                sysOpt->next();
                updateRomList();
            }
        }
    }

    return true;
}

void UIRomList::onUpdate() {
    if (!isVisible() || pMain->getUiProgressBox()->isVisible()) {
        return;
    }

    unsigned int buttons = pMain->getInput()->getButtons();

    if (buttons > 0 && buttons != Input::Delay) {
        mTimerLoadInfo.restart();
        mTimerLoadVideo.restart();
    } else {
        if ((mTimerLoadInfoDone == 0) && mTimerLoadInfo.getElapsedTime().asMilliseconds() > mTimerLoadInfoDelay) {
            pRomInfo->load(pListBox->getSelection());
            mTimerLoadInfoDone = 1;
        }
        if (mTimerLoadVideoDelay > 0 && mTimerLoadVideoDone == 0 &&
            mTimerLoadVideo.getElapsedTime().asMilliseconds() > mTimerLoadVideoDelay) {
            pRomInfo->loadVideo(pListBox->getSelection());
            mTimerLoadVideoDone = 1;
        }
    }

    RectangleShape::onUpdate();
}

UIRomList::~UIRomList() {
    printf("~UIRomList\n");
    delete (pRomList);
}
