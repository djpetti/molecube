from application import Application


class MyApp(Application):
  """ A simple demonstration app. """

  def on_reconfiguration(self, config):
    print config
