# Etkileşim Sistemi - İlk Test

Bu sistem kameraya göre değil, karakterin baktığı yöne göre çalışır. Böylece oda
kamerası hareket ederken veya başka bir kameraya geçerken etkileşim yönü değişmez.

## Kontroller

- Klavye: `E`
- Gamepad: Alt yüz tuşu (`A` / `Cross`)

## Test nesnesini haritaya ekleme

1. `L_CameraTest` haritasını açın.
2. `Place Actors` panelinde `Interaction Test Actor` aratın.
3. Aktörü zeminin üstüne ve `Player Start` noktasının yaklaşık 100-150 cm önüne
   yerleştirin.
4. `Play` düğmesine basın.
5. Karakteri kutuya doğru çevirip yaklaşın.
6. Ekranın alt kısmında `[E] Test nesnesini incele` yazısı görünmelidir.
7. `E` tuşuna bastığınızda kutunun ışığı yeşile dönmeli ve ekranda etkileşimin
   başarılı olduğunu belirten mesaj görünmelidir.

Yazı görünmüyorsa karakterin doğrudan nesneye baktığını ve aradaki mesafenin 160
cm'den kısa olduğunu kontrol edin. Algılama çizgisi kamera yönünü değil,
karakterin ileri yönünü kullanır.

`Interaction Test Actor` yalnızca teknik doğrulama içindir. Gerçek kapı, eşya,
inceleme ve bulmaca davranışları daha sonra aynı `SurvivorInteractable`
interface'i üzerinden eklenecektir.
