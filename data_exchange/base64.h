#ifndef BASE64_H
#define BASE64_H

#include <string>
#include <iostream>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);

#endif // BASE64_H
