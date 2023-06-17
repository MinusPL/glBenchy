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
	uint16_t wrap_s;
	uint16_t wrap_t;
	uint16_t filter_min;
	uint16_t filter_max;
	uint16_t internal_format;
	uint16_t image_format;
};

#endif