/* Hello Triangle - c�digo adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gr�fico - Jogos Digitais - Unisinos
 * Vers�o inicial: 7/4/2017
 * �ltima atualiza��o em 24/06/2022
 * 
 * Modificado por: Carolina Menezes
 *
 */
 

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Fun��o de callback para eventos de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Fun��es para configurar o shader e a geometria
int setupShader();
int setupGeometry();

const GLuint WIDTH = 1000, HEIGHT = 1000;

// C�digo-fonte do vertex shader
const GLchar* vertexShaderSource = "#version 450\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
"gl_Position = model * vec4(position, 1.0);\n"
"finalColor = vec4(color, 1.0);\n"
"}\0";

// C�digo-fonte do fragment shader
const GLchar* fragmentShaderSource = "#version 450\n"
"in vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = finalColor;\n"
"}\n\0";

bool rotateX, // Flag para rota��o no eixo X
rotateY, // Flag para rota��o no eixo Y
rotateZ = false; // Flag para rota��o no eixo Z
float scale = 0.9f; // Fator de escala
glm::vec3 translation(0.0f, 0.0f, 0.0f); // Vetor de transla��o

int main()
{
	// Inicializa��o da biblioteca GLFW
	glfwInit();

	// Cria��o da janela GLFW com largura, altura e t�tulo especificados
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "M2 -- Carolina Menezes", nullptr, nullptr);

	// Define a janela GLFW como contexto atual
	glfwMakeContextCurrent(window);

	// Define a fun��o de callback para eventos de teclado
	glfwSetKeyCallback(window, key_callback);

	// Inicializa��o da biblioteca GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Obt�m informa��es sobre o renderizador e a vers�o do OpenGL
	const GLubyte* renderer = glGetString(GL_RENDERER); /* Obtem a informa��o sobre o renderizador */
	const GLubyte* version = glGetString(GL_VERSION); /* Obtem a vers�o do OpenGL como uma string */
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version supported: " << version << std::endl;

	// Obt�m o tamanho do framebuffer da janela
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	// Define a �rea de visualiza��o (viewport)
	glViewport(0, 0, width, height);

	// Configura o shader
	GLuint shaderID = setupShader();

	// Configura a geometria
	GLuint VAO = setupGeometry();

	// Usa o shader especificado
	glUseProgram(shaderID);

	// Define a matriz de transforma��o do modelo
	glm::mat4 model = glm::mat4(1);
	GLint modelLoc = glGetUniformLocation(shaderID, "model");
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Habilita o teste de profundidade
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		// Processa eventos de input
		glfwPollEvents();

		// Limpa o buffer de cor e profundidade com a cor de fundo especificada
		glClearColor(1.0f, 0.9f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Define a largura da linha e o tamanho do ponto
		glLineWidth(10);
		glPointSize(20);

		// Obt�m o �ngulo atual com base no tempo
		float angle = (GLfloat)glfwGetTime();

		// Define a matriz de transforma��o do modelo
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(scale, scale, scale));
		model = glm::translate(model, translation);

		// Verifica as vari�veis de rota��o e aplica a rota��o adequada
		if (rotateX)
		{
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else if (rotateY)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (rotateZ)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		// Atualiza a matriz de transforma��o do modelo no shader
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// Desenha a geometria
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 72);
		glDrawArrays(GL_POINTS, 0, 72);
		glBindVertexArray(0);

		// Troca os buffers (duplo buffer)
		glfwSwapBuffers(window);
	}

	// Libera os recursos da geometria
	glDeleteVertexArrays(1, &VAO);

	// Encerra a biblioteca GLFW
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// Verifica se a tecla foi pressionada
	if (action != GLFW_PRESS)
		return;

	switch (key)
	{
		// Tecla ESC - Sair do programa
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;

		// Tecla X - Rotacionar no eixo X
	case GLFW_KEY_X:
		rotateX = true;
		rotateY = false;
		rotateZ = false;
		break;

		// Tecla Y - Rotacionar no eixo Y
	case GLFW_KEY_Y:
		rotateX = false;
		rotateY = true;
		rotateZ = false;
		break;

		// Tecla Z - Rotacionar no eixo Z
	case GLFW_KEY_Z:
		rotateX = false;
		rotateY = false;
		rotateZ = true;
		break;

		// Tecla Seta para Baixo - Diminuir escala
	case GLFW_KEY_DOWN:
		scale -= 0.1f;
		break;

		// Tecla Seta para Cima - Aumentar escala
	case GLFW_KEY_UP:
		scale += 0.1f;
		break;

		// Tecla D - Mover para direita
	case GLFW_KEY_D:
		translation.x += 0.1f;
		break;

		// Tecla A - Mover para esquerda
	case GLFW_KEY_A:
		translation.x -= 0.1f;
		break;

		// Tecla W - Mover para cima
	case GLFW_KEY_W:
		translation.y += 0.1f;
		break;

		// Tecla S - Mover para baixo
	case GLFW_KEY_S:
		translation.y -= 0.1f;
		break;

		// Tecla I - Mover no eixo Z para frente
	case GLFW_KEY_I:
		translation.z += 0.1f;
		break;

		// Tecla J - Mover no eixo Z para tr�s
	case GLFW_KEY_J:
		translation.z -= 0.1f;
		break;
	}
}

int setupShader()
{
    // Cria��o do objeto shader de v�rtices
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // Cria��o do objeto shader de fragmentos
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // Cria��o do programa de shader e vincula��o dos shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    // Libera��o da mem�ria dos shaders, j� que n�o s�o mais necess�rios ap�s a vincula��o
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int setupGeometry()
{
    // Defini��o dos v�rtices do objeto a ser renderizado
    GLfloat vertices[] = {
		// Quadrado 1 / Cubo 1
		-1.0, -1.0, 0.25,		1.0, 0.5, 0.5,
		-1.0, -0.5, 0.25,		1.0, 0.5, 0.5,
		-0.5, -1.0, 0.25,		1.0, 0.5, 0.5,

		-1.0, -0.5, 0.25,		1.0, 0.1, 0.6,
		-0.5, -0.5, 0.25,		1.0, 0.1, 0.6,
		-0.5, -1.0, 0.25,		1.0, 0.1, 0.6,

		// Quadrado 2 / Cubo 1
		-1.0, -1.0, -0.25,	    0.3, 0.3, 0.3,
		-1.0, -0.5, -0.25,	    0.3, 0.3, 0.3,
		-0.5, -1.0, -0.25,	    0.3, 0.3, 0.3,

		-1.0, -0.5, -0.25,	    0.1, 0.1, 0.1,
		-0.5, -0.5, -0.25,	    0.1, 0.1, 0.1,
		-0.5, -1.0, -0.25,	    0.1, 0.1, 0.1,

		// Quadrado 3 / Cubo 1
		-1.0, -1.0,  0.25,	    0.5, 1.0, 1.0,
		-1.0, -0.5,  0.25,    	0.5, 1.0, 1.0,
		-1.0, -1.0, -0.25,	    0.5, 1.0, 1.0,

		-1.0, -0.5,  0.25,		0.1, 0.5, 1.0,
		-1.0, -0.5, -0.25,	    0.1, 0.5, 1.0,
		-1.0, -1.0, -0.25,	    0.1, 0.5, 1.0,

		// Quadrado 4 / Cubo 1
		-0.5, -1.0,  0.25,		1.0, 1.0, 0.0,
		-0.5, -0.5,  0.25,		1.0, 1.0, 0.0,
		-0.5, -1.0, -0.25,	    1.0, 1.0, 0.0,

		-0.5, -0.5,  0.25,		1.0, 1.0, 0.7,
		-0.5, -0.5, -0.25,	    1.0, 1.0, 0.7,
		-0.5, -1.0, -0.25,	    1.0, 1.0, 0.7,

		// Quadrado 5 / Cubo 1
		-1.0, -0.5,  0.25,		1.0, 0.5, 1.0,
		-0.5, -0.5,  0.25,		1.0, 0.5, 1.0,
		-1.0, -0.5, -0.25,	    1.0, 0.5, 1.0,

		-0.5, -0.5, 0.25,		1.0, 0.0, 1.0,
		-0.5, -0.5, -0.25,	    1.0, 0.0, 1.0,
		-1.0, -0.5, -0.25,	    1.0, 0.0, 1.0,

		// Quadrado 6 / Cubo 1
		-1.0, -1.0,  0.25,		0.0, 1.0, 1.0,
		-0.5, -1.0,  0.25,		0.0, 1.0, 1.0,
		-1.0, -1.0, -0.25,	    0.0, 1.0, 1.0,

		-0.5, -1.0,  0.25,		0.7, 1.0, 1.0,
		-0.5, -1.0, -0.25,	    0.7, 1.0, 1.0,
		-1.0, -1.0, -0.25,	    0.7, 1.0, 1.0,

		// Quadrado 1 / Cubo 2
		0.5, 0.0, 0.0,			1.0, 0.5, 0.5,
		0.5, 0.5, 0.0,			1.0, 0.5, 0.5,
		1.0, 0.0, 0.0,			1.0, 0.5, 0.5,

		0.5, 0.5, 0.0,			1.0, 0.1, 0.6,
		1.0, 0.5, 0.0,			1.0, 0.1, 0.6,
		1.0, 0.0, 0.0,			1.0, 0.1, 0.6,

		// Quadrado 2 / Cubo 2
		0.5, 0.0, -0.5,			0.3, 0.3, 0.3,
		0.5, 0.5, -0.5,			0.3, 0.3, 0.3,
		1.0, 0.0, -0.5,			0.3, 0.3, 0.3,

		0.5, 0.5, -0.5,			0.1, 0.1, 0.1,
		1.0, 0.5, -0.5,			0.1, 0.1, 0.1,
		1.0, 0.0, -0.5,			0.1, 0.1, 0.1,

		// Quadrado 3 / Cubo 2
		0.5, 0.0, 0.0,			 0.5, 1.0, 1.0,
		0.5, 0.5, 0.0,			 0.5, 1.0, 1.0,
		0.5, 0.0, -0.5,			 0.5, 1.0, 1.0,

		0.5, 0.5, 0.0,			0.1, 0.5, 1.0,
		0.5, 0.5, -0.5,			0.1, 0.5, 1.0,
		0.5, 0.0, -0.5,			0.1, 0.5, 1.0,

		// Quadrado 4 / Cubo 2
		1.0, 0.0,  0.0,			1.0, 1.0, 0.0,
		1.0, 0.5,  0.0,			1.0, 1.0, 0.0,
		1.0, 0.0, -0.5,			1.0, 1.0, 0.0,

		1.0, 0.5,  0.0,			1.0, 1.0, 0.7,
		1.0, 0.5, -0.5,		    1.0, 1.0, 0.7,
		1.0, 0.0, -0.5,			1.0, 1.0, 0.7,

		// Quadrado 5 / Cubo 2
		0.5, 0.5,  0.0,			1.0, 0.5, 1.0,
		1.0, 0.5,  0.0,			1.0, 0.5, 1.0,
		0.5, 0.5, -0.5,			1.0, 0.5, 1.0,

		1.0, 0.5,  0.0,			1.0, 0.0, 1.0,
		1.0, 0.5, -0.5,			1.0, 0.0, 1.0,
		0.5, 0.5, -0.5,			1.0, 0.0, 1.0,

		// Quadrado 6 / Cubo 2
		0.5, 0.0,  0.0,			0.0, 1.0, 1.0,
		1.0, 0.0,  0.0,			0.0, 1.0, 1.0,
		0.5, 0.0, -0.5,			0.0, 1.0, 1.0,

		1.0, 0.0,  0.0,			0.7, 1.0, 1.0,
		1.0, 0.0, -0.5,         0.7, 1.0, 1.0,
		0.5, 0.0, -0.5,         0.7, 1.0, 1.0,
	};

	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

