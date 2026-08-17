# Kilitli Kapı ve Anahtar Döngüsü testi

## Anahtar Data Asset'i oluştur

1. `Content/Items` klasöründe sağ tıkla ve `Miscellaneous > Data Asset` seç.
2. `Survivor Item Definition` sınıfını seç.
3. Varlığın adını `DA_TestMoonKey` yap.
4. Şu geçici test değerlerini gir:
   - `Item Id`: `test_moon_key`
   - `Display Name`: `Ay Anahtarı`
   - `Description`: `Soğuk metalden yapılmış eski bir anahtar.`
   - `Inspection Discovery`: `Sapında silik bir ay işareti bulunuyor.`
   - `Noise Class`: `Metal`
   - `Matching Lock Symbol`: `moon`
   - `Lock Symbol Display Name`: `Ay`
   - `Max Stack Size`: `1`

## Anahtarı haritaya koy

1. Bir `World Item Pickup` aktörü yerleştir.
2. `Pickup > Item Definition` alanına `DA_TestMoonKey` ata.
3. Karakterin ulaşabileceği bir noktaya koy.

## Kilitli kapıyı yerleştir

1. Place Actors panelinde `Locked Door` ara ve haritaya sürükle.
2. Zemine oturt; mavi odak ışığının karakterin yaklaşacağı tarafa baktığından
   emin ol. Gerekirse aktörü Z ekseninde döndür.
3. `Door > Lock` altında şu değerleri ayarla:
   - `Required Key`: `DA_TestMoonKey`
   - `Starts Locked`: işaretli
   - `Lock Symbol`: `moon`
   - `Lock Symbol Display Name`: `Ay`
4. Kapının iki yanında yürüyebilecek kadar boşluk bırak ve haritayı kaydet.

## Önerilen test sırası

1. Anahtarı almadan kapıya yaklaş. `[E] Ay işaretli kapıyı incele` görünmeli.
2. `E` bas. `Kapı kilitli. Üzerinde Ay işareti var.` mesajı çıkmalı.
3. Anahtarı al ve `I` ile envanteri aç.
4. Anahtarı seçip `F` ile incele. Hem gizli metin hem de
   `HAFIZA BAĞLANTISI` görünmeli.
5. Envanteri kapatıp kapıya dön ve `E` bas. Kapı anahtarla açılıp fiziksel
   olarak karakterden uzağa doğru dönmeli.
6. Haritada bu anahtarla açılan başka kilitli kapı yoksa ekranda
   `Bu anahtara artık ihtiyacım yok.` mesajı görünmeli.
7. Envanteri aç. Anahtarın altında `[R] ARTIK GEREKMİYOR` yazmalı.
8. `R` bas. Anahtar yalnızca oyuncunun seçimiyle çantadan çıkmalıdır.

## Birden fazla kapı testi

Aynı `Required Key` değerine sahip ikinci bir `Locked Door` yerleştir. İlk
kapıyı açtıktan sonra anahtar gereksiz sayılmamalı. İkinci ve son kapı
açıldığında gereksiz duruma geçmelidir. Kapıların açılma sırası sonucu
değiştirmemelidir.

## Gamepad

- Dünyada etkileşim: alt yüz düğmesi
- Envanterde gereksiz anahtarı at: üst yüz düğmesi

Geçici kapı küpleri yalnızca mekanik testi içindir. Nihai kapı modeli, açılma
sesi ve materyalleri oyunun mekânı belirlendiğinde değiştirilecektir.
