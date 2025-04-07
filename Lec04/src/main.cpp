#include <iostream>

#include<glad/glad.h>

#include<GLFW/glfw3.h>
#include "shaderprogram.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


float vertices[] = {
    // Yakın üçgen (z = -1.5f) — yeşil
    -0.3f, -0.3f, -0.5f,  // Sol alt
     0.3f, -0.3f, -0.5f,  // Sağ alt
     0.0f,  0.3f, -0.5f,  // Üst

    // Uzak üçgen (z = -0.5f) — kırmızı
    -0.5f, -0.5f, -1.5f,  // Sol alt
     0.5f, -0.5f, -1.5f,  // Sağ alt
     0.0f,  0.5f, -1.5f   // Üst
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

    // === Derinlik ayarları ===
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearDepthf(1.0f);
    glDepthRangef(0.0f, 1.0f);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 640.0f/480.0f, 0.1f, 10.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -2.0f));
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;

    int mvpLoc = glGetUniformLocation(program.getId(), "uMVP");


    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.4f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Z-buffer temizliği

        program.use();
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
        glBindVertexArray(VAO);

        // Yeşil yakın üçgen
        glUniform4f(glGetUniformLocation(program.getId(), "uColor"), 0.0f, 1.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Kırmızı uzak üçgen
        glUniform4f(glGetUniformLocation(program.getId(), "uColor"), 1.0f, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 3, 3);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();

}