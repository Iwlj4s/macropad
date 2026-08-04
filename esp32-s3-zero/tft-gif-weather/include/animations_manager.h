#ifndef ANIMATIONS_MANAGER_H
#define ANIMATIONS_MANAGER_H

#include <Arduino.h>
#include "animations_data.h"

struct AnimationItem {
    const uint8_t* data;
    unsigned int size;
    const char* name;
};

class AnimationsManager {
    private:
        inline static const AnimationItem animList[] = {
            { cat_fire_anim, cat_fire_size, "Cat near campfire"},
            { cat_in_boat_anim, cat_in_boat_size, "Cat in Boat" }
        };

    public:
        const int TOTAL_ANIMS = sizeof(animList) / sizeof(animList[0]);
        int currentAnimIndex = 0;

        AnimationItem get_currentAnimation();
        void nextAnimation();
        void prevAnimation();
    
};

#endif