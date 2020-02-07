# Shell
C dili ile Linux ortamında geliştirilmiş bir komut satırı yorumlayıcısı

### Giriş yönlendirme
Bir dosyayı komuta yönlendirmek için giriş yönlendirme(<) kullanınız.
Alt prosesin standart girdisini verilen giriş dosyasına yönlendirir.  
Örnek:  
> cat < file.txt
    
### Çıkış yönlendirme
Bir komutu dosyaya yönlendirmek için çıkış yönlendirme(>) kullanınız.
Alt prosesin standart çıktısını dosyaya yönlendirir.  
Örnek:  
> cat file1 > file2

### Arkaplan 
Komutun arkaplanda çalıştırılması için (&) kullanınız.
Arkaplandaki işlem bittiğinde ekrana  
    [pid] retval: <exitcode> şeklinde çıktı basılır.  
Örnek:  
> sleep 5 &  
> cat file.txt  
> file.txt’nin içindekiler ekrana yazdırılır.  
> [24617] retval: 0
  
### Çoklu komut
Birden fazla komut girmek için komutları noktalı virgül(;) ile ayırınız.  
Örnek:  
> echo message > message.txt ; sleep 1 ; cat < message.txt
