from application import Application


class MyApp(Application):
  """ A simple demonstration app. """

  def on_reconfiguration(self, config):
    has_connection = False
    for value in config.itervalues():
      if value is not None:
        has_connection = True

    if has_connection:
      self.clear_display()
      self.draw_text("A", (0, 0), 42)
    else:
      self.clear_display()
      self.draw_text("B", (0, 0), 42)
