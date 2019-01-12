import mock
import unittest

import numpy as np

from apps.libmc.sim.protobuf import graphics_message_pb2

from simulator import cube, event


class TestCube(unittest.TestCase):
  """ Tests for the Cube class. """

  @mock.patch("simulator.display.Display")
  @mock.patch("simulator.virtual_cube.cube_vm.CubeVm")
  def setUp(self, mocked_cube_vm, mocked_display):
    # Create a fake canvas.
    self.__mocked_canvas = mock.MagicMock()
    # Create a cube for testing.
    self.__cube = cube.Cube(self.__mocked_canvas, (0, 0), "blue")

    # Save the display mock so we can access it later.
    self.__mocked_display = mocked_display

    # Reset the mocks so they are in a known state.
    self.__mocked_canvas.reset_mock()
    self.__mocked_display.reset_mock()

  @mock.patch("simulator.virtual_cube.cube_vm.CubeVm.select_on")
  @mock.patch("simulator.virtual_cube.cube_vm.CubeVm")
  @mock.patch("simulator.display.Display")
  @mock.patch("simulator.virtual_cube.cube_vm.CubeVm")
  def test_select_on(self, mocked_cube_vm, mocked_display, mocked_vm,
                     mocked_select):
    """ Tests that select_on() works under normal conditions. """
    # We're just going to create some fake "CubeVms" so we can tell them
    # appart.
    fake_vms = []
    for i in range(0, 3):
      fake_vms.append(mock.MagicMock())
    mocked_vm.side_effect = fake_vms

    # Make some cubes to test with.
    cubes = []
    for i in range(0, 3):
      # The arguments that we don't use we can just make into mocks.
      cubes.append(cube.Cube(mock.MagicMock(), (0, 0),
                             mock.MagicMock()))

    # It should have initialized the CubeVms.
    expected_calls = [mock.call()] * len(cubes)
    mocked_vm.assert_has_calls(expected_calls)
    # It should have started them too.
    for fake_vm in fake_vms:
      fake_vm.start.assert_called_once_with()

    # Make it look like the first one is readable.
    mocked_select.return_value = [fake_vms[0]]

    readable = cube.Cube.select_on(cubes)

    # It should have indicated that the correct one is readable.
    self.assertListEqual([cubes[0]], readable)
    # It should have called select_on().
    mocked_select.assert_called_once()

  @mock.patch("simulator.display.Display")
  @mock.patch("simulator.virtual_cube.cube_vm.CubeVm")
  def test_screen_change(self, mocked_cube_vm, mocked_display):
    """ Tests that the screen change event callback works under normal
    conditions. """
    # Create a cube.
    my_cube = cube.Cube(self.__mocked_canvas, (0, 0), "blue")

    # It should have created the display.
    mocked_display.assert_called_once()

    # Make sure it bound the event.
    self.__mocked_canvas.bind_cube_event.assert_called_with(event.GraphicsEvent,
                                                            my_cube,
                                                            mock.ANY)
    # Grab the internal callback function.
    args, _ = self.__mocked_canvas.bind_cube_event.call_args
    _, _, callback = args

    image = np.zeros((100, 100, 3), dtype=np.uint8)

    # Create a fake event and try dispatching it.
    fake_tk_event = mock.MagicMock()
    fake_tk_event.cube_id = 0
    fake_tk_event.op_type = graphics_message_pb2.GraphicsMessage.PAINT
    fake_tk_event.image_width = 100
    fake_tk_event.image_height = 100
    fake_tk_event.image_data = image.tobytes()
    fake_event = event.GraphicsEvent(fake_tk_event)

    # Pass this event to the event handler.
    callback(fake_event)

    # It should have updated the screen image.
    mocked_display_inst = mocked_display.return_value
    mocked_display_inst.update.assert_called_once()
    args, _ = mocked_display_inst.update.call_args
    set_image = args[0]
    self.assertTrue(np.array_equal(image, set_image))

  @mock.patch("simulator.display.Display")
  @mock.patch("simulator.virtual_cube.cube_vm.CubeVm")
  def test_screen_change_unsupported_op(self, mocked_cube_vm, mocked_display):
    """ Tests that the screen change event callback handles an unsupported
    operation. """
    # Create a cube.
    my_cube = cube.Cube(self.__mocked_canvas, (0, 0), "blue")

    # It should have created the display.
    mocked_display.assert_called_once()

    # Make sure it bound the event.
    self.__mocked_canvas.bind_cube_event.assert_called_with(event.GraphicsEvent,
                                                            my_cube,
                                                            mock.ANY)
    # Grab the internal callback function.
    args, _ = self.__mocked_canvas.bind_cube_event.call_args
    _, _, callback = args

    # Create a fake event and try dispatching it.
    fake_tk_event = mock.MagicMock()
    # This is an invalid op.
    fake_tk_event.op_type = -1
    fake_event = event.GraphicsEvent(fake_tk_event)

    # Pass this event to the event handler.
    callback(fake_event)

    # It should not have done anything.
    mocked_display.return_value.update.assert_not_called()


if __name__ == "__main__":
  unittest.main()
