#include "goldKey.h"
#include <stdlib.h>

GoldKey::GoldKey() {
    drawn = false;
    used = false;
}
bool GoldKey::isDrawn() {
    return drawn;
}
bool GoldKey::isUsed() {
    return used;
}
void GoldKey::draw() {
    drawn = true;
}
void GoldKey::use() {
    used = true;
}
void GoldKey::setDescription(char* s) {
    description = s;
}
char* GoldKey::getDescription() {
    return description;
}
void GoldKey::setUsedWhenDrawn() {
    useWhenDrawn = true;
}
bool GoldKey::isUsedWhenDrawn() {
    return useWhenDrawn;
}
