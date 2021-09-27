#include <fstream>
#include <sstream>
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

class Shader
{
public:
	uint32_t ID;

	Shader(std::string vertexPath, std::string fragmentPath)
	{
		// 1. Read Shader Code from File
		std::string vertexCode,
					fragmentCode;
		std::ifstream vertexShaderFile, 
					  fragmentShaderFile;

		vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			// open 
			vertexShaderFile.open(vertexPath);
			fragmentShaderFile.open(fragmentPath);
			std::stringstream vertexShaderStream, 
							  fragmentShaderStream;

			vertexShaderStream << vertexShaderFile.rdbuf();
			fragmentShaderStream << fragmentShaderFile.rdbuf();

			vertexShaderFile.close();
			fragmentShaderFile.close();

			vertexCode = vertexShaderStream.str();
			fragmentCode = fragmentShaderStream.str();

		}
		catch (std::ifstream::failure e)
		{
			std::cout << "Error Reading Shader: " << e.what() << std::endl;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// 2. Compile Shader Code
		uint32_t vertex, 
				 fragment;
		int32_t success;
		char infoLog[512];

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "Error Compiling Vertex Shader: " << std::endl << infoLog << std::endl;
		}

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "Error Compiling Fragment Shader: " << std::endl << infoLog << std::endl;
		}

		// create shader program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "Error Compiling Shader Program: " << std::endl << infoLog << std::endl;
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);

	}

	void Use()
	{
		glUseProgram(ID);
	}

	void SetUniformBool(const std::string& name, bool value)
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void SetUniformInt(const std::string& name, int32_t value)
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void SetUniformFloat(const std::string& name, float_t value)
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		SetupMesh();
	};

	void Draw(Shader &shader)
	{
		int32_t diffuseNr = 1, 
				specularNr = 1;
		for (int32_t i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + 1);
			std::string number;
			std::string name = textures[i].type;
			if (name == "texture_diffuse")
			{
				number = std::to_string(diffuseNr++);
			}
			else if (name == "texture_specular")
			{
				number = std::to_string(specularNr++);
			}

			shader.SetUniformFloat(("material." + name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		glActiveTexture(GL_TEXTURE0);

		// draw mesh 
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	};
private:
	uint32_t VAO, VBO, EBO;
	void SetupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);


		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int32_t), &indices[0], GL_STATIC_DRAW);

		// vert pos
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vert norm
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vert tex coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	};
};

class Model
{
public:
	
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
	std::cout << "Loading scene data: " << file << std::endl;
	// Assimp Setup
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(file, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
	// If the import failed, report it
	if (nullptr == scene) {
		std::cout << "Failed to import: " << file << ": " << importer.GetErrorString() << std::endl;
		return;
	}

	std::cout << "Loaded:" << std::endl
		<< "  Meshes: " << scene->mNumMeshes << std::endl
		<< "  Materials: " << scene->mNumMaterials << std::endl
		<< "  Textures: " << scene->mNumTextures << std::endl
		<< "  Lights: " << scene->mNumLights << std::endl
		<< "  Cameras: " << scene->mNumCameras << std::endl
		<< "  Animations: " << scene->mNumAnimations << std::endl
	;

	std::cout << "Constructing Scene " << std::endl;



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

