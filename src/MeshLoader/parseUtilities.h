/*H******************************************************************
 * FILENAME: parseUtilities.h
 * AUTHOR: Zackary Finer
 *
 * DESCRIPTION:
 * provides prototypes for parse utitilities
 */
#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <regex>

namespace parseUtilities
{
	std::vector<std::string> split(const std::string&, const std::string&, bool);
}