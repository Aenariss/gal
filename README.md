GAL Projekt - Téma 14 - Porovnání - Plánování cest dopravních prostředků

Autoři: Vojtěch Fiala <xfiala61>, Peter Močáry <xmocar00>

Pro překlad stačí použít -make-.
Spuštění je potom možné provádět pomocí např. ./gal --algorithm savings ./data/A-n32-k05.xml
Jsou 2 možnosti spuštění -- savings a genetic.
Princip je popsán v dokumentaci.

Pro experimenty pak slouží python skripty, např. python3 ./compare.py, případně další skripty. Tento skript konkrétně spočítá rozdíly pro všechny instance ve složce data/.
To znamená instance 0-100 nodes.

Další instance jsou ve složce data/data-sets/. Pro jejich spuštění slouží zbývající python skripty.

Jelikož by běh experimentů byl na velmi dlouho, ve složce results jsou již spočítané výsledky.

Pro spuštění všech porovnání je potřeba python a odpovídající knihovny (pip install -r requirements.txt).
