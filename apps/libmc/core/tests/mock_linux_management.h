#ifndef LIBMC_CORE_TESTS_MOCK_LINUX_MANAGEMENT_H_
#define LIBMC_CORE_TESTS_MOCK_LINUX_MANAGEMENT_H_

#include "gmock/gmock.h"

#include "apps/libmc/core/linux_management_interface.h"

namespace libmc {
namespace core {
namespace testing {

// Basic mock class for anything conforming to the LinuxManagementInterface.
class MockLinuxManagement : public LinuxManagementInterface {
 public:
  MOCK_CONST_METHOD0(Sync, void());
  MOCK_CONST_METHOD0(Halt, bool());
};

}  // namespace testing
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_MOCK_LINUX_MANAGEMENT_H_
