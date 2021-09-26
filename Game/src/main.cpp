#include <iostream>
#include <string>
#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <cstdio>
#include <rapidjson\rapidjson.h>
#include <rapidjson\document.h>
#include <rapidjson\filereadstream.h>

struct Config
{
	static inline int16_t screen_width = 800;
	static inline int16_t screen_height = 600;
	static inline std::string win_title = "Whatever";
} Config;


void ParseConfig()
{
	FILE* fp = fopen("config.json", "rb");
	char readBuffer[65536];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	rapidjson::Document _configDoc;
	_configDoc.ParseStream(is);

	fclose(fp);

	if (_configDoc.HasMember("screen_width") && _configDoc["screen_width"].IsInt())
		Config::screen_width = _configDoc["screen_width"].GetInt();
	if (_configDoc.HasMember("screen_height") && _configDoc["screen_height"].IsInt())
		Config::screen_height = _configDoc["screen_height"].GetInt();
	if (_configDoc.HasMember("win_title") && _configDoc["win_title"].IsString())
		Config::win_title = _configDoc["win_title"].GetString();
	
}

int main()
{
	ParseConfig();
	std::cout << "Launching " << Config::win_title;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* screen = SDL_CreateWindow(Config::win_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Config::screen_width, Config::screen_height, 0);

	SDL_Delay(3000);

	SDL_Quit();

	return 0;
}

