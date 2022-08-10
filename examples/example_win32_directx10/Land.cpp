#include "Land.h"

Land::Land() {
    ind = 0;
    pos = ImVec2(0.0f, 0.0f);
    hotels = 0;
}

Land::Land(int x, ImVec2 p) {
    ind = x;
    pos = p;
    hotels = 0;
}

int Land::getInd() {
    return ind;
}
ImVec2 Land::getPos() {
    return pos;
}
int Land::getHotels(){
    return hotels;
}
Player* Land::getPlayer() {
    return p;
}
void Land::setName(char* n) {
    name = n;
}
void Land::setPlayer(Player* player) {
    p = player;
}

char* Land::getName() {
    return name;
}

ImVec4 Land::getColor() {
    return color;
}
void Land::setColor(ImVec4 c) {
    color = c;
}
bool Land::isSpecial() {
    return special;
}
void Land::Special() {
    special = true;
}
