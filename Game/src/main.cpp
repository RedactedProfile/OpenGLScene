#include <iostream>
#include <string>
#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <GL/glew.h>

#include <cstdio>
#include <rapidjson\rapidjson.h>
#include <rapidjson\document.h>
#include <rapidjson\filereadstream.h>

struct Config
{
	static inline int16_t screen_width = 800;
	static inline int16_t screen_height = 600;
	static inline bool fullscreen = false;
	static inline std::string win_title = "Whatever";
} Config;

struct State
{
	static inline SDL_Window* m_window;
	static inline SDL_GLContext m_glContext;
} State;


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
	if (_configDoc.HasMember("fullscreen") && _configDoc["fullscreen"].IsBool())
		Config::screen_height = _configDoc["fullscreen"].GetBool();
	if (_configDoc.HasMember("win_title") && _configDoc["win_title"].IsString())
		Config::win_title = _configDoc["win_title"].GetString();
	
}

void Clear()
{
	glClearColor(100.0f / 1000, 149.0f / 1000, 237.0f / 1000, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Update()
{
	
	

}

void FixedUpdate()
{

}

void LateUpdate()
{
	glFlush();
}

void Present()
{
	SDL_GL_SwapWindow(State::m_window);
}

int main()
{
	ParseConfig();
	std::cout << "Launching " << Config::win_title;

	bool quit = false;
	SDL_Event event;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	

	SDL_Window* m_window = SDL_CreateWindow(Config::win_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Config::screen_width, Config::screen_height, SDL_WINDOW_OPENGL);
	if (!m_window) 
	{
		std::cout << "Could not create window: " << SDL_GetError() << "\n";
		return 0;
	}
	State::m_window = m_window;

	SDL_GLContext m_glContext = SDL_GL_CreateContext(m_window);
	if (!m_glContext)
	{
		std::cout << "Could not create context: " << SDL_GetError() << "\n";
		return 0;
	}
	State::m_glContext = m_glContext;

	SDL_GL_MakeCurrent(State::m_window, State::m_glContext);

	while (!quit)
	{
		SDL_WaitEvent(&event);

		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		}

		Clear();
		Update();
		FixedUpdate();
		LateUpdate();
		Present();
	}

	SDL_Quit();

	return 0;
}

