# Tank Kontrolü - İlk Oynanabilir Sürüm

Bu aşamada proje, klasik Resident Evil tarzı temel tank kontrolüne sahiptir:

- `W` / `Yukarı Ok`: Karakterin baktığı yöne ilerler.
- `S` / `Aşağı Ok`: Karakterin baktığı yönün tersine, daha yavaş geri gider.
- `A` / `Sol Ok`: Karakteri olduğu yerde sola döndürür.
- `D` / `Sağ Ok`: Karakteri olduğu yerde sağa döndürür.
- `Shift` + `W` / `Yukarı Ok`: Karakterin baktığı yöne koşar.
- Gamepad sol çubuğu da aynı davranışı verir.
- Gamepad sol omuz tuşu basılıyken ileri hareket koşuya dönüşür.

Sağa ve sola strafe özellikle yoktur. Karakterdeki basit blok gövde yalnızca
gerçek model ve animasyonlar eklenene kadar kullanılacak geçici test parçasıdır.

## Unreal Editor'da test

1. Unreal Editor açıksa kapatın. C++ sınıf yapısı değiştiği için bu ilk derlemeyi
   Editor kapalıyken yapmak daha güvenlidir.
2. `SurvivorHorrorCPP.uproject` dosyasını açın.
3. Haritada bir `Player Start` olduğundan emin olun. Yoksa `Place Actors`
   panelinde `Player Start` aratıp zeminin biraz üstüne bırakın.
4. Üst araç çubuğundaki `Play` düğmesine basın.
5. Yukarıdaki tuşlarla hareketi deneyin.

Eğer karakter yerine serbest uçan Editor kamerası açılırsa, `World Settings >
GameMode Override` değerini `None` (proje varsayılanını kullanır) ya da
`SurvivorHorrorGameMode` yapın. Bir sonraki aşamada klasik sabit açılı kamera
bölgelerini ve kamera geçişlerini kuracağız.

## Ayarlanabilir başlangıç değerleri

`ClassicTankCharacter` sınıfından oluşturulacak bir Blueprint üzerinde şu
değerler kod yazmadan değiştirilebilir:

- `Forward Speed`: 180 cm/s
- `Run Speed`: 320 cm/s
- `Backward Speed Multiplier`: 0.65
- `Turn Rate`: 95 derece/saniye

İlk değerler bilinçli olarak modern aksiyon oyunlarından daha ağır seçildi.
Animasyonlar eklendiğinde hızları animasyon adımlarıyla birlikte tekrar
ayarlayacağız.
