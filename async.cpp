#include <iostream>
#include "async.h"
#include "context.h"

namespace async
{

handle_t connect(std::size_t bulk)
{
    try
    {
        return ContextPool::Instance().make_context(bulk);
    }
    catch (...)
    {
        std::cerr << "Failed to create new context.\n";
        return nullptr;
    }
}

void receive(handle_t handle, const char *data, std::size_t size)
{
    try
    {
        auto context = ContextPool::Instance().find(handle);
        if (context)
        {
            context->process(data, size);
        }
    }
    catch (...)
    {
        std::cerr << "Failed to receive data.\n";
    }
}

void disconnect(handle_t handle)
{

    try
    {
        ContextPool::Instance().erase(handle);
    }
    catch (...)
    {
        std::cerr << "Failed to erase context.\n";
    }
}

} // namespace async
