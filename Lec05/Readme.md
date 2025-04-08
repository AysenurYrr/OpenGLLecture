# 🎯 OpenGL'de Derinlik Testi (Z-Buffer) Kullanımı

3B grafik çiziminde, sahnede hangi nesnenin önde ve hangisinin arkada olduğunu belirlemek için **derinlik testi (depth testing)** kullanılır.  
Bu işlem, her pikselin bir **Z-değeri** (derinlik) ile kontrol edilmesini sağlar.

---

## 🧠 Z-Buffer Nedir?

OpenGL, çizdiğin sahnedeki her pikselin Z-değerini (kamera uzaklığı) bir **depth buffer** içinde saklar.

| Z-Değeri | Anlamı           |
|----------|------------------|
| `0.0`    | Kameraya en yakın |
| `1.0`    | Kameraya en uzak  |

---

## ✅ Derinlik Testini Açmak

```cpp
glEnable(GL_DEPTH_TEST);
```

Bu satırla, OpenGL'e "çizeceğim piksellerin derinliğini kontrol et" demiş olursun.

---

## ⚙️ `glDepthFunc(...)`

Bu fonksiyon, derinlik testinin **nasıl çalışacağını** belirler.

### En yaygın kullanılan:

```cpp
glDepthFunc(GL_LESS);
```

> "Yeni pikselin Z-değeri eskiye göre **daha küçükse** (yakınsa), onu çiz."

| Fonksiyon      | Açıklama                            |
|----------------|-------------------------------------|
| `GL_LESS`      | Yeni piksel < Eski piksel (öncelikli) |
| `GL_LEQUAL`    | Yeni ≤ Eski                         |
| `GL_GREATER`   | Yeni > Eski                         |
| `GL_ALWAYS`    | Her zaman çiz                       |
| `GL_NEVER`     | Asla çizme                          |

---

## 🧼 `glClearDepthf(...)`

```cpp
glClearDepthf(1.0f);
```

Bu fonksiyon, `glClear(GL_DEPTH_BUFFER_BIT)` çağrıldığında **depth buffer hangi değerle sıfırlanacak** onu belirler.

### Açıklama:
- `1.0f`: Z-buffer başlangıçta **sonsuz uzaklıkta gibi davranır**.
- İlk çizdiğin her şey bu 1.0'dan daha yakında olacağı için çizilir.

---

## 🔁 Derinlik Temizliği

```cpp
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
```

Bu satır her karede Z-buffer ve renk buffer'ını sıfırlar.

---

## ⚠️ `glClearDepthf()` Kullanmazsam?

OpenGL’in **varsayılan değeri `1.0f`**'tır. Genelde sorun olmaz.  
Ama daha önce bir yerde `glClearDepthf(0.0f)` yazılmışsa ve değiştirilmemişse:

- Z-buffer 0.0 ile temizlenir  
- Yeni gelen piksellerin Z değeri **daha küçük olamaz** (`z ≥ 0.0`)  
- Sonuç: **Hiçbir şey çizilmez! (Siyah ekran 😱)**

---

## 🎓 Örnek Kullanım

```cpp
glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_LESS);
glClearDepthf(1.0f);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
```

Bu yapı ile sahnende her kare temizlenir ve **önce öndeki objeler görünür**.

---

## 🔚 Özet

| Ayar                     | Görev                                  |
|--------------------------|----------------------------------------|
| `glEnable(GL_DEPTH_TEST)` | Derinlik testi aktif                   |
| `glDepthFunc(GL_LESS)`    | Yakın olan çizilsin                    |
| `glClearDepthf(1.0f)`     | Z-buffer başlangıcı: uzaklık (1.0f)    |
| `glClear(...)`            | Temizleme işlemi (color + depth buffer)|

---

> ✅ Derinlik testi, 3D sahnelerde objelerin **doğru sırayla görünmesini sağlar**.  
> Bu yüzden her karede dikkatlice temizlenmeli ve doğru karşılaştırma fonksiyonu seçilmelidir.
