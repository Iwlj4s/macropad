#include "animations_manager.h"

AnimationItem AnimationsManager::get_currentAnimation() {
    return animList[currentAnimIndex];
}

void AnimationsManager::nextAnimation() {
    currentAnimIndex++;
    if (currentAnimIndex >= TOTAL_ANIMS) {
        currentAnimIndex = 0;
    }
}

void AnimationsManager::prevAnimation() {
    currentAnimIndex--;
    if (currentAnimIndex < 0){
        currentAnimIndex = 0;
    }
}