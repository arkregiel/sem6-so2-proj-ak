# Systemy operacyjne 2 - projekt 1

## Opis projektu

Zadanie polegało na rozwiązaniu problemu jedzących filozofów, czyli klasyczny problem synchronizacji.

Proces tworzy wątek dla każdego filozofa. Filozof może być w jednym z trzech stanów: _myśli_, _je_ lub _jest głodny_ (czeka na możliwość jedzenia). Sekcją krytyczną jest moment, w którym filozof próbuje zacząć jeść (próbuje chwycić pałeczki/widelce po jego lewej i prawej stronie). Aby wyeliminować zjawisko wyścigu (ang. _race condition_) zastosowano mutex, który powoduje, że w jednej chwili tylko jeden wątek znajduje się w sekcji krytycznej (próbuje zacząć jeść). Gdy dany filozof kończy jeść, wybudza wątki filozofów z jego _lewej_ i _prawej_ strony które oczekują na możliwość jedzenia.

## Kompilacja i uruchamianie

Program działa na systemach opartych o jądro Linux. Aby skompilować program, należy otworzyć w terminalu katalog `Projekt1`, a następnie wykonać polecenie `make`. Zostanie utworzony plik `main` w katalogu `out`.

Program uruchamiamy wraz z liczbą filozofów:

```bash
$ ./out/main <philosophersCount>
```

Na przykład:

```bash
$ ./out/main 5
```
