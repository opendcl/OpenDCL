#pragma once

#include <string>

std::string base64_encode(unsigned char const* , unsigned int len, unsigned int width = 76);
std::string base64_decode(std::string const& s);
