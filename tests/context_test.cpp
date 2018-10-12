#include "gmock/gmock.h"
#include <string>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <memory>
#include <cstddef>
#include <mutex>
#include "context.h"
#include "async.h"

using namespace testing;

class TestContextCollection : public Test
{
  public:
    std::size_t bulk = 5;
    async::handle_t handle = 0;
    void SetUp() override
    {
        ContextPool::Instance();
    }
};

TEST_F(TestContextCollection, Test_make_context)
{
    ASSERT_EQ(ContextPool::Instance().get_context_size(), 0);
    ASSERT_EQ(ContextPool::Instance().find(handle), nullptr);
    handle = ContextPool::Instance().make_context(bulk);
    handle = ContextPool::Instance().make_context(bulk);
    ASSERT_EQ(ContextPool::Instance().get_context_size(), 2);
}

TEST_F(TestContextCollection, Test_erase_context)
{

    ASSERT_EQ(ContextPool::Instance().get_context_size(), 2);
    handle = ContextPool::Instance().make_context(bulk);
    ContextPool::Instance().erase(handle);
    ASSERT_EQ(ContextPool::Instance().get_context_size(), 2);
}

TEST_F(TestContextCollection, Test_find_context)
{
    ASSERT_EQ(ContextPool::Instance().get_context_size(), 2);
    handle = ContextPool::Instance().make_context(bulk);
    ASSERT_EQ(ContextPool::Instance().find(handle), handle);
}
