from PIL import Image
import matplotlib.pyplot as plt
import numpy as np

# .ppm dosyasını aç
img = Image.open("src/fbo1_output.ppm")
img2 = Image.open("src/fbo2_output.ppm")


# Görseli göster
plt.imshow(img)
plt.title("FBO1 Output")
plt.axis("off")
plt.show()

# Görseli göster
plt.imshow(img2)
plt.title("FBO2 Output")
plt.axis("off")
plt.show()
