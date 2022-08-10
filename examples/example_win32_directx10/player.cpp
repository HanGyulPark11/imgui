#include "player.h"
#include <stdlib.h>
#include <time.h>


Player::Player() {
    ind = 0;
    money = 2500;
    pos = 0;
    rolled = false;
}

int Player::getMoney() {
    return money;
}

void Player::addMoney(int x) {
    money = money + x;
}

int* Player::roll() {
    srand(time(NULL));
    int ret[2];
    ret[0] = rand() % 6 + 1;
    ret[1] = rand() % 6 + 1;

    return ret;
}

bool Player::done() {
    return rolled;
}

void Player::end() {
    rolled = true;
}

void Player::endTurn() {
    rolled = false;
}

void Player::proceed(int x) {
    int newPos = (pos + x) % 32;
    if (newPos < pos) {
        money += 80;
    }
    pos = newPos;
}

int Player::getPos() {
    return pos;
}

int Player::pay(Player* p, int hotels) {
    int toll = hotelFee * hotels + landToll;
    if (toll > money) {
        getLoans();
    }
    if (toll > money) {
        bankrupt();
    }
    else {
        addMoney(-toll);
        p->addMoney(toll);
        return toll;
    }
    return -1;
}

bool Player::buy(int land, int hotels) {
    int price = 0;
    if (Assets[land] > 0){
        price = hotelPrice * hotels;
    }
    else {
        price = landPrice + hotelPrice * hotels;
    }
    if (price > money || price == 0) {
        return false;
    }
    addMoney(-price);
    if (Assets[land] == 0)
        Assets[land] = hotels + 1;
    else if (Assets[land] > 0)
        Assets[land] = Assets[land] + hotels;
    return true;
}

void Player::setInd(int x) {
    ind = x;
}

int Player::getInd() {
    return ind;
}

int* Player::getAssets() {
    return Assets;
}

bool Player::isBusy() {
    return busy;
}

void Player::lock() {
    busy = true;
}

void Player::unlock() {
    busy = false;
}

int Player::getDebt() {
    return debt;
}
int Player::getTurns() {
    return turn;
}
bool Player::getLoans() {
    if (turn == -1)
    {
        money += maxLoan;
        debt = maxLoan;
        turn = 3;
        return true;
    }
    return false;
}
void Player::decreaseTurn() {
    if (--turn == 0) {
        repay();
    }
}
void Player::repay() {
    if (money < debt) {
        bankrupt();
    }
    else {
        money -= debt;
    }
}

bool Player::isBroke() {
    return broke;
}

void Player::bankrupt() {
    money = 0;
    broke = true;
    for (int i = 0; i < 32; i++) {
        Assets[i] = 0;
    }
}

int Player::jailTurn() {
    return jTurn;
}

int Player::jail() {
    return --jTurn;
}

int Player::drawGoldKey(GoldKey* gold,int gnum) {
    srand(time(NULL));
    int d = rand() % gnum+1;
    int index = 0;
    while (true) {
        if (d == 0) {
            gold[index].draw();
            if (gold[index].isUsedWhenDrawn()) {
                goldKey[2] = index;
                return index;
            }
            else {
                if (goldKey[0] == -1) {
                    goldKey[0] = index;
                    return index;
                }
                else {
                    goldKey[1] = index;
                    return index;
                }
            }
        }
        while (gold[index].isDrawn()) {
            index++;
        }
        d--;
    }
}

void Player::backwards(int n) {
    pos -= n;
}

void Player::Jail() {
    jTurn = 3;
}

void Player::breakOut() {
    jTurn = -1;
}

int Player::usedKey() {
    return goldKey[2];
}
