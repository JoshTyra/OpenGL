#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;  // Store the path of the texture to compare with other textures
};

#endif // TEXTURE_H