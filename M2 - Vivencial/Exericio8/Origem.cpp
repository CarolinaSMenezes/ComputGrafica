/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Jogos Digitais - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 02/03/2022
 * 
 * Modificado por: Carolina Menezes
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"


// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções
int numero_vertices = 0;
int setupGeometry();
GLFWwindow* initializeGL();
int setupShader();

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Variáveis para controlar a rotação em cada eixo
bool rotacionarX=false, rotacionarY=false, rotacionarZ=false;

// Código-fonte do vertex shader (em GLSL)
const GLchar* vertexShaderSource = "#version 450\n"
"layout (location = 0) in vec3 position;\n"
"uniform mat4 model;\n"
"void main()\n"
"{\n"
//...pode ter mais linhas de código aqui!
"gl_Position = model * vec4(position, 1.0);\n"
"}\0";

//Códifo fonte do Fragment Shader (em GLSL)
const GLchar* fragmentShaderSource = "#version 450\n"
"uniform vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = finalColor;\n"
"}\n\0";



// Função MAIN
int main()
{   // Inicializa a janela GLFW
    GLFWwindow* window = initializeGL();

	// Configura o shader
	GLuint shaderID = setupShader();

	// Configura a geometria
	GLuint VAO = setupGeometry();

	// Define o shader em uso
	glUseProgram(shaderID);

	// Configura a matriz de modelo
	glm::mat4 model = glm::mat4(1); 
	GLint modelLoc = glGetUniformLocation(shaderID, "model");

	// Rotaciona a matriz de modelo
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, false, glm::value_ptr(model));

	GLint colorLoc = glGetUniformLocation(shaderID, "finalColor");
	glUniform4f(colorLoc, 0.5f, 0.0f, 0.5f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
		// Limpa o buffer de cor e o buffer de profundidade
		glClearColor(1.0f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Obtém o ângulo atual do tempo
		float angle = (GLfloat)glfwGetTime();

		// Define a matriz de modelo de acordo com a rotação selecionada
		model = glm::mat4(1);
		if (rotacionarX)
		{
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else if (rotacionarY)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (rotacionarZ)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		glUniformMatrix4fv(modelLoc, 1, false, glm::value_ptr(model));

		// Renderiza o modelo de Suzanne
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, (numero_vertices * 3));
		glUniform4f(colorLoc, 0.0, 0.0, 0.0, 1.0);

		// Renderiza as arestas do modelo
		for (int n = 0; n < numero_vertices; n += 3) {
			glDrawArrays(GL_LINE_LOOP, n, 3);
		}
		glUniform4f(colorLoc, 0.5f, 0.4f, 0.4f, 0.5f);

        glBindVertexArray(0);

		// Troca os buffers e processa eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
	// Libera recursos
	glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}

// Função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// Verifica se a tecla ESC foi pressionada para fechar a janela
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Verifica se a tecla X foi pressionada para rotacionar em torno do eixo X
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotacionarX = true;
		rotacionarY = false;
		rotacionarZ = false;
	}

	// Verifica se a tecla Y foi pressionada para rotacionar em torno do eixo Y
	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotacionarX = false;
		rotacionarY = true;
		rotacionarZ = false;
	}

	// Verifica se a tecla Z foi pressionada para rotacionar em torno do eixo Z
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotacionarX = false;
		rotacionarY = false;
		rotacionarZ = true;
	}

}

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
int setupGeometry()
{
	std::vector< glm::vec3 > vertexIndices = {}; // Vetor de índices dos vértices
	std::vector< glm::vec3 > triangulos = {};// Vetor de triângulos
	string line;
	string v, valuesX, valuesY, valuesZ;

	ifstream myfile("suzanneTri.obj"); // Abre o arquivo que contém a geometria

	while (!myfile.eof())
	{
		getline(myfile, line);
		std::istringstream iss(line);

		// Verifica se a linha contém as coordenadas de um vértice
		if (line[0] == 'v' && line[1] == ' ') {
			iss >> v >> valuesX >> valuesY >> valuesZ;
			glm::vec3 aux = { std::stof(valuesX), std::stof(valuesY), std::stof(valuesZ) };
			vertexIndices.push_back(aux);
		}

		 // Verifica se a linha contém a definição de um triângulo
		if (line[0] == 'f' && line[1] == ' ')
		{

			string delimiter = " ";
			string delimiter1 = "/";
			int primeira_parte = 0;
			int segunda_parte = 0;
			int terceira_parte = 0;
			auto finish = line.find(delimiter); // Encontra a posição do primeiro espaço em branco
			line = line.substr(finish + 1);

			if (finish != line.npos) { // Verifica se o primeiro espaço em branco foi encontrado
				primeira_parte = stoi(line.substr(0, line.find(delimiter1))); // 
			}

			finish = line.find(delimiter); // Encontra a posição do próximo espaço em branco
			line = line.substr(finish + 1);

			if (finish != line.npos) { // Verifica se o próximo espaço em branco foi encontrado
				segunda_parte = stoi(line.substr(0, line.find(delimiter1))); // 
			}

			finish = line.find(delimiter); // Encontra a posição do próximo espaço em branco
			line = line.substr(finish + 1);

			if (finish != line.npos) { // Verifica se o próximo espaço em branco foi encontrado
				terceira_parte = stoi(line.substr(0, line.find(delimiter1))); // 
			}

			triangulos.push_back(vertexIndices[terceira_parte - 1]);
			triangulos.push_back(vertexIndices[segunda_parte - 1]);
			triangulos.push_back(vertexIndices[primeira_parte - 1]);


			numero_vertices++;
		}
	}

	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, triangulos.size() * sizeof(GLfloat) * 3, &triangulos[0], GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);



	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

GLFWwindow* initializeGL()
{
	// Inicialização da GLFW
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		return nullptr;
	}

	// Configuração do contexto OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "M2 Vivencial - Carolina Menezes", nullptr, nullptr);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	// Inicialização do GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	return window;
}

int setupShader()
{
	// Vertex shader
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

	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	return shaderProgram;
}