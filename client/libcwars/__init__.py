import ctypes

LOG_LEVEL_DEBUG = 7


class CTimer(ctypes.Structure):
    _fields_ = [
        ("timeout", ctypes.c_uint32),
        ("dt", ctypes.c_uint32),
        ("interval", ctypes.c_bool),
        ("userdata", ctypes.c_void_p),
        ("callback", ctypes.CFUNCTYPE(None, ctypes.c_void_p)),
    ]


class LinkedListLink(ctypes.Structure):
    pass


LinkedListLink._fields_ = [
    ("next", ctypes.POINTER(LinkedListLink)),
    ("previous", ctypes.POINTER(LinkedListLink)),
    ("data", ctypes.c_void_p),
]


class LinkedList(ctypes.Structure):
    _fields_ = [
        ("start", ctypes.POINTER(LinkedListLink)),
        ("end", ctypes.POINTER(LinkedListLink)),
        ("size", ctypes.c_uint32),
    ]


class Client(ctypes.Structure):
    _fields_ = [
        ("socket", ctypes.c_int),
        ("connected", ctypes.c_bool),
        ("clock", ctypes.c_int64),
        ("latency", ctypes.c_int64),
        ("delta_latency", ctypes.c_int64),
        ("latency_buffer", ctypes.POINTER(LinkedList)),
        ("out_message_queue", ctypes.POINTER(LinkedList)),
        ("in_message_queue", ctypes.POINTER(LinkedList)),
    ]


class Map(ctypes.Structure):
    _fields_ = [
        ("entities", ctypes.POINTER(LinkedList)),
        ("width", ctypes.c_uint32),
        ("height", ctypes.c_uint32),
    ]


class Game(ctypes.Structure):
    _fields_ = [
        ("client", ctypes.POINTER(Client)),
        ("latency_timer", ctypes.POINTER(CTimer)),
        ("map", ctypes.POINTER(Map)),
    ]


class CWars:
    def __init__(self, lib_so_path: str):
        self.lib = ctypes.CDLL(lib_so_path)

        self.__init_functions()

        self.lib.logging_init(b"cwars client", LOG_LEVEL_DEBUG)

        self.game = self.lib.game_init()

    def __del__(self):
        self.lib.game_cleanup(ctypes.pointer(self.game))

        self.lib.logging_cleanup()

    def __init_functions(self):
        self.lib.logging_init.argtypes = [
            ctypes.c_char_p,
            ctypes.c_int,
        ]

        self.lib.game_init.restype = ctypes.POINTER(Game)

        self.lib.client_connect.argtypes = [
            ctypes.POINTER(Client),
            ctypes.c_char_p,
            ctypes.c_int,
        ]
        self.lib.client_connect.restype = ctypes.c_bool

        self.lib.game_loop_once.argtypes = [ctypes.POINTER(Game), ctypes.c_int64]

        self.__init_messages()

    def __init_messages(self):
        self.lib.client_send_get_server_time_message.argtypes = [
            ctypes.POINTER(Client),
        ]

        self.lib.client_send_get_entities_message.argtypes = [
            ctypes.POINTER(Client),
        ]

    def client_connect(self, ip: str, port: int):
        res = self.lib.client_connect(
            self.game.contents.client, ip.encode("utf-8"), port
        )

        if res:
            self.lib.client_send_get_server_time_message(self.game.contents.client)

            self.lib.client_send_get_entities_message(self.game.contents.client)

        return res

    def update(self, delta_time: int):
        if self.game.contents.client.contents.connected:
            self.lib.game_loop_once(self.game, delta_time)
