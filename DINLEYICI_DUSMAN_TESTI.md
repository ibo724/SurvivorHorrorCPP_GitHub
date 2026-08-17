# Dinleyici Düşmanını Kurma ve Test Etme

Bu prototipte nihai düşman modeli veya animasyonu yoktur. Renkli küp, ışık ve üstündeki durum
yazısı yalnızca davranışı kolayca okuyabilmek içindir.

## 1. Test haritasını aç

1. `SurvivorHorrorCPP.uproject` dosyasını aç.
2. Content Browser içinden `Content/Maps/L_CameraTest` haritasını aç.

## 2. Yürünebilir alanı oluştur

Dinleyici hedefe gidebilmek için Unreal'ın Navigation Mesh sistemini kullanır.

1. Sol üstte **Place Actors** panelini aç. Görünmüyorsa `Window > Place Actors` seç.
2. Arama alanına `Nav Mesh Bounds Volume` yaz.
3. Çıkan aktörü test odasının zeminine sürükle.
4. Volume seçiliyken **R** ile Scale aracına geç ve hacmi testte yürünecek bütün zemini
   kaplayacak şekilde büyüt. Hacmin alt ve üst sınırı zemini ve karakterleri içine almalı.
5. Klavyeden **P** tuşuna bas. Yürünebilir zeminin yeşil görünmesi gerekir. Tekrar P ile bu
   görünümü kapatabilirsin.

Yeşil alan yoksa Dinleyici sesi duyabilir ve renk değiştirebilir ama hedefe yürüyemez.

## 3. Dinleyici'yi yerleştir

1. Place Actors aramasına `ListenerEnemy` yaz.
2. C++ sınıfını yeşil zeminin üzerine sürükle.
3. İlk karşılaştırma için düşmanı oyuncunun başlangıç rotasından yaklaşık 600–800 Unreal
   birimi (6–8 metre) uzağa koy.
4. `Ctrl + Shift + S` ile her şeyi kaydet.

Dinleyici seçiliyken Details panelindeki `Listener Enemy` başlıklarından şu değerler bağımsız
olarak değiştirilebilir:

- `Hearing Distance`: İşitme mesafesi.
- `Close Detection Distance`: Çok yakındaki oyuncuyu doğrudan fark etme mesafesi.
- `Investigate Speed` / `Chase Speed`: Araştırma ve kovalama hızları.
- `Search Duration`: Son ses konumunda ne kadar arayacağı.
- `Attack Damage`: Şimdilik 25; dört saldırı tam canlı oyuncuyu öldürür.
- `Attack Cooldown`: İki saldırı arasındaki bekleme.
- `Show Debug State`: Geçici durum yazısını açar veya kapatır.

## 4. Davranış testi

Play'e bas ve sırayla dene:

1. **Hareketsiz kal:** Çanta gürültülü olsa bile hareket sesi üretilmez. Dinleyici yeşil ve
   `SESSIZ` durumda kalmalı.
2. **Yürü:** Boş/sessiz çantayla varsayılan etkili işitme mesafesi yaklaşık 360 birimdir.
   Yeterince uzaktaysan düşman tepki vermemeli.
3. **Koş:** Varsayılan etkili mesafe yaklaşık 1080 birime çıkar. Dinleyici sarı
   `DINLIYOR / ARASTIRIYOR` durumuna geçip son ses konumuna yürümeli.
4. **Ses çıkardıktan sonra dur veya köşe dön:** Düşman oyuncunun gerçek konumunu duvarın
   arkasından sihirli biçimde bilmez; son duyduğu konumu arar, sonra mavi `ARIYOR` durumuna
   geçer ve süre dolunca sakinleşir.
5. **Çantaya metal/sert test eşyaları al:** Aynı yürüme hızında daha uzaktan duyulmalısın.
   Envanterde eşyaları ayırarak çanta sesini azaltmak bu riski yeniden düşürür.
6. **Çok yaklaş:** Düşman kırmızı `KOVALIYOR`, saldırı anında beyaz `SALDIRIYOR` olur.
7. Dört saldırıdan sonra sağlık sırasıyla `İYİ`, `DİKKAT`, `TEHLİKE` durumlarından geçer ve
   ölüm ekranı açılır. **R** veya **Enter** ile harita baştan yüklenir.

## 5. Bir sorun olursa

- **Renk değişiyor ama yürümüyor:** P ile yeşil NavMesh'i kontrol et. Dinleyici ve hedef
  konumu yeşil alanın üzerinde olmalı.
- **Hiç duymuyor:** Önce koşarak yaklaş; düşmanın `Hearing Distance` değerini kontrol et.
- **Kamera yerine karakter görünümüne geçiyor:** Test karakterinin doğru kamera zone içinde
  başladığını kontrol et; düşman kamera sistemini değiştirmez.
- **Bu düşmanı oyundan çıkarmak istiyorum:** Haritadaki `ListenerEnemy` aktörünü seçip Delete'e
  bas. Diğer oyun sistemleri ona bağlı değildir.
