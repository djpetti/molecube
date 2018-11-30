#ifndef LIBMC_CORE_EVENTS_TESTS_MOCK_SYSTEM_MANAGER_H_
#define LIBMC_CORE_EVENTS_TESTS_MOCK_SYSTEM_MANAGER_H_

#include "gmock/gmock.h"

#include "apps/libmc/core/system_manager_interface.h"

namespace libmc {
namespace core {
namespace events {
namespace testing {

// Mock class for SystemManager.
class MockSystemManager : public SystemManagerInterface {
 public:
  MOCK_METHOD0(Shutdown, bool());
};

}  // namespace testing
}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_TESTS_MOCK_SYSTEM_MANAGER_H_
