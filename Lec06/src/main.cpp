#include <iostream>
#include <vector>
#include <fstream>
#include<glad/glad.h>

#include<GLFW/glfw3.h>
#include "shaderprogram.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


float vertices[] = {
    // Yakın üçgen — yeşil
    -0.3f, -0.3f, -0.5f,  // Sol alt
     0.0f, -0.3f, -0.5f,  // Sağ alt
     0.0f,  0.3f, -0.5f,  // Üst

    // Uzak üçgen  — kırmızı
    -0.0f, -0.0f, -0.7f,  // Sol alt
     0.5f,  0.0f, -0.7f,  // Sağ alt
     0.0f,  0.3f, -0.7f   // Üst
};

float quadVertices[] = {
    // pos      // tex
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    
    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

//OpenGL nesnelerinin id değerlerini tutacak olan değişkenler
unsigned int VBO;
unsigned int VAO;

// Framebuffer oluşturur, hem renk hem de derinlik verisini texture olarak döner
GLuint createFramebuffer(GLuint& colorTexture, GLuint& depthTexture) {
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // === Renk texture'u ===
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

    // === Derinlik texture'u ===
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 640, 480, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    // === Hata kontrolü ===
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[HATA] Framebuffer tamamlanamadi!" << std::endl;
    } else {
        std::cout << "[BILGI] Framebuffer tamamlandi." << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return fbo;
}

// Sahneyi verilen FBO'ya çizer, derinlik testini etkinleştirir
void drawSceneToFBO(GLuint fbo, ShaderProgram& program, GLuint vao, glm::mat4 mvp, bool greenFirst) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, 640, 480);

    // === Derinlik ayarları ===
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);         // Yeni piksel daha yakınsa çiz
    //glClearDepthf(1.0f);          // Her kare başında tüm pikseller "sonsuz uzaklıkta"
    //glDepthRangef(0.0f, 1.0f);    // Gerçekten buffer'ı bu değerle temizle

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearDepthf(1.0f);

    glClearColor(0.0f, 0.4f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program.use();
    glUniformMatrix4fv(glGetUniformLocation(program.getId(), "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    glBindVertexArray(vao);

    if (greenFirst) {
        // Yeşil - Kırmızı
        glUniform4f(glGetUniformLocation(program.getId(), "uColor"), 0.0f, 1.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUniform4f(glGetUniformLocation(program.getId(), "uColor"), 1.0f, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 3, 3);
    } else {
        // Kırmızı - Yeşil
        glUniform4f(glGetUniformLocation(program.getId(), "uColor"), 1.0f, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 3, 3);
        glUniform4f(glGetUniformLocation(program.getId(), "uColor"), 0.0f, 1.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderSplitScreen(GLuint color1, GLuint depth1, GLuint color2, GLuint depth2, ShaderProgram& shader, GLuint quadVAO) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // default framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();

    // İlk sahne (sol yarı)
    glViewport(0, 0, 640, 480);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color1);
    glUniform1i(glGetUniformLocation(shader.getId(), "color1"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depth1);
    glUniform1i(glGetUniformLocation(shader.getId(), "depth1"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, color2);
    glUniform1i(glGetUniformLocation(shader.getId(), "color2"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depth2);
    glUniform1i(glGetUniformLocation(shader.getId(), "depth2"), 3);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // İkinci sahne (sağ yarı)
    glViewport(320, 0, 640, 480); // ekranın sağ yarısı

    glDrawArrays(GL_TRIANGLES, 6, 6); // Aynı sahneyi bir kez daha çiziyoruz
}

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

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = model;

    // FBO oluştur
    GLuint color1, depth1, color2, depth2;
    GLuint fbo1 = createFramebuffer(color1, depth1);
    GLuint fbo2 = createFramebuffer(color2, depth2);

    // Her iki sahneyi FBO'lara çiz
    drawSceneToFBO(fbo1, program, VAO, mvp, true);  // Yeşil -> Kırmızı
    drawSceneToFBO(fbo2, program, VAO, mvp, false); // Kırmızı -> Yeşil

    ////////////////////////////////////////////////////////////////////////////
    /*          MERGE           */

    ShaderProgram depthMergeShader;
    depthMergeShader.attachShader("../shaders/depth_merge.vs", GL_VERTEX_SHADER);
    depthMergeShader.attachShader("../shaders/depth_merge.fs", GL_FRAGMENT_SHADER);
    depthMergeShader.link();

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    
    while (!glfwWindowShouldClose(window)) {
        // 1. input kontrolü (isteğe bağlı)
        glfwPollEvents();

        // 2. sahne işlemi (sadece combine sonucu gösterilecekse tekrar tekrar combine etmene gerek yok)
        renderSplitScreen(color1, depth1, color2, depth2, depthMergeShader, quadVAO);

        // 3. buffer swap
        glfwSwapBuffers(window);
    }

    // Temizle
    glDeleteFramebuffers(1, &fbo1);
    glDeleteFramebuffers(1, &fbo2);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}