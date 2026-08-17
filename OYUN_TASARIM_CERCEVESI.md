# Oyun Tasarım Çerçevesi (Yaşayan Belge)

Bu belge kesinleşmiş bir tasarım sözleşmesi değildir. Oyun geliştikçe maddeler eklenebilir,
değiştirilebilir veya tamamen çıkarılabilir. Kod tarafındaki sistemler de bu nedenle mümkün
olduğunca birbirinden bağımsız ve ayarlanabilir kurulacaktır.

## Şimdilik benimsediğimiz ana fikirler

- Oyunun omurgası **risk–ödül** olacak.
- Kaynaklar kıt olacağı için her düşmanı öldürmek doğru çözüm olmayacak.
- İyi oynayan, çevresini gözlemleyen ve harita rotalarını hatırlayan oyuncu daha güvenli ve
  ekonomik ilerleyecek.
- Çanta düzeni yalnızca görsel bir envanter bulmacası değil; çıkarılan ses yoluyla düşman
  davranışını etkileyen bir risk olacak.
- İlk mekân fikri, *Resident Evil 1 Remake* hissinde büyük ve birbirine bağlanan rotaları olan
  bir malikâne. Bu mekân ve oyunun korku tonu henüz kesinleşmedi.

## Silahlar (geçici karar)

1. **Magnum:** Çok kıt mermili, öldürücü silah. Vuruşu güçlü bir ses, görsel tepki ve düşman
   tepkisiyle tatmin edici hissettirmeli. Bu sunum katmanı silah sistemi kurulurken ayrıca
   tasarlanacak.
2. **Geçici bayıltıcı / sessiz araç:** Düşmanı bir süre etkisiz bırakacak, öldürmeyecek ikinci
   seçenek. Kullanıcının söylediği “susturucu” kelimesinin tam karşılığı ve çalışma biçimi
   yaratıcı aşamada netleştirilecek.

Silah adları, sayıları, etkileri ve dengeleri değiştirilebilir. Sağlık/hasar altyapısı herhangi
bir silaha doğrudan bağımlı olmayacaktır.

## Kayıt sistemi (geçici karar)

Kayıt almak ücretsiz olmayacak. Şimdilik düşünülen bedel:

- 1 Magnum mermisi **veya**
- 2 sağlık eşyası.

Bu, oyuncuyu “şimdi güvenceye mi alayım, ilerideki tehlike için kaynağı mı saklayayım?”
kararıyla karşılaştıracak. Hikâye içindeki açıklaması, kayıt noktalarının biçimi ve kesin
bedelleri daha sonra kararlaştırılacak.

## İlk düşman prototipi: Dinleyici

Dinleyici görsel tasarımı kesinleşmiş bir yaratık değil, oynanış fikrini sınayan sökülebilir
bir prototiptir.

- Oyuncu hareketsizken hareket sesi çıkarmaz.
- Yürüme sesi, koşma sesinden daha güvenlidir.
- Çantanın ses puanı hareket sesini artırır.
- Duyduğu son konumu araştırır; oyuncuyu çok yakında fark ederse kovalar.
- Yakın saldırısı şimdilik 25 hasar verir; tam sağlıktaki oyuncu dört saldırıda ölür.
- Haritadaki bütün Dinleyici aktörleri silindiğinde envanter, karakter, kapılar, kamera ve
  sağlık sistemi çalışmaya devam eder.

Hız, işitme mesafesi, yakın algılama, saldırı hasarı, saldırı aralığı ve arama süresi aktörün
Details panelinden değiştirilebilir. Model, animasyon, kesin sesler ve yaratık hikâyesi daha
sonra birlikte kararlaştırılacaktır.

## Teknik değiştirilebilirlik kuralları

- Sağlık/hasar, ses üretimi, düşman kararları ve görsel/sesli sunum ayrı katmanlardır.
- Oyun kuralları mümkün olduğunca C++ içindeki sabit sayılara gömülmez; Blueprint/Details
  üzerinden ayarlanabilir.
- Geçici test modelleri ve renkler nihai sanat kararı sayılmaz.
- Yeni bir fikir mevcut sistemi zorunlu kılmaz: beğenilmeyen prototip haritadan veya projeden
  diğer temel sistemleri bozmadan çıkarılabilmelidir.
