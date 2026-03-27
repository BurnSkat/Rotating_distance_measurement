# Rotating_distance_measurement



Es soll mit einer IR-Fernbedienung ein Steppermotor bewegt werden, auf dessen Achse ein Ultraschallsensor montiert ist. Dieser soll mit Tasten auf der Fernbedienung über die horizontale Achse stufenlos um 360° in beide Richtungen gedreht werden können, sodass der Abstand in [m] auf einem LCD-Display per Knopfdruck in Echtzeit dargestellt werden kann.
Für eine korrekte IR-Signalerkennung von der Fernbedienung wird eine rote LED installiert, die aufleuchtet, wenn ein IR-Signal erkannt wird.

Zusatz: Taste auf der Fernbedienung für automatische Messungen für einen kompletten 360° Umlauf. Speicherung der Messergebnisse in einen bestimmten Format auf einem SD-Kartenmodul, um die Auswertung zu erleichtern.


Wir wollen ein Gerät zur Abmessung eines Raumes entwerfen. Dabei soll ein Ultraschallsensor verwendet werden, der auf der Drehachse eines Steppermotors befestigt ist. Dieser kann an einer Stelle des Raumes abgestellt werden und die horizontale Messung soll dann per IR-Fernbedienung eingeleitet werden. 
 
Das Gerät soll per An/Aus-Schalter ein, bzw. ausgeschaltet werden. Eine LED soll dabei angeben ob das Gerät an oder aus ist.
Ziel ist es, am Ende 2 Betriebsmodi zu haben. Einmal einen automatischen Ablauf, wo der Sensor in festgelegten Abständen regelmäßige Messungen im horizontalen 360° Kreis vollführt und die Ergebnisse pro Messung im Display ausgibt, und dazu einen manuellen Modus, wo Einzelmessungen getätigt werden können, wobei Winkel und Anzahl der Messungen per IR-Fernbedienung eingestellt werden.
 
Eventuell soll sogar ausgewählt werden in wie vielen Messungen die 360° im Automatikmodus abgefahren werden sollen, quasi eine wählbare Auflösung.
 
Im Display soll die Menüanzeige angezeigt werden, also ob Automatik oder Manueller Modus gewählt wurde. Zudem sollen die Messergebnisse dort wiedergegeben werden und man soll auch per Fernbedienung durch die Liste der Messungen navigieren können.
Der Motor soll im Idealfall auch über einen Encoder verfügen damit eine Kalibrierung des Sensors möglich ist. Oder die Kalibrierung wird über einen Endschalter realisiert.
 
 
 
Einteilung nach EVA:
 
Eingabe:
Ultraschallsensor, IR Fernbedienung, An/Aus-Schalter
 
Verarbeitung: Arduino, (ggf. Encoder)
