#pragma once

#include <list>
#include <memory>
#include <boost/asio.hpp>
#include <boost/process.hpp>

class Process: public std::enable_shared_from_this<Process>
{
public:
    using Callback = std::function<void (short, std::list<std::string>, std::list<std::string>)>;
    
    template <typename... Ts>
    static std::shared_ptr<Process> execute(boost::asio::io_context & ctx, Callback callback, std::string const & cmd, Ts &&... args);

    void terminate();

    static void test(boost::asio::io_context & ctx);
    
private:
    
    struct Capture
    {
        static constexpr unsigned int const BUF_SIZE{500};

        boost::process::async_pipe pipe;
        char buf[BUF_SIZE];
        std::string lineBuffer;
        std::list<std::string> result;

        Capture(boost::asio::io_context & ctx):
            pipe(ctx)
        {}
    };

    template <typename... Ts>
    Process(boost::asio::io_context & ctx, Callback callback, std::string const & cmd, Ts &&... args);
    
    void read();
    void read(Capture & c);

    Capture _out;
    Capture _err;
    Callback _callback;
    boost::process::child _child;
    std::atomic<bool> _flag;
};


template <typename... Ts>
std::shared_ptr<Process> Process::execute(boost::asio::io_context & ctx, Callback callback, std::string const & cmd, Ts &&... args)
{
    auto ptr = std::shared_ptr<Process>(new Process(ctx, std::move(callback), cmd, std::forward<Ts>(args)...));
    ptr->read();
    return ptr;
}



template <typename... Ts>
Process::Process(boost::asio::io_context & ctx, Callback callback, std::string const & cmd, Ts &&... args):
    _out(ctx),
    _err(ctx),
    _callback{callback},
    _child(cmd, args..., boost::process::std_out > _out.pipe, boost::process::std_err > _err.pipe),
    _flag{false}
{}
