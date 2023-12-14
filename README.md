GAL Projekt - Téma 14 - Porovnání - Plánování cest dopravních prostředků

Autoři: Vojtěch Fiala <xfiala61>, Peter Močáry <xmocar00>

Pro překlad stačí použít make.
Spuštění je potom možné provádět pomocí např. ./gal --algorithm savings ./data/A-n32-k05.xml
Jsou 2 možnosti spuštění -- savings a genetic.
Princip je popsán v dokumentaci.

Pro experimenty pak slouží python skripty, např. python3 ./compare.py, případně další skripty. Tento skript konkrétně spočítá rozdíly pro všechny instance ve složce data/.
To znamená instance 0-100 nodes.

Další instance jsou ve složce data/data-sets/. Pro jejich spuštění slouží zbývající python skripty.

Pro instance 100-200 slouží zbývající python skripty a jejich spouštění je následující:
python3 ./gather-data.py savings ./data/data-set/ results/savings-results.csv a poté
python3 ./compare-data.py ./results/savings-results.csv ./results/genetic-results.csv a případně i 
python3 ./plot-path.py savings ./data/data-set/A-n33-k05.xml

Jelikož by běh experimentů byl na velmi dlouho (kvůli valgrindu), ve složce results jsou již spočítané výsledky.

Pro spuštění všech porovnání je potřeba python a odpovídající knihovny (pip install -r requirements.txt).
