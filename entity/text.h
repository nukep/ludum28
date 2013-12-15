#ifndef ENTITY_TEXT_H
#define ENTITY_TEXT_H

#include "tileset.h"

struct entity_text {
    const struct tileset *font;
    const char *text;
};

#endif


