import mock
import unittest

from simulator import cube


class TestCube(unittest.TestCase):
  """ Tests for the Cube class. """

  @mock.patch("simulator.virtual_cube.cube_vm.CubeVm.select_on")
  @mock.patch("simulator.virtual_cube.cube_vm.CubeVm")
  def test_select_on(self, mocked_vm, mocked_select):
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

if __name__ == "__main__":
  unittest.main()
