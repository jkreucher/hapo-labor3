# Labor Versuch 3: kombinierte Anzeige‐, Tasten‐ und Schnittstellenfunktionen

HAPO Labor bei Prof. Dr.‐Ing. Jens Altenburg an der TH-Bingen

## Aufgabenstellung:

Im Versuch 3 werden mehrere Funktionseinheiten in einer Aufgabe zusammengeführt. Mit dem Potentiometer auf dem Versuchsboard wird ein analoges Eingangssignal bereitgestellt. Im ersten Teil der Versuchsaufgabe wird die Eingangsspannung als Zahlenwert zwischen 0.00 und 3.30 (Volt) in der 7‐Segmentanzeige dargestellt.

Im zweiten Schritt wird eine Schwellspannung (Triggerwert) programmiert, bei deren Überschreitung der Signalton ertönt. Die Schwellspannung wird durch die Taster S2 und S3 variiert. Der Taster S2 erhöht die Schwellspannung in Schritten von 10 mV bei jeder kurzen Betätigung. Bei einem langen Tastendruck erfolgt eine automatische Erhöhung der Spannung um 100 mV alle 100 Millisekunden solange die Taste gedrückt bleibt.

Die Tastenfunktion von S3 funktioniert analog, nur werden jetzt die Werte verringert. Wird der Minimal‐ bzw. Maximalwert erreicht, ändert sich die Anzeige resp. der Schwellwert nicht mehr. Mit der Taste S1 wird der Einstellung aktiviert. Ein kurzer Tastendruck wechselt in den Einstellmode. Während der Einstellung blinkt der Dezimalpunkt mit einer Frequenz von einem Hertz. Ein erneuter kurzer Tastendruck beendet den Einstellmode.

Wenn ein Schwellwertvergleich mit Signalausgabe aktiviert ist, wird ein „C“ im linken Digit der 7‐Segmentanzeige eingeblendet, z.B. [C1.23] für einen aktivierten Vergleich von Schwellspannung und Analogeingangswert. Ein langer Tastendruck auf S1 schaltet den Vergleich ein bzw. aus.

Im letzten Schritt der Versuchsaufgabe wird die Einstellung der Schwellwerte bzw. deren Aktivierung/Deaktivierung über die serielle Schnittstelle kontrolliert. Die UART des Mikrocontrollers wird über ein Terminalprogramm mit dem PC verbunden und mit den Tasten [a], [+] und [‐] werden die Schwellwerte variiert.

**Hinweis:** Zur Nutzung der seriellen Schnittstelle müssen Sie einen virtuellen COM‐Port Treiber der Fa. STMicroelectronics installieren. Auf den Laborrechner an der TH ist dies erfolgt, d.h. hier können Sie sofort das Terminalprogramm benutzen. Auf Ihren privaten Laptops ist dieser Treiber zu installieren, damit Sie das Terminal verwenden können. Wenn Sie Ihre Software entsprechend vorbereiten, ist eine Treiberinstallation auf Ihren privaten Laptops nicht nötig, Sie testen dann im Labor an der Hochschule.


## Bewertung:

Das Testat wird bei erfolgreicher Präsentation der Software im Labor erteilt. Jede Laborgruppe sendet mir Ihre main.cpp per Email bis 12.00 Uhr des Vortages des Labortermins zu. **Ohne Softwarezusendung kein Testat!**
