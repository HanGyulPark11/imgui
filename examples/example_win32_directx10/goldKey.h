#pragma once
class GoldKey {
private:
    bool drawn;
    bool used;
    char* description;
    bool useWhenDrawn = false;

public:
    GoldKey();
    bool isDrawn();
    bool isUsed();
    void draw();
    void use();
    void setDescription(char* s);
    char* getDescription();
    void setUsedWhenDrawn();
    bool isUsedWhenDrawn();
};
