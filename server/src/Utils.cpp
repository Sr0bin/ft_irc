#include "Utils.hpp"
#include <algorithm>
#include <cctype>

static int safeTolower(int c) {
	return (std::tolower(static_cast<unsigned char>(c)));
}

std::string Utils::toLower(const std::string &s) {
	std::string result = s;
	std::transform(result.begin(), result.end(), result.begin(), safeTolower);
	return (result);
}

bool Utils::ircEquals(const std::string &a, const std::string &b) {
	return (toLower(a) == toLower(b));
}