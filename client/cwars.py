from __future__ import annotations

import signal
import arcade
import libcwars

SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600
FPS = 30
SCREEN_TITLE = "CWars"


def signal_handler(sig, frame):
    print("You pressed Ctrl+C!")
    arcade.close_window()


class MyGame(arcade.Window):
    """

    Main application class.

    """

    def __init__(self):

        # Call the parent class and set up the window

        super().__init__(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE)

        self.background_color = arcade.csscolor.BLACK

        self.cwars = libcwars.CWars("../build/libcwars_client.so")

    def setup(self):
        """Set up the game here. Call this function to restart the game."""

        if not self.cwars.client_connect("127.0.0.1", 9876):
            print("Failed to connect to server")
        else:
            print("Connected to server")

    def on_draw(self):
        """Render the screen."""

        self.clear()

        entities = self.cwars.get_entities()

        for entity in entities:
            arcade.draw_circle_filled(
                self.cwars.lib.fixed_to_float(entity.position.x), self.cwars.lib.fixed_to_float(entity.position.y), 32, arcade.color.WHITE
            )

        # Code to draw the screen goes here

    def on_mouse_press(self, x, y, button, modifiers):
        """Called whenever the mouse button is clicked."""

        print(f"Click at ({x}, {y})")

        self.cwars.create_entity(x, y)

    def on_key_press(self, key, modifiers):
        """Called whenever a key is pressed."""

        # Jump
        if key == arcade.key.UP or key == arcade.key.W:
            entities = self.cwars.get_entities()

            for entity in entities:
                print(f"Entity id: {entity.id}")
                self.cwars.lib.vector_print(entity.position)
                print(
                    f"Jumping entity at ({self.cwars.lib.fixed_to_float(entity.position.x)}, {self.cwars.lib.fixed_to_float(entity.position.y)})"
                )

    def on_update(self, delta_time: float):
        """Movement and game logic"""
        self.cwars.update(int(delta_time * 1000))


def main():
    """Main function"""

    signal.signal(signal.SIGINT, signal_handler)

    window = MyGame()

    window.setup()

    window.set_update_rate(1 / FPS)

    arcade.run()


if __name__ == "__main__":

    main()
