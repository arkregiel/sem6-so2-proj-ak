# Systemy operacyjne 2 - projekt 2

Autorzy:
- [Artur Kręgiel](https://github.com/arkregiel)
- [Miłosz Bedryło](https://github.com/lolex565)

Prowadzący: [mgr inż. Damian Raczkowski](https://github.com/DocentSzachista)

## Opis projektu

Zadanie polegało na implementacji polegającego na implementacji prostego wielowątkowego czatu.

## Uruchamianie

Projekt został zrealizowany w języku Python w wersji 3.13.2.

Przed uruchomieniem należy zainstalować zależności za pomocą polecenia:

```
pip install -r requirements.txt
```

W pierwszej kolejności należy uruchomić serwer czatu, którego kod znajduje się w pliku `server.py`

```
python server.py -a 127.0.0.1 -p 1337
```

gdzie:
- `-a` oznacza adres IPv4, na którym nasłuchuje serwer
- `-p` oznacza numer portu, na którym nasłuchuje serwer

Po uruchomieniu serwera, można uruchamiać klientów. Kod klienta znajduje się w pliku `client.py`

```
python client.py -a 127.0.0.1 -p 1337 -n docent 
```

gdzie:
- `-a` oznacza adres IPv4, na którym nasłuchuje serwer
- `-p` oznacza numer portu, na którym nasłuchuje serwer
- `-n` oznacza nazwę użytkownika (nick)