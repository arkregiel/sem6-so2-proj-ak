import sys
import socket
import threading
import netaddr
import argparse


class ChatClient:
    """Class that represents the chat client"""

    __name: str = ""
    __server_addr: str = ""
    __server_port: int = 0
    __socket: socket.socket = None
    __stay_alive: bool = True
    __receiving_thread: threading.Thread = None

    def __init__(self, name: str, server_addr: str, server_port: int):
        """
        constructor for ChatClient
        :param name: name of the client
        :param server_addr: server address
        :param server_port: server port
        """
        netaddr.IPAddress(server_addr)
        assert server_port > 0
        assert server_port < 2**16

        self.__name = name
        self.__server_addr = server_addr
        self.__server_port = server_port

    def receive_data(self) -> None:
        """
        method responsible for listening for new messages and displaying them
        """
        try:
            while self.__stay_alive:
                message = self.__socket.recv(1024).decode().strip()
                if not message:
                    break
                print(message)
        except (socket.error, UnicodeDecodeError) as e:
            print(e)
        except KeyboardInterrupt:
            return

    def send_data(self) -> None:
        """
        method responsible for sending data to the server
        """
        try:
            while True:
                message = input().strip()
                if not message:
                    continue
                self.__socket.sendall(message.encode())
        except (socket.error, UnicodeEncodeError) as e:
            print(e)
        except KeyboardInterrupt:
            self.__stay_alive = False
            print("Goodbye")
        finally:
            self.__socket.close()

    def start(self) -> None:
        """
        method that starts the client and establishes connections to the server via socket
        """
        try:
            self.__socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.__socket.connect((self.__server_addr, self.__server_port))

            self.__socket.sendall(self.__name.encode())

            #spinning up a new thread responsible for receiving data and displaying it
            self.__receiving_thread = threading.Thread(target=self.receive_data)
            self.__receiving_thread.daemon = True
            self.__receiving_thread.start()

            self.send_data()
        except socket.error as e:
            print(e)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="ChatClient - project for Operating systems 2",
        epilog=f"example: python3 {sys.argv[0]} -a 127.0.0.1 -p 1337 -n docent",
    )

    parser.add_argument(
        "-a", "--address", help="IPv4 address", metavar="ADDR", required=True
    )

    parser.add_argument(
        "-p", "--port", help="Port number", metavar="PORT", required=True
    )

    parser.add_argument("-n", "--name", help="Nickname", metavar="NAME", required=True)

    args = parser.parse_args(sys.argv[1:])

    client = ChatClient(args.name[:16], args.address, int(args.port))
    client.start()
