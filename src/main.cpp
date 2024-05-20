#include <iostream>

#include <boost/asio.hpp>

#include <controller.h>
#include <process.h>


int main(int argc, char **argv)
{
    
    boost::asio::io_context ctx;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuard{boost::asio::make_work_guard(ctx)};

    unsigned int const numOfThreads{2};
    boost::asio::thread_pool pool(numOfThreads);

    for (auto i=0u; i<numOfThreads; ++i)
    {
        boost::asio::post(pool,
                          [&ctx] ()
                          {
                              ctx.run();
                          });
    }
    
    Controller controller(ctx);

    pool.join();
    
    return 0;
}
