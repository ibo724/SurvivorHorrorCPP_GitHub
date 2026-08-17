# Envanter ve eşya altyapısı

Bu aşamada oyunun nihai envanter ekranı, slot sayısı ve eşya kategorileri
seçilmedi. Sistem varsayılan olarak sınırsız slotla çalışır; kapasite daha sonra
tek bir ayarla etkinleştirilebilir.

## Test eşyası oluşturma

1. Unreal Editor'ı aç ve derleme tamamlanınca Content Browser'a git.
2. `Content` altında `Items` adlı bir klasör oluştur.
3. Klasörün içinde sağ tıkla, `Miscellaneous > Data Asset` seç.
4. Sınıf listesinden `SurvivorItemDefinition` seç.
5. Varlığı `DA_TestItem` adıyla kaydet.
6. Ayrıntılar panelinde şu geçici değerleri gir:
   - `Item Id`: `test_item`
   - `Display Name`: `Test Eşyası`
   - `Description`: `Envanter altyapısını sınamak için kullanılır.`
   - `Max Stack Size`: `1`
7. `Place Actors` panelinde `World Item Pickup` ara ve haritaya sürükle.
8. Aktörü seçip `Pickup > Item Definition` alanına `DA_TestItem` varlığını ata.
9. Eşyayı zeminin biraz üstüne yerleştir ve haritayı kaydet.

## Beklenen sonuç

- Karakter eşyaya dönüp yaklaşınca `[E] Test Eşyası al` görünür.
- `E` tuşuna basınca eşya kaybolur.
- Ekranda `Test Eşyası alındı.` bildirimi belirir.
- Output Log'da `Pickup collected: Test Eşyası x1` kaydı oluşur.

Envanter şimdilik arka planda tutulur. Görsel menü, kapasite ve eşya kullanım
kuralları yaratıcı yönü belirledikten sonra bu altyapının üstüne eklenecek.
