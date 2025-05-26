#include <gui/main_screen/MainView.hpp>
#include <images/BitmapDatabase.hpp>

MainView::MainView()
{
}

void MainView::setupScreen()
{
    // Setup tick counter
    tickCounter = 0;
    MainViewBase::setupScreen();
    playPauseButton.setBitmaps(Bitmap(BITMAP_PLAY_ICON_ID), Bitmap(BITMAP_PLAY_ICON_ID));
    progression.setTouchable(false);
    mettreAJourTexte();
}

void MainView::tearDownScreen()
{
    MainViewBase::tearDownScreen();
}

void MainView::handleTickEvent()
{
    // Update the wildcard text, but only each 10th time
    if (++tickCounter % 10 == 0)
    {
        updateWildcardTexts(tickCounter / 10);
    }
}

void MainView::updateWildcardTexts(uint16_t tick)
{
    Unicode::snprintf(tickTxtBuffer, sizeof(tickTxtBuffer) / sizeof(tickTxtBuffer[0]), "%d", tick);
    tickTxt.invalidate();


    Unicode::strncpy(touchgfxTxtBuffer, mot,  9);
    touchgfxTxt.invalidate();
}


void MainView::functionPlayPauseButton()
{
    static bool isPlaying = false;

    if (!isPlaying)
    {
        playPauseButton.setBitmaps(Bitmap(BITMAP_PAUSE_ICON_ID), Bitmap(BITMAP_PAUSE_ICON_ID));

    }
    else
    {
        playPauseButton.setBitmaps(Bitmap(BITMAP_PLAY_ICON_ID), Bitmap(BITMAP_PLAY_ICON_ID));

    }

    isPlaying = !isPlaying;
    playPauseButton.invalidate(); // Redessine le bouton
}

void MainView::functionNextButton(){
    motIndex = (motIndex + 1) % 2;
    mettreAJourTexte();
}

void MainView::functionPreviousButton(){
}

void MainView::mettreAJourTexte()
{

}