#include <iostream>

#include<glad/glad.h>

#include<GLFW/glfw3.h>
#include "shaderprogram.hpp"


//noktalara ait koordinat bilgileri.
float vertices[] = {
    -0.6f, -0.6f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f,
     0.0f,  0.5f, 0.0f,
    -0.8f,  0.9f, 0.0f,
     0.8f,  0.9f, 0.0f
}; 

//OpenGL nesnelerinin id değerlerini tutacak olan değişkenler

unsigned int VBO;
unsigned int VAO;

int main(int argc,char** argv)
{
    if(!glfwInit()){
		std::cerr << "GLFW baslatilamadi" << std::endl;
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "Pencere adi", NULL, NULL); //Pencere oluşturdum
	
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

    ShaderProgram program;

    program.attachShader("../shaders/simplevs.glsl",GL_VERTEX_SHADER);
    program.attachShader("../shaders/simplefs.glsl",GL_FRAGMENT_SHADER);
    program.link();

    //vertex array object oluşturuluyor
    glGenVertexArrays(1, &VAO); 
    //vertex buffer object oluşuruluyor
    glGenBuffers(1,&VBO);

    //vertex array object aktif edildi.
    glBindVertexArray(VAO);
    //vertex buffer nesnesi aktif edildi.
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    //vertex buffer'a nokta bilgileri ytollanıyor
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    //gönderilen vertex'e ait özellikler etiketleniyor
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //özellik  etiket i aktif ediliyor.
    glEnableVertexAttribArray(0); 


    while(!glfwWindowShouldClose(window))
    {
        //oluşturulacak resim başlangıç rengine boyanıyor
        glClearColor(0.0f, 0.4f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        //çizimde kullanılacak olan program nesnesi aktif ediliyor
        program.use();
        //çizimde kullanılacak olan Vertex array object aktif ediliyor
        glBindVertexArray(VAO);
        //çizim komutu gönderiliyor
        glDrawArrays(GL_TRIANGLES, 0, 6);


        glfwSwapBuffers(window);

        glfwPollEvents();
    }

}