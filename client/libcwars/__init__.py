import ctypes

LOG_LEVEL_DEBUG = 7


class Vector(ctypes.Structure):
    _fields_ = [
        ("x", ctypes.c_int64),
        ("y", ctypes.c_int64),
    ]


class Entity(ctypes.Structure):
    _fields_ = [
        ("id", ctypes.c_char * 37),
        ("position", Vector),
        ("velocity", Vector),
        ("radius", ctypes.c_uint32),
    ]


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

        self.lib.fixed_from_int.argtypes = [ctypes.c_int32]

        self.lib.fixed_from_int.restype = ctypes.c_int64

        self.lib.fixed_to_float.argtypes = [ctypes.c_int64]

        self.lib.fixed_to_float.restype = ctypes.c_float

        self.lib.vector_print.argtypes = [Vector]

        self.__init_messages()

    def __init_messages(self):
        self.lib.client_send_get_server_time_message.argtypes = [
            ctypes.POINTER(Client),
        ]

        self.lib.client_send_get_entities_message.argtypes = [
            ctypes.POINTER(Client),
        ]

        self.lib.client_send_create_entity_message.argtypes = [
            ctypes.POINTER(Client),
            Vector,
        ]

    def client_connect(self, ip: str, port: int):
        res = self.lib.client_connect(
            self.game.contents.client, ip.encode("utf-8"), port
        )

        if res:
            self.lib.client_send_get_server_time_message(self.game.contents.client)

            self.lib.client_send_get_entities_message(self.game.contents.client)

        return res

    def create_entity(self, x: int, y: int):
        print(f"Creating entity at ({x}, {y})")
        pos = Vector(self.lib.fixed_from_int(x), self.lib.fixed_from_int(y))

        self.lib.vector_print(pos)

        self.lib.client_send_create_entity_message(self.game.contents.client, pos)

    def update(self, delta_time: int):
        if self.game.contents.client.contents.connected:
            self.lib.game_loop_once(self.game, delta_time)

    def get_entities(self):
        entities = []

        link = self.game.contents.map.contents.entities.contents.start

        while link:
            entity = link.contents.data

            # typecast to Entity
            entity = ctypes.cast(entity, ctypes.POINTER(Entity)).contents

            entities.append(entity)

            link = link.contents.next

        return entities
