# Sağlık Eşyası Kurulumu ve Testi

Sağlık eşyasının adı, görünüşü ve kesin denge değeri henüz yaratıcı karar değildir. Bu testte
yalnızca ayarlanabilir tüketim sisteminin doğru çalıştığını doğrulayacağız.

## 1. Test sağlık eşyasını oluştur

1. Unreal'ı aç ve Content Browser'da `Content/Items` klasörüne gir.
2. `DA_TestItem` üzerine sağ tıkla ve **Duplicate** seç.
3. Yeni dosyanın adını `DA_TestHealth` yap ve aç.
4. Aşağıdaki geçici değerleri gir:

   - `Item Id`: `test_health`
   - `Display Name`: `Test Sağlık Eşyası`
   - `Description`: `Yaraları geçici olarak tedavi eden test malzemesi.`
   - `Noise Class`: `Soft / Yumuşak`
   - `Max Stack Size`: `2`
   - `Use Effect`: `Sağlık Yenile`
   - `Health Restore Amount`: `50`

5. Data Asset'i kaydet.

## 2. Haritaya yerleştir

1. `L_CameraTest` haritasını aç.
2. Place Actors aramasından `WorldItemPickup` aktörünü zemine sürükle.
3. Aktörü seçip Details panelindeki `Pickup > Item Definition` alanına `DA_TestHealth` ata.
4. `Quantity` değerini `2` yap ve haritayı kaydet.

## 3. Oyun testi

1. Play'e bas ve iki sağlık eşyasını al.
2. Canın tamken envanteri aç, eşyayı seç ve **Boşluk** tuşuna bas.
   - “Sağlığın zaten iyi” mesajı görünmeli.
   - Eşya tüketilmemeli.
3. Envanteri kapat ve Dinleyici'den iki veya üç saldırı al.
4. Düşmandan biraz uzaklaşıp envanteri aç. Oyun envanterde durakladığı için saldırı devam
   etmez.
5. Sağlık eşyasını seç ve **Boşluk** tuşuna bas.
   - Bir eşya eksilmeli.
   - Sağlık durumu iyileşmeli.
   - `Test Sağlık Eşyası kullanıldı.` mesajı görünmeli.
6. Eşyayı tekrar kullan. Sağlık 100'ü geçmemeli ve yalnızca gerçekten kullanılırsa tüketilmeli.
7. Ay anahtarı gibi `Use Effect = Kullanılamaz` olan bir eşyada Boşluk'a basarsan eşya
   tüketilmeden “Bu eşya şu anda kullanılamaz” mesajı görünmeli.

## Kontroller

- `I` veya `Tab`: Envanteri aç/kapat.
- `WASD` veya oklar: Yuva seç.
- `Boşluk`: Seçili eşyayı kullan.
- Gamepad `Right Trigger`: Seçili eşyayı kullan.

Test bittikten sonra `Health Restore Amount`, yığın sayısı ve eşyanın çanta sesi istediğimiz
gibi değiştirilebilir. `DA_TestHealth` yalnızca test varlığıdır; nihai oyun eşyası değildir.
