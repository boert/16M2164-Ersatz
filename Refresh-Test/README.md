# Refreshtest

## Parameter

default:
Schacht: 0C
Steuerwort: 1F

Das Programm arbeitet im Bereich 4000h bis 5FFFh

RAM4 wird ausgeschaltet
das Speichermodul auf dem entprechenden Schacht eingeschaltet

Der Speicher wird mit einem Muster gefüllt und über dieses Muster wird eine einfache Prüfsumme gebildet (SUM16).
Die Prüfsumme dient als Referenz für den weiteren Programmlauf.
Der Speicherinhalt wird in den Bildspeicher kopiert und angezeigt.

Dann wird eine bestimmte Zeit gewartet (beginnend mit 20 ms).
Der Speicher wird wieder zur Anzeige gebracht und es wird die Prüfsumme gebildet.
Wenn die Prüfsumme stimmt, wird die Wartezeit erhöht und das Warten beginnt erneut.

Andernfalls erfolgt die Ausschrift 'FAIL' und das Programm wird beendet.
