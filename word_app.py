from application import Application
from tabletop import Cube


class WordGameLetter(Application):
  """ A simple word game for a little demo. This version of the app displays a
  letter. """

  def __init__(self, letter):
    """
    Args:
      letter: A single character to display. """
    self.__letter = letter

    # The word displayed on the cubes to the right of us.
    self.__right_word = ""
    # The word displayed on the cubes to the left of us.
    self.__left_word = ""
    # The side that the most recent word request came from.
    self.__word_req_side = None
    # How many word request messages we sent.
    self.__sent_word_requests = 0
    # How many word request messages we received.
    self.__received_words = 0

    # Keeps track of the current cube connections.
    self.__connections = {Cube.Sides.LEFT: None,
                          Cube.Sides.RIGHT: None,
                          Cube.Sides.TOP: None,
                          Cube.Sides.BOTTOM: None}

  def __handle_word_message(self, side, message):
    """ Handles a message requesting the currently-displayed word.
    Args:
      side: The side that the sender is connected on.
      message: The message to handle. """
    self.__left_word = ""
    self.__right_word = ""
    self.__word_req_side = side
    self.__sent_word_requests = 0
    self.__received_words = 0

    # We get the word by building it up recursively based on the words we get
    # from the cubes on the left and right.
    if (side != Cube.Sides.LEFT and self.__connections[Cube.Sides.LEFT]):
      # Request the word from the cubes on the left of us.
      self.send_message(Cube.Sides.LEFT, {"type": "word"})
      self.__sent_word_requests += 1
    if (side != Cube.Sides.RIGHT and self.__connections[Cube.Sides.RIGHT]):
      # Request the word from the cubes on the right of us.
      self.send_message(Cube.Sides.RIGHT, {"type": "word"})
      self.__sent_word_requests += 1

    if self.__sent_word_requests == 0:
      # In the base case, we can just provide our letter.
      resp_message = {"type": "word_resp", "word": self.__letter}
      self.send_message(side, resp_message)

    elif self.__received_words == self.__sent_word_requests:
      # We've received responses for everything we sent. Now we can send our own
      # response.
      word = self.__left_word + self.__letter + self.__right_word
      resp_message = {"type": "word_resp", "word": word}
      self.send_message(self.__word_req_side, resp_message)

  def __handle_word_resp_message(self, side, message):
    """ Handles a message responding to a word request.
    Args:
      side: The side that the sender is connected on.
      message: The message to handle. """
    if side == Cube.Sides.LEFT:
      # We got the left word.
      self.__left_word = message["word"]
    if side == Cube.Sides.RIGHT:
      # We got the right word.
      self.__right_word = message["word"]

    self.__received_words += 1

  def _start_app(self):
    # Draw the letter on the screen.
    self.clear_display()
    self.draw_text(self.__letter, (0, 0), 48)

  def _on_message_receive(self, side, message):
    if message["type"] == "word":
      # This message is requesting the currently-displayed word.
      self.__handle_word_message(side, message)
    if message["type"] == "word_resp":
      # This message is in response to a word request message.
      self.__handle_word_resp_message(side, message)

  def on_reconfiguration(self, config):
    # Save the new configuration.
    self.__connections = config

class WordGameChecker(Application):
  """ A simple word game for a little demo. This version of the app can be
  connected to a word in order to check its validity. """

  def _start_app(self):
    # Indicate that this is for checking.
    self.clear_display()
    self.draw_text("Check", (0, 0), 18)

  def _on_message_receive(self, side, message):
    # Get the word response.
    word = message["word"]
    print word

  def on_reconfiguration(self, config):
    # If we connect the checker cube to something, we want to check the current
    # word. First we have to get it. Choose the side to send it to.
    send_side = None
    for side, cube in config.iteritems():
      if cube is not None:
        # This side is connected.
        send_side = side
        break

    if send_side is None:
      # This means we disconnected the checker cube, in which case, do nothing.
      return

    # Send the word check message.
    self.send_message(send_side, {"type": "word"})
