//Autor: Nedeljko Tesanovic
//Namjena: Program koji crta trougao sa razlicito obojenim tjemenima

#include <GL/glew.h> //Biblioteka za olaksanu upotrebu OpenGL funkcija i ekstenzija
#include <GLFW/glfw3.h> //Biblioteka za lakse stvaranje OpenGL konteksta (prozora) - include uvijek NAKON glew.h

#include <iostream>

//Za citanje iz fajla
#include <fstream>
#include <sstream>

std::string readFile(const char* filePath); //Vraca sadrzaj fajla sa putajne kao string
int compileShader(const char* filePath, GLuint shaderType); //Vraca kompajlirani sejder objekat odredjenog tipa, ciji je izvorni kod u fajlu na putanji
int createProgram(const char* vsSource, const char* fsSource); //Vraca objedinjeni sedjer program, prima putanje do izvornih fajlova verteks i fragment sejdera

int main()
{
	//Inicijalizacija GLFW biblioteke
	if (!glfwInit())
	{
		std::cout << "GLFW Init failure!\n";
		return 1;
	}

	//Dimenzije i naslov prozora
	unsigned int wWidth = 600;
	unsigned int wHeight = 600;
	const char* wTitle = "[Generic title]";

	//Biranje OpenGL verzije (3.3 Programabilni pajplajn)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Core = Programabilni pajplajn; Compatibility = Programabilni + legacy funkcije iz fiksnog pajplajna

	//Stvaranje prozora
	// Parametri (Sirina, Visina, Naslov, Monitor za fullscreen, Prozor (kontekst) sa kojim se dijele resursi)
	GLFWwindow* window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL); //Za fullscreen 4. parametar promjeniti na glfwGetPrimaryMonitor()
	if (window == NULL)
	{
		std::cout << "Context creation failure!\n";
		glfwTerminate();
		return 1;
	}
	else {
		//Postavi novostvoreni prozor kao aktivni OpenGL kontekst
		glfwMakeContextCurrent(window);
	}

	//Inicijalizacija GLEW biblioteke
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW Init failure!\n";
		glfwTerminate();
		return 1;
	}

	//Definisanje tjemena
	float vertices[] =
	{
		//Pozicija	|		Boja
		// X	Y	|	R	G	B	A
		-0.5, 0.5,	  1.0, 0.0, 0.0, 1.0,
		0.5, 0.5,	  1.0, 1.0, 0.0, 1.0,
		0.0, -0.5,	  0.0, 0.0, 1.0, 1.0,
	};

	// Korak (u bajtovima) za kretanje po tjemenima
	unsigned int stride = (2 + 4) * sizeof(float);

	//Stvaranje bafera
	unsigned int VAO; //Vertex Array Object (VBOs + Attribute Pointers)
	unsigned int VBO; //Vertex Buffer Object

	//Generisi jedan VAO
	glGenVertexArrays(1, &VAO);
	//Izaberi ga kao aktivan
	glBindVertexArray(VAO);

	//Generisi jedan bafer
	glGenBuffers(1, &VBO);
	//Izaberi novogenerisani bafer kao aktivni
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Postavi podatke bafera
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Opisi atribute, tj. kako da interpretiramo podatke tjemena
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0); // Pozicija
	glEnableVertexAttribArray(0); //Ukljuci taj nacin interpretacije
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float))); //Boja
	glEnableVertexAttribArray(1);

	//Unbind-ovanje stvari koje smo upravo generisali
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Pravimo unified Shader objekat (program)
	int shaderProgram = createProgram("basic.vert", "basic.frag");

	//Render petlja
	while (!glfwWindowShouldClose(window))
	{
		//Postavljanje boje pozadine (RGBA)
		//Parametri (float [0-1]. Veca vrijednost = svijetlija boja) : Crvena, Zelena, Plava, Alfa (0 = Potpuno providno, 1 = Potpuno neprovidno)
		glClearColor(0.5, 0.5, 0.5, 1.0);

		//Ciscenje ekrana
		glClear(GL_COLOR_BUFFER_BIT);

		//Biranje aktivnog sejdera
		glUseProgram(shaderProgram);
		//Biranje stvari koje cemo da crtamo
		glBindVertexArray(VAO);
		//Crtanje
		glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / stride);
		glBindVertexArray(0); //Oslobadjamo bafer nakon crtanja

		//Zamijena prikazanog i pozadinskog bafera
		glfwSwapBuffers(window);

		//Hvatanje dogadjaja za prozor (mijenjanje velicine, itd)
		glfwPollEvents();
	}

	//Pospremanje
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);

	glfwTerminate();
	return 0;
}

std::string readFile(const char* filePath)
{
	//Funkcija za kopiranje sadrzaja fajla u string
	//Nije najefikasniji nacin, ali radi i razumljivo je :)
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		std::cout << "Couldn't open file from " << filePath << " !\n";
		return "";
	}
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

int compileShader(const char* filePath, GLuint shaderType)
{//Vraca kompajlirani sejder objekat odredjenog tipa, ciji je izvorni kod u fajlu na putanji

	std::string temp = readFile(filePath);
	const char* sourceCode = temp.c_str();

	//Pravljejne praznog sejder objekta
	unsigned int shader = glCreateShader(shaderType);
	//Postavljanje izvornog koda sejdera
	//Parametri: ciljni sejder objekat, broj promjenljivih koje sadrze kodove, sami kodovi, duzine kodova unutar promjenljivih koje ih sadrze
	//4. parametar se moze postaviti za NULL ako stringovi imaju null terminator
	glShaderSource(shader, 1, &sourceCode, NULL);
	//Kompajliranje sejdera
	glCompileShader(shader);
	//Ispitivanje uspjesnosti kompilacije
	int success;
	char infoLog[512];
	//glGetShaderiv Vraca status flag-bitova unutar sejdera
	//Parametri: sejder, ciljni bit, odrediste za status tog bita
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		//glGetShaderInfoLog vraca poruku o gresci
		//Parametri: sejder, maksimalna duzina poruke, duzina poruke (NULL ako ima null terminator), odrediste poruke
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << shaderType << " SHADER COMPILATION FAILURE!\n InfoLog: " << infoLog << std::endl;
		return -1;
	}
	return shader;
}
int createProgram(const char* vsSource, const char* fsSource)
{
	//Vraca objedinjeni sedjer program, prima putanje do izvornih fajlova verteks i fragment sejdera

	int program = glCreateProgram();
	int vertexShader = compileShader(vsSource, GL_VERTEX_SHADER);
	int fragmentShader = compileShader(fsSource, GL_FRAGMENT_SHADER);

	//Kompajlirane sejdere zakacimo na program
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	//Povezemo izlaze zakacenih sejdera
	glLinkProgram(program);

	//Provjerimo uspjesnost na slican nacin kao kompajliranje sejdera
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "Program linking failure!\n InfoLog: " << infoLog << std::endl;
	}

	//Posto je sadrzaj sejdera sada u samom programu, rijesimo se prethodno stvorenih sejder objekata
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}