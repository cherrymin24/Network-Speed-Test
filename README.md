# Network-Speed-Test

Cerinte:
- [ ]Aplicatia poate fi rulata fie in modul server cu parametrul -s fie in modul client cu parametrul -c urmat de adresa ip a serverului.  
- [ ]Permiterea setari portului pe care se doreste a se face conexiunea cu parametrul -p urmat de port.  
- [ ]Daca se doreste modificarea timpului in care se trimite date, se poate folosi parametrul -t urmat de noul timp.  
- [ ]In mod normal la fiecare secunda, un calcul este efectuat pentru a afisa bandwith-ul pentru in acea secunda. Daca se doreste modificarea acestui interval, se va folosi argumentul -i urmat de un numar de secunde pentru intervalele noi.  
- [ ]Protocolul de transmitere a datelor poate fi modificat din TCP(default) in UDP cu parametrul -u.  

17.06.2024 
M-am documentat despre modul de functionare al iperf3:
https://iperf.fr/iperf-doc.php

18.06.2024
Am finalizat conexiunea client server

19.06.2024
Am adaugat un script care permite rezolvarea unora dintre cerinte.

20.06.2024
In loc de un script care ruleaza fie executabilul serverului, fie cel al clientului, acum, atat implementarea pentru server cat si cea pentru client sunt in acelasi cpp.

21.06.2024
Adaugarea serverului si a clientului pentru UDP.
