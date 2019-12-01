/*H******************************************************************
 * FILENAME: parseUtilities.cpp
 * AUTHOR: Zackary Finer
 *
 * DESCRIPTION:
 * implementation for parse utilities functions
 */
#include "parseUtilities.h"

/*
	Split's a string into sub strings using a delimiter

	@param location: std::string - the string to be split
	@param delim: std::string - the delimiter to be used to split the location string
	@param ignoreEmpty: bool - a flag used to represent whether to discard empty strings from the result

	@return: std::vector<std::string> - a vector containing all sub strings which were split
*/
std::vector<std::string> parseUtilities::split(const std::string& location, const std::string& delim, bool ignoreEmpty)
{
	int match_i = 0;
	int initial_match = 0;
	int stream_pos = 0;
	std::stringstream buffer;
	std::vector<std::string> tokens;
	for (int i = 0; i < location.size(); i++)
	{
		if (match_i == delim.size())//if we've had a match
		{
			auto debug = buffer.str().size();
			if ((!ignoreEmpty || (buffer.str().size() > 0)) && buffer.str() != delim)
				tokens.push_back(buffer.str().substr(0, initial_match));
			buffer.str(std::string());//clear buffer
			stream_pos = 0;//reset read head
			match_i = 0;
			initial_match = 0;
		}
		if (location[i] == delim[match_i])
		{
			if (match_i == 0)
				initial_match = stream_pos;
			match_i++;
		}
		else
		{
			match_i = 0;
			initial_match = 0;
		}
		buffer << location[i];
		stream_pos++;

	}
	if ((!ignoreEmpty || (buffer.str().size() > 0)) && buffer.str() != delim)
		tokens.push_back(buffer.str());
	return tokens;
}