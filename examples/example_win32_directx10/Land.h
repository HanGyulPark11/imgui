#pragma once
#include "imgui.h"
#include "player.h"

class Land {
private:
    int ind;
    ImVec2 pos;
    int hotels;
    Player* p = nullptr;
    char* name = nullptr;
    ImVec4 color = ImVec4(90, 90, 120, 255);
    bool special;

public:
    Land();
    Land(int ind, ImVec2 pos);
    int getInd();
    ImVec2 getPos();
    int getHotels();
    Player* getPlayer();
    void setName(char* n);
    void setPlayer(Player* player);
    char* getName();
    ImVec4 getColor();
    void setColor(ImVec4 c);
    bool isSpecial();
    void Special();
};
