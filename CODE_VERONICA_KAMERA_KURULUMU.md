# Code Veronica Tarzı Hareketli Oda Kamerası

Bu sistemde kontrol yönleri kameraya göre değişmez. Karakter her zaman kendi
baktığı yöne ilerlediği için bir kamera geçişi sırasında bile tank kontrolü aynı
şekilde çalışmaya devam eder.

Her `CodeVeronicaCameraZone` aktörü üç temel parçadan oluşur:

- `TriggerVolume`: Kameranın etkin olduğu görünmez oda alanı.
- `CameraRail`: Kameranın hareket edebileceği düzenlenebilir çizgi.
- `TrackingCamera`: Ray üzerinde hareket edip karaktere bakan gerçek kamera.

## İlk test odasını hazırlama

1. Unreal Editor'da `File > New Level > Basic` ile basit bir seviye açın.
2. Seviyeyi `Content/Maps/L_CameraTest` adıyla kaydedin.
3. Zeminin biraz üstüne bir `Player Start` yerleştirin.
4. `Place Actors` panelinde `Code Veronica Camera Zone` aratıp seviyeye bırakın.
5. Aktörü seçip `TriggerVolume` bileşeninin `Box Extent` değerlerini odanın
   tamamını kapsayacak şekilde büyütün. Aktörün tamamını ölçeklemeyin; bu işlem
   kamera rayını da istenmeden ölçekler.
6. `CameraRail` bileşenini seçin. İki spline noktasını viewport içinde taşıyarak
   kameranın odanın hangi hattında ilerleyeceğini belirleyin.
7. `Player Start` noktasının TriggerVolume içinde kaldığından emin olun.
8. `Play` düğmesine basın. Karakter ray doğrultusunda ilerledikçe kamera da ray
   üzerinde kaymalı ve karakteri kadrajda tutmalıdır.

Oyun birinci şahıs görünümünde başlarsa `Player Start` aktörünün tamamının
`TriggerVolume` kutusu içinde kaldığını kontrol edin. `World Settings > GameMode
Override` değeri `None` veya `SurvivorHorrorGameMode` olmalıdır. `Simulate`
yerine normal `Play` kullanın.

## Başlangıç ayarları

Aktörü seçtiğinizde `Camera Zone` kategorisinde şu değerleri göreceksiniz:

- `Position Lag Speed = 3`: Kamera konumunun karakteri yakalama hızı.
- `Rotation Lag Speed = 6`: Kameranın karaktere dönme hızı.
- `Camera Transition Time = 0.25`: Odaya girerken yapılan yumuşak geçiş.
- `Camera Field Of View = 50`: Daha dar, eski tarz sinematik görüş açısı.
- `Look At Offset Z = 60`: Kameranın karakterin ayakları yerine gövdesine bakması.

`Camera Transition Time` değerini `0` yaparsanız klasik anlık kamera kesmesi elde
edersiniz. Birden fazla oda kamerasında TriggerVolume alanlarını çok az üst üste
bindirmek, kapı eşiğinde kamerasız kalınmasını önler.

Bu ilk teknik prototiptir. Kamera açıları, kadraj dili, geçişlerin sert veya
yumuşak oluşu ve hangi odalarda kameranın hareket edeceği yaratıcı tasarım
aşamasında birlikte belirlenecektir.
