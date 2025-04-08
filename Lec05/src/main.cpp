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

GLuint createFramebuffer(GLuint& textureColorbuffer) {
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Renk verisi için texture
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Framebuffer'a ekle
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // Derinlik buffer'ı da ekle (depth test çalışsın diye)
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 640, 480);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer eksik!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return fbo;
}

void drawSceneToFBO(GLuint fbo, ShaderProgram& program, GLuint vao, glm::mat4 mvp, bool greenFirst) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, 640, 480);

    // === Derinlik ayarları ===
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);         // Yeni piksel daha yakınsa çiz
    //glClearDepthf(1.0f);          // Her kare başında tüm pikseller "sonsuz uzaklıkta"
    //glDepthRangef(0.0f, 1.0f);    // Gerçekten buffer'ı bu değerle temizle
    
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Ekrana geri dön
}

void readFramebuffer(GLuint fbo, std::vector<unsigned char>& pixels) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    pixels.resize(640 * 480 * 3); // RGB
    glReadPixels(0, 0, 640, 480, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool compareBuffers(const std::vector<unsigned char>& a, const std::vector<unsigned char>& b) {
    return a == b;
}


void saveBufferToPPM(const std::vector<unsigned char>& pixels, const std::string& filename, int width, int height) {
    std::ofstream file(filename, std::ios::binary);
    file << "P6\n" << width << " " << height << "\n255\n";

    for (int y = height - 1; y >= 0; --y) {
        file.write((char*)&pixels[y * width * 3], width * 3);
    }

    file.close();
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

    // MVP matrisi
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 10.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -2.0f));
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;

    // FBO oluştur
    GLuint tex1, tex2;
    GLuint fbo1 = createFramebuffer(tex1);
    GLuint fbo2 = createFramebuffer(tex2);

    // Her iki sahneyi FBO'lara çiz
    drawSceneToFBO(fbo1, program, VAO, mvp, true);  // Yeşil -> Kırmızı
    drawSceneToFBO(fbo2, program, VAO, mvp, false); // Kırmızı -> Yeşil

    // Piksel verilerini oku
    std::vector<unsigned char> buffer1, buffer2;
    readFramebuffer(fbo1, buffer1);
    readFramebuffer(fbo2, buffer2);

    // Karşılaştır
    if (compareBuffers(buffer1, buffer2)) {
        std::cout << "Görüntüler AYNI!" << std::endl;
    } else {
        std::cout << "Görüntüler FARKLI!" << std::endl;
    }

    std::vector<unsigned char> fboData;
    readFramebuffer(fbo1, fboData);
    saveBufferToPPM(fboData, "fbo1_output.ppm", 640, 480);

    readFramebuffer(fbo2, fboData);
    saveBufferToPPM(fboData, "fbo2_output.ppm", 640, 480);

    // Temizle
    glDeleteFramebuffers(1, &fbo1);
    glDeleteFramebuffers(1, &fbo2);
    glDeleteTextures(1, &tex1);
    glDeleteTextures(1, &tex2);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}