#include "iostream"

#include<glad/glad.h> //Glad kütüphanesi GLFW den önce tanımlanmalı. Çünkü GLFW OpenGL fonksiyonlarını yüklerken glad kütüphanesini kullanıyor.

#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>

#include "kutuphane.hpp"

// Programı biz işlemcide derlemeyeceğimiz için
// Program ekran kartına yollanacağı için 
// Ekran kartına karakter dizisi olarak yollanacaktır

//vertex shader kaynak kodu
char* vssource =
"#version 330 core\n						\
layout (location = 0) in vec3 inPosition;	\
void main()									\
{											\
	gl_Position = vec4(inPosition, 1.0);	\
}";

//fragment shader kaynak kodu
char* fssource =
"#version 330 core\n 							\
out vec4 fragColor; 							\
void main() 									\
{ 												\
	fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); 	\
}";

// Üçgen için 3 nokta ve her bir nokta için x,y,z bilgileri mevcut.
float vertices[] = {
	-0.6f, -0.6f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
};

unsigned int programId; //program id'sini yakalamak için değişken
int main(int argc, char** argv)
{
	if(!glfwInit()){
		std::cerr << "GLFW baslatilamadi" << std::endl;
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "Pencere adi", NULL, NULL); //Pencere oluşturdum
	
	glm::mat4 mat;
	//Pencereyi doğru mu oluşturdum? Hata oldu mu kontrol et.
	if(!window){
		std::cerr << "GLFW penceresi olusturulamadi" << std::endl;
		glfwTerminate(); //GLFW nün aldığı katnakları serbest bırakıyorum
		return -1; //Bitir
	}

	// Pencereyi ekranda göster. Hangi pencere üzerinde çizim yapılacak göster
	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Program nesnesi oluşturma ve program nesnesinin ID'sini bize döndürür.
	programId = glCreateProgram();
	// Bir program nesnesi oluşturduk. Bunu içerisi şuan boş. Buna vertex ve program shader göndereceğim.

	//Öncelikle vertex
	unsigned int vertexShaderId = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShaderId, 1, &vssource, NULL);

	glCompileShader(vertexShaderId);
	//Vertex shader oluşturuldu ve Ekran kartına bunu derlemesini söyledik.

	//Fragment Shader
	unsigned int vertexFragmentId = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexFragmentId, 1, &fssource, NULL);

	//Fragment shader oluşturuldu ve Ekran kartına bunu derlemesini söyledik
	glCompileShader(vertexFragmentId);

	//İki shader nesnem oluşturuldu ve derlendi. Şimdi bunları program nesneme yükleyeceğim.
	glAttachShader(programId, vertexShaderId);

	glAttachShader(programId, vertexFragmentId);

	glLinkProgram(programId);

	/* 
	Opengl vertex  bilgilerini direk dire bir dizi içerisinde tutamıyor. 
	Vertex Buffer Object denen nesneler içerinde barındırıyor bu nesneleri
	*/
	unsigned int VBO; //Vertex Buffer Object
	unsigned int VAO;

	/*
	Bu fonksiyon buffer oluşturmak için kullanılmaktadır.
	İlk parametre kaç buffer oluşturulacak, diğer parametre
	*/ 
	glGenBuffers(1, &VAO);
	glGenBuffers(1, &VBO); //VBO oluşturuldu

	glGenVertexArrays(1,&VAO);
	/*
	OpenGl aktif olan bufferda işlem yapıyor. Buffer'ı aktif etmiyor.
	Dolayısı ile Buffer öncelikle aktif edilmelidir.
	glBindBuffer: Aktif edilecek Buffer türü, Aktif edlecek Buffer ID'si
	*/

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices,GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 3*sizeof(float),(void*)0);

	glEnableVertexAttribArray(0);

	/* 
	Pencere kapatılana kadar devam eden döngü.
	glfwWindowShouldClose pencerenin kapatılıp kapatılmadığını kontrol eder.
	Eğer pencere kapatılmışsa 0 döndürür.
	Bu yüzden pencere kapatılmadığı sürece döngü devam eder.
	*/
	while(glfwWindowShouldClose(window) == 0){
		glClearColor(0.0f,0.4f,0.7f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(programId); //Programı kullanmaya başla

		glBindVertexArray(VAO); //Vertex Array Object'i kullanmaya başla
		glEnableVertexAttribArray(0); //Vertex Array Object'i kullanmaya başla
		glDrawArrays(GL_TRIANGLES, 0, 3); //Üçgen çiz

		// Pencereyi güncelle
		glfwSwapBuffers(window);
		// Olayları kontrol et. Kullanıcıdan gelen inputları al fare tıklanması gibi
		glfwPollEvents();
	}
}
