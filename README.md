Mit dem TTGO LORA§" Oled Board habe ich eine Lora Sender und einen Lora Empfänger programmiert.
Der Lora Empfänger hat zusätzlich noch eine SD Card in dem die empfangen Nachrichten auf die SD Karte geschrieben werden.

Der Support für die SD Karte war etwas mühsam aber am Ende hat es funktioniert.

Sowohl das LoRa Radio Modul als auch das SD Card Modul werden über das SPI Interface angesteuert.

In diesem Sketch ist es so implementiert, dass das Lora Modul bereits auf dem Development fest verdrahtet ist und folgende Pins verwendet![image](https://github.com/user-attachments/assets/833cf529-a425-43a1-b699-f58930ed0492)
Das SD Card Modul ist nicht fest verdrahtet sondern muss noch an das TTGO dev. board angeschlossen werden.

Dazu habe ich für alle PINs des SD Card Moduls separate GPIOs verwendet

Beim SPI Bus ist es nun so, dass nicht gleichzeitig mit beiden Devices kommuniziert werden kann. Die Steuerung welches Modul gerade verwendet werden soll, geschieht über die CS Leitung.

#Da die Verwendung der 'standard SD card Lib' Schwierigkeiten bereitet hat, habe ich die 

mySD.h und mySD.cpp sowie die File.cpp libs separate in das Projekt installiert.
-----
Auch die Pin Defintionen Sd2PinMapa.h habe ich noch hinzugefügt![image](https://github.com/user-attachments/assets/d49f9255-b106-4edf-b1bd-28404c605da4)


