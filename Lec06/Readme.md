drawSceneToFBO(...) fonksiyonuna glEnable(GL_DEPTH_TEST) ve gerekli ayarları ekledim. Artık sahneler çizilirken GPU, Z eksenine göre derinlik karşılaştırması yapacak 

depth_merge.fs
vTexCoord: Fullscreen quad üzerindeki texture koordinatları
depthX texture'ları: GL_DEPTH_COMPONENT formatında, değerler 0.0 (yakın) ile 1.0 (uzak) arasında olur
colorX: Normal RGB renk texture’ları

dept_merge.vs
aPos: (-1, -1) ile (1, 1) arasında ekranı kaplayan pozisyonlar
aTexCoord: (0,0) ile (1,1) arasında texture koordinatları
vTexCoord: Fragment shader’a geçecek