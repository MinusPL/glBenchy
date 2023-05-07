#include "texture.h"

Texture::Texture() :
	width(0), height(0), internal_format(GL_RGBA), image_format(GL_RGBA), wrap_s(GL_REPEAT), wrap_t(GL_REPEAT), filter_min(GL_LINEAR_MIPMAP_LINEAR), filter_max(GL_LINEAR), name("")
{
	glGenTextures(1, &this->id);
}