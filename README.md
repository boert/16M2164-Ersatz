# Ersatz für 16M2164

## Situation
Der Hybridschaltkreis 16M2164 von KWH (Keramische Werke Hermsdorf) scheint nur in sehr geringen Stückzahlen hergestellt worden zu sein. Er enthält 16 mal das Die (ein Stück vom Halbleiterwafer) des Schaltkreises U2164 sowie einige Stützkondensatoren auf einem Keramiksubstrat.
Der U2164 ist ein dynamischer RAM mit 64kBit.
In Summe enthält der 16M2164 ein Megabit bzw. 128 kByte Speicher.

Zum Einzigen - mir bekannten Einsatz - kommt der Schaltkreis 16M2164 im Modul M036 für den KC85 aus Mühlhausen [^1]. 
Da einige Leiterplatte für dieses Modul im Umlauf sind, besteht seit geraumer Zeit der Wunsch einen Ersatz für den 16M2164 zu schaffen.

Es gibt seit einigen Jahren eine Schaltkreisersatzplatine, die mit vier dRAMs in der Konfiguration 64k x 4 auskommt [^2].

---
## dynamische Speicher

Bei dynamischen Speichern wird die Information als Ladung in Kondensatoren abgebildet. Durch Leckströme verliert sich diese Ladung mit der Zeit und muß daher regelmäßig aufgefrischt werden (Refresh). Dies geschiet u.a. beim Aktivieren des /RAS-Signals. Der Hersteller gibt dabei an, in welchem Zeitraum jede ROW-Adresse einmal aktiviert werden muß. Normalerweise wird die angelegte ROW-Adresse vom Speichercontroller entsprechend hochgezählt. Bei einem geschickt adressierten Bildwiederholspeicher könnte man auf den Refresh verzichten, da dort durch regelmäßige Lesezugriffe die Information beim Lesen aufgefrischt wird.
---

---
## dRAM-Größen und Refresh

Speicher-IC | Speichergröße | Refreshzyklen | Refreshzeit | Gehäuse
----------- | ------------: | ------------: | ----------: | -------
MK4096      |   4k x 1      |   64          |  2 ms       | DIP16
MK4116      |  16k x 1      |  128          |  2 ms       | DIP16
µPD4164     |  64k x 1      |  128          |  2 ms       | DIP16
MK4564      |  64k x 1      |  128          |  2 ms       | DIP16, PLCC18
MK4164      |  64k x 1      |  256          |  4 ms       | DIP16
HYB4164     |  64k x 1      |  256          |  4 ms       | DIP16
MK41256     | 256k x 1      |  256          |  4 ms       | DIP16, PLCC18
HYB51100    |   1M x 1      |  512          |  8 ms       | SOJ26/20
            |               |               |             | 
4416        |  16k x 4      |  256          |  4 ms       | DIP18, PLCC18
4464        |  64k x 4      |  256          |  4 ms       | DIP18, PLCC18
µPD41464    |  64k x 4      |  256          |  4 ms       | DIP18, PLCC18
SMJ4464     |  64k x 4      |  256          |  4 ms       | DIP18, PLCC18
M5M4464A    |  64k x 4      |  256          |  4 ms       | DIP18, PLCC18, ZIP20
KM41464A    |  64k x 4      |  256          |  4 ms       | DIP18, PLCC18, ZIP20
HYB514256   | 256k x 4      |  512          |  8 ms       | DIP20, SOJ26/20
µPD424400   |   1M x 4      | 1024          | 16 ms       | TSOP26, SOJ26/20

---

Der Nachteil der bestehenden Variante: sie funktioniert leider nicht richtig.
Die U880/Z80-CPU enthält einen 7 Bit Refreshzähler, der z.B. für die Bausteine mit 64 oder 128 Refreshzyklen ausreichend ist.
Bei den hier verwendeten Speicher-IC der Größe 64k x 4 Bit wird ein 8 Bit Zähler benötigt, der bis 256 Refreshzyklen abdeckt.
Damit erhält die obere Hälfte des Speichers keinen definierten Refresh und verliert mit der Zeit ihren Inhalt.

Im Folgenden sind drei Varianten zur Lösung der Problematik aufgezeigt:

## Variante R: Refresh
Man könnte den Refreshzähler erweitern. 

Ein Schaltung dafür findet sich z.B. in [^3].

Auch im Grundgerät D001 des KC85/4 werden aus den Signalen /rfsh, mreq und ab6 die Signale RF7 und RF8 gebildet, die den Refreshzähler des Z80 auf 9 Bit erweitern:
![KC85/4, erweiterter Refresh](Bilder/KC85_RF7_RF8.jpg)
Der erweiterte Refreshzähler wird nur für interne RAM-Bausteine genutzt.

Am Modul 16M2164 steht weder der erweiterte Refreshzähler noch das Refresh-Signal zur Verfügung.
Zumindest letzteres müsste separat zugeführt werden um den Refreshzähler zu erweitern.
Ich wollte eine Lösung die mit den Signalen des 16M2164 auskommt.

## Variante D: dynamischer RAM
Im Robotrontechnik-Forum ist ein Bild vom Innenaufbau des 16M2164 verlinkt [^4]. Mit etwas Geschick lassen sich die nötigen 16 Stück U2164 huckepack auf einer Aufsteckplatine plazieren und so die Schaltung des 16M2164 1:1 nachbilden.

![M036 mit 16M2164-Ersatz aus 16 U2164](Bilder/M036_mit_dRAM.jpg){width=70% height=70%}

Großer Nachteil dieser Lösung: sie benötigt zuviel Platz und passt nicht mehr in ein Modulgehäuse.
Das M036 läßt sich so regulär nur an einem Adapter M007 betreiben.


## Variante S: statischer RAM
Warum ersetzen wir nicht den dRAM durch einen einzelnen SRAM der Größe 128kx8 Bit?
Ein SRAM benötigt die Signale /CS, /WE und /OE sowie Daten und Adressen.
Am Bestückungsplatz für das 16M2164 stehen die Signale /RAS, /CAS0 (unter Hälfte), /CAS1 (obere Hälfte) und /WE zur Verfügung. Die Adressenleitungen sind multiplext.

Es ist eine Signalanpassung nötig und die Adressen benötigen einen Demultiplexer.
Ein Latch, was mit dem invertierten /RAS-Signal angesteuert wird, speichert die eine Hälfte der Adressinformation. Die andere Hälfte wird direkt an den SRAM durchgereicht. Aus /CAS0 und /CAS1 wird das /CS-Signal gebildet. Aus dem Signal /WE wird noch das Signal /OE abgeleitet.

Da es ein RAM ist, können Datenleitungen problemlos miteinander vertauscht werden. Ebenso kann man Adressleitungen tauschen, solange keine Sonderfunktionen realisiert sind.
Damit wird das Routing auf einer zweilagigen Platine erleichtert.

Im Bild ist die Lösung zu sehen, die von der Grundfläche näher am Original 16M2164 ist.
![M036 mit 16M2164-Ersatz aus 16 U2164](Bilder/M036_mit_SRAM.jpg){width=70% height=70%}
Mit flachen Präzisions-Stiftleisten (z.B. BKL 10120536 von reichelt) läßt sich die Ersatzplatine mit im Modulgehäuse unterbringen.

# Test im M036 #

# Nutzung des Modul M036 #

## Typestar ##

## RAMDOS ##

## RAM-Disk für MicroDOS (D004) ##

### Referenzen und Links
[^1]:
Modul M036, Beschreibung, Bilder und Handbuch
http://www.mpm-kc85.de/html/m036_segram128k.htm

[^2]:
Schaltkreisersatzplatine 16M2164
https://www.sax.de/~zander/ --> Hobby --> KC85 --> Hardware --> 128K-Segmented-RAM-Modul (M036)

[^3]:
c't 1984, Heft 9 
M. Hungershausen
8-Bit-Refresh für Z80

[^4]:
Bild vom Innenleben des 16M2164
https://www.robotrontechnik.de/html/forum/thwb/showtopic.php?threadid=2115

Innenleben des U2164
https://www.richis-lab.de/RAM02.htm
