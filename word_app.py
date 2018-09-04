from application import Application


class WordGameLetter(Application):
  """ A simple word game for a little demo. This version of the app displays a
  letter. """

  def __init__(self, letter):
    """
    Args:
      letter: A single character to display. """
    self.__letter = letter

  def _start_app(self):
    # Draw the letter on the screen.
    self.clear_display()
    self.draw_text(self.__letter, (0, 0), 48)

class WordGameChecker(Application):
  """ A simple word game for a little demo. This version of the app can be
  connected to a word in order to check its validity. """

  def _start_app(self):
    # Indicate that this is for checking.
    self.clear_display()
    self.draw_text("Check", (0, 0), 18)
