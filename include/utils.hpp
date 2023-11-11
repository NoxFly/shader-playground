/**
 * @author Dorian Thivolle
 */

#pragma once

#include <string_view>
#include <string>

/**
 * Removes the leading whitespaces from a string.
 * Does not modify the given string.
 * Returns the modified string.
 */
std::string ltrim(const std::string& str);

/**
 * Removes the trailing whitespaces from a string.
 * Does not modify the given string.
 * Returns the modified string.
 */
std::string rtrim(const std::string& str);

/**
 * Removes the leading and trailing whitespaces from a string.
 * Does not modify the given string.
 * Returns the modified string.
 */
std::string trim(const std::string& str);

/**
 * Replaces a substring in the given string
 * @param str The main string
 * @param find The substring to replace
 * @param replace The substring to replace by
 * @return The modified string
 */
std::string replace(const std::string& str, const std::string& find, const std::string& replace);