#ifndef MAIN_VIEW_HPP
#define MAIN_VIEW_HPP

#include <gui_generated/main_screen/MainViewBase.hpp>
#include <gui/main_screen/MainPresenter.hpp>

class MainView : public MainViewBase
{
public:
    MainView();
    virtual ~MainView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    void updateWildcardTexts(uint16_t tick);
    virtual void functionPlayPauseButton();
    virtual void functionPreviousButton();
    virtual void functionNextButton();

protected:
    const char* mot = "salut";
    uint8_t motIndex = 0;
    void mettreAJourTexte();
    uint16_t tickCounter;
    Unicode::UnicodeChar texte1Buffer[16];
    Unicode::UnicodeChar texte2Buffer[16];
};

#endif // MAIN_VIEW_HPP
