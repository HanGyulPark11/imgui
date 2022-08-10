#pragma once
#include "goldKey.h"

static int landPrice = 20;
static int hotelPrice = 50;
static int landToll = 30;
static int hotelFee = 100;
static int maxLoan = 1000;

class Player {
    private:
        int ind;
        int money;
        int pos;
        int Assets[32] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
        int goldKey[3] = { -1, -1, -1 };
        bool rolled;
        bool busy;
        int debt;
        int turn = -1;
        bool broke;
        int jTurn = -1;

    public:
        Player();
        int getMoney();
        void addMoney(int x);
        int* roll();
        bool done();
        void end();
        void proceed(int x);
        int getPos();
        void endTurn();
        int pay(Player* p, int hotels);
        bool buy(int land, int hotels);
        void setInd(int x);
        int getInd();
        int* getAssets();
        bool isBusy();
        void lock();
        void unlock();
        int getDebt();
        int getTurns();
        bool getLoans();
        void decreaseTurn();
        void repay();
        bool isBroke();
        void bankrupt();
        int jailTurn();
        int jail();
        int drawGoldKey(GoldKey* gold, int gnum);
        void backwards(int n);
        void Jail();
        void breakOut();
        int usedKey();
};
