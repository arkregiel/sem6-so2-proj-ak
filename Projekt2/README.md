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
python client.py -a 127.0.0.1 -p 1337 -n user 
```

gdzie:
- `-a` oznacza adres IPv4, na którym nasłuchuje serwer
- `-p` oznacza numer portu, na którym nasłuchuje serwer
- `-n` oznacza nazwę użytkownika (nick)

## Działanie

Po uruchomieniu serwera ten nasłuchuje na podanym interfejsie i porcie nadchodzących połączeń od klientów. W momencie nadejścia połączenia od strony klienta, na serwerze tworzona jest instancja obiektu reprezentującego klienta, oraz odpowiadający jednemu połączeniu wątek z działającą metodą __client_handler odpowiadającą za wysyłanie reszcie klientów wiadomości, oraz za dostęp do sekcji krytycznej, czyli zapisywanie do historii czatu.

Działanie klienta ogranicza się do połączenia się do socketa serwera na bazie argumentów podanych przy uruchomieniu, a później enkodowanie i wysyłanie do serwera wiadomości wpisywanej w oknie czatu. Oprócz tego w programie klienta do odbierania wiadomości od innych użytkowników przekazywanych przez serwer używany jest oddzielny wątek.