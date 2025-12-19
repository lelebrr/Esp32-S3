#ifndef AI_MENU_H
#define AI_MENU_H

#include "MenuItemInterface.h"
#include "ai_attack_learner.h"

class AIMenu : public MenuItemInterface {
public:
    AIMenu();
    void optionsMenu();
    void draw();
    String getName();

private:
    void updateAttacks();
    void growBruteForceList();
    void qLearnFeedback(bool success);
    void generateNewAttack();
    void showStats();
    void toggleStealth();
};

extern AIMenu aiMenu;

#endif
