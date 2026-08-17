# Çanta Sesi ve İnceleme Hafızası testi

## Test eşyasını güncelle

1. `Content/Items/DA_TestItem` varlığını aç.
2. `Item > Bag Noise > Noise Class` değerini `Metal` yap.
3. `Item > Inspection > Inspection Discovery` alanına şunu yaz:
   `Metal yüzeyin altında kazınmış küçük bir 17 işareti var.`
4. `Max Stack Size` değerini `1` olarak bırak ve kaydet.

Bu metin yalnızca sistemi sınamak içindir; gerçek eşya ve puzzle ipucunu daha
sonra oyunun mekânına göre birlikte yazacağız.

## Envanteri aç ve yönet

1. Haritada iki `World Item Pickup` bulunduğundan emin ol. Var olan aktörü
   `Alt` tuşuna basılı tutup taşıyarak çoğaltabilirsin.
2. Play'e bas ve iki eşyayı da `E` ile al.
3. `I` veya `Tab` ile envanteri aç.
4. Oyun dünyasının durduğunu ve sekiz yuvanın göründüğünü kontrol et.
5. `WASD` veya ok tuşlarıyla yuvalar arasında gezin.
6. Dolu bir yuvada `E` tuşuna bas, boş bir hedefe git ve tekrar `E` tuşuna
   bas. Eşya hedef yuvaya taşınmalıdır.
7. Seçimi iptal etmek veya envanteri kapatmak için `Esc` kullan.

## Çanta Sesi testi

- İki metal eşya yan yanayken ses puanı `7`, seviye `ORTA` olmalıdır.
- Eşyaların birini 8. yuvaya taşıdığında puan `4`, seviye `DÜŞÜK` olmalıdır.
- Boş ve yumuşak eşya yuvaları gürültülü komşuluğu keser.
- Bu puan daha sonra yürüme sesleri ve düşmanların işitme mesafesine
  bağlanacaktır.

## İnceleme Hafızası testi

1. Test eşyasını seç ve `F` tuşuna bas.
2. Alt bilgi panelinde gizli metin görünmeli ve `HAFIZAYA KAYDEDİLDİ` yazmalıdır.
3. Aynı türdeki diğer test eşyasını seçtiğinde bilgi hatırlanmaya devam eder.

Hafıza şimdilik çalışan oyun oturumu boyunca saklanır. Kalıcı kayıt SaveGame
sistemi yapıldığında eklenecektir.

## Kapasite testi

Haritaya dokuz adet `World Item Pickup` yerleştirip toplamaya çalış. İlk sekiz
eşya alınmalı; dokuzuncuda eşya yerde kalmalı ve ekranda
`Daha fazla taşıyamam.` mesajı görünmelidir.

## Gamepad

- Envanter: `Menu/Start`
- Gezinme: D-pad
- Taşı/yerleştir: alt yüz düğmesi
- İncele: sol yüz düğmesi
- İptal/kapat: sağ yüz düğmesi
