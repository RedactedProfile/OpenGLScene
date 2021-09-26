#include <iostream>
#include <string>
#include <vector>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm\glm.hpp>

#include <cstdio>
#include <rapidjson\rapidjson.h>
#include <rapidjson\document.h>
#include <rapidjson\filereadstream.h>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

struct Config
{
	static inline int32_t screen_width = 800;
	static inline int32_t screen_height = 600;
	static inline bool fullscreen = false;
	static inline bool vsync = false;
	static inline std::string win_title = "Whatever";
	static inline std::string scene = "";
} Config;

struct State
{
	static inline SDL_Window* m_window;
	static inline SDL_GLContext m_glContext;
	static inline int32_t m_time = 0;
	static inline int32_t m_deltaTime = 0;
} State;


struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 TexCoords;
};

struct Texture 
{
	uint32_t id;
	std::string type;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures);
private:
	uint32_t VAO, VBO, EBO;
};


void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	std::cout << "[OpenGL Error](" << type << ") " << message << std::endl;
}

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
		Config::fullscreen = _configDoc["fullscreen"].GetBool();
	
	if (_configDoc.HasMember("vsync") && _configDoc["vsync"].IsBool())
		Config::vsync = _configDoc["vsync"].GetBool();
	
	if (_configDoc.HasMember("win_title") && _configDoc["win_title"].IsString())
		Config::win_title = _configDoc["win_title"].GetString();

	if (_configDoc.HasMember("scene") && _configDoc["scene"].IsString())
		Config::scene = _configDoc["scene"].GetString();
	
}

void LoadScene(const std::string file)
{
	std::cout << "Loading scene: " << file << std::endl;
	// Assimp Setup
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(file, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
	// If the import failed, report it
	if (nullptr == scene) {
		std::cout << "Failed to import: " << file << ": " << importer.GetErrorString() << std::endl;
		return;
	}

	// Room

	
	// Sphere 

	// Light
}

void Clear()
{
	glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Update()
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
	std::cout << "Launching " << Config::win_title << std::endl;

	bool quit = false;
	SDL_Event event;

	SDL_Init(SDL_INIT_EVERYTHING);

	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	SDL_Window* m_window = SDL_CreateWindow(Config::win_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Config::screen_width, Config::screen_height, SDL_WINDOW_OPENGL);
	if (!m_window) 
	{
		std::cout << "Could not create window: " << SDL_GetError() << std::endl;
		return 0;
	}
	State::m_window = m_window;

	SDL_GLContext m_glContext = SDL_GL_CreateContext(m_window);
	if (!m_glContext)
	{
		std::cout << "Could not create context: " << SDL_GetError() << std::endl;
		return 0;
	}
	State::m_glContext = m_glContext;

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "glew failed to initialize: " << glewGetErrorString(err) << std::endl;
		return false;
	}

#if _DEBUG 
	// Enable debug output
	if (glDebugMessageCallback)
	{
		std::cout << "Registering OpenGL Debug callback" << std::endl;
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(MessageCallback, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
	}
	else 
	{
		std::cout << "glDebugMessageCallback not available" << std::endl;
	}
#endif

	SDL_GL_MakeCurrent(State::m_window, State::m_glContext);

	std::cout << "GLVERSION: " << glGetString(GL_VERSION) << std::endl;

	// VSync
	if (Config::vsync)
	{
		if (SDL_GL_SetSwapInterval(1) < 0)
		{
			std::cout << "Couldn't set vsync" << std::endl;
			return false;
		}
	}
	


	LoadScene(Config::scene);

	State::m_time = SDL_GetTicks();
	while (!quit)
	{
		SDL_WaitEvent(&event);

		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		}

		uint32_t now = SDL_GetTicks();
		State::m_deltaTime = now - State::m_time;

		Clear();
		Update();
		LateUpdate();
		Present();

		State::m_time = now;
	}

	SDL_Quit();

	return 0;
}

