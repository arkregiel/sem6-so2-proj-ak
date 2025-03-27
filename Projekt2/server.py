import sys
import socket
import threading
import netaddr
import argparse


class ChatClient:
    """Class that contains info about client"""

    name: str = ""
    sock: socket.socket = None
    address: str = ""
    port: int = 0

    def __init__(self, name: str, sock: socket.socket, addr: str, port: int):
        netaddr.IPAddress(addr)
        assert port > 0
        assert port < 2**16

        self.name = name
        self.sock = sock
        self.address = addr
        self.port = port

    def __str__(self):
        return f"{self.name}[{self.address}:{self.port}]"

    def __repr__(self):
        return self.__str__()


class ChatServer:
    """Class responsible for running chat server and handling client connections and messaging"""

    __server_address: str = ""
    __server_port: int = 0
    __server_socket: socket.socket | None = None
    __connected_clients: list[ChatClient] = []
    __client_threads: list[threading.Thread] = []
    __stay_alive: bool = True
    __mutex: threading.Lock = threading.Lock()
    __history: list[str] = []

    def __init__(self, addr: str, port: int):
        netaddr.IPAddress(addr)
        assert port > 0
        assert port < 2**16
        self.__server_address = addr
        self.__server_port = port

    def __client_handler(self, client: ChatClient):
        try:
            while self.__stay_alive:
                message = client.sock.recv(1024).decode().strip()
                if not message:
                    break

                print(f"[{client}] {message}")

                self.broadcast(client, f"[{client.name}] {message}")

        except (socket.error, UnicodeDecodeError) as e:

            print(f"[!!] Error: {e}")
        finally:
            self.__connected_clients.remove(client)
            client.sock.close()
            print(f"[-] Client {client} disconnected")
            self.broadcast(client, f"[Server] {client.name} left the chat")
            return

    def broadcast(self, client: ChatClient, message: str) -> None:
        self.__mutex.acquire()

        self.__history.append(message)

        for c in self.__connected_clients:
            if c.name == client.name:
                continue
            try:
                c.sock.sendall(message.encode())
            except socket.error as e:
                print(e)

        self.__mutex.release()

    def start(self) -> None:
        self.__server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.__server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.__server_socket.bind((self.__server_address, self.__server_port))
        self.__server_socket.listen(5)
        print(f"[*] Server listening on [{self.__server_address}:{self.__server_port}]")

        try:
            while True:
                try:
                    client_socket, remote_addr = self.__server_socket.accept()
                    print(
                        f"[+] New connection from [{remote_addr[0]}:{remote_addr[1]}]"
                    )

                    name = client_socket.recv(16).decode().strip()
                    new_client = ChatClient(
                        name, client_socket, remote_addr[0], remote_addr[1]
                    )
                    self.__connected_clients.append(new_client)

                    self.broadcast(new_client, f"[Server] {name} joined chat")

                    self.__mutex.acquire()

                    new_client.sock.sendall("\n".join(self.__history).encode())

                    self.__mutex.release()

                    t = threading.Thread(
                        target=self.__client_handler, args=(new_client,)
                    )
                    self.__client_threads.append(t)
                    t.daemon = True
                    t.start()
                except (socket.error, UnicodeDecodeError) as e:
                    print(e)

        except KeyboardInterrupt:
            self.__stay_alive = False
            (t.join() for t in self.__client_threads)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Chat Server - project for Operating systems 2",
        epilog=f"example: python3 {sys.argv[0]} -a 127.0.0.1 -p 1337",
    )

    parser.add_argument(
        "-a", "--address", help="IPv4 address", metavar="ADDR", required=True
    )

    parser.add_argument(
        "-p", "--port", help="Port number", metavar="PORT", required=True
    )

    args = parser.parse_args(sys.argv[1:])

    server = ChatServer(args.address, int(args.port))
    server.start()
