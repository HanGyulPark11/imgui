#include "Asset.h"

Asset::Asset() {
    int num = 0;
}
Player::Player() {
    money = 2500;
    pos = 0;
}

int Player::getMoney() {
    return money;
}

void Player::addMoney(int x) {
    money = money + x;
}

class Asset {
private:
    int num;
    class Land;

public:
    Asset();
    Asset(int num, class Land);
};
