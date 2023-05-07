#ifndef TEXTURE_H
#define TEXTURE_H

#include "../../platform/platform.h"
#include "../object/object.h"
#include <cstdint>


class Texture : public Object
{
public:
	Texture();
	~Texture();

	GLuint id;
	std::string name;
	uint16_t width;
	uint16_t height;
	uint8_t wrap_s;
	uint8_t wrap_t;
	uint8_t filter_min;
	uint8_t filter_max;
	uint8_t internal_format;
	uint8_t image_format;
};

#endif