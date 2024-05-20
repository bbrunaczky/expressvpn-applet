#include <process.h>
#include <iostream>
    
void Process::read()
{
    read(_out);
    read(_err);
}

    
void Process::read(Capture & c)
{
    auto ptr = shared_from_this();
    boost::asio::async_read(c.pipe,
                            boost::asio::buffer(c.buf, c.BUF_SIZE),
                            [this, ptr, &c] (boost::system::error_code const & ec, std::size_t size)
                            {
                                    
                                std::string_view sv(c.buf, size);
                                std::size_t firstChar{0};
                                auto pos = sv.find('\n');
                                while (std::string::npos != pos)
                                {
                                    c.lineBuffer.append(&c.buf[firstChar], pos-firstChar);
                                    c.result.push_back(std::move(c.lineBuffer));
                                    firstChar = pos + 1;
                                    pos = sv.find('\n', firstChar);
                                }
                                if (firstChar < size)
                                {
                                    c.lineBuffer += sv.substr(firstChar);
                                }

                                if (boost::asio::error::eof == ec)
                                {
                                    auto prev = _flag.exchange(true);
                                    if (prev)
                                    {
                                        // both stdout and stderr captures finish
                                        _child.wait();
                                        _callback(_child.exit_code(), std::move(_out.result), std::move(_err.result));
                                    }
                                    return;
                                }

                                read(c);
                            });
}


void Process::terminate()
{
    _child.terminate();
}


void Process::test(boost::asio::io_context & ctx)
{
    auto callback = [] (short retCode, std::list<std::string> stdOut, std::list<std::string> stdErr)
                    {
                        std::cout << "Return code: " << retCode << std::endl;
                        std::cout << std::endl << "StdOut:" << std::endl;
                        for (auto const & line: stdOut)
                        {
                            std::cout << line << "|" << std::endl;
                        }
                        std::cout << std::endl << "StdErr:" << std::endl;
                        for (auto const & line: stdErr)
                        {
                            std::cout << line << "|" << std::endl;
                        }
                    };
    std::shared_ptr<Process> out = Process::execute(ctx, callback, "/bin/ls", "-l", "/root/");
    std::shared_ptr<Process> err = Process::execute(ctx, callback, "/bin/ls", "-l");
    std::shared_ptr<Process> hanging = Process::execute(ctx, callback, "/home/winnie/work/expressvpn-applet/build/do.sh");

    std::shared_ptr<boost::asio::deadline_timer> timer = std::make_shared<boost::asio::deadline_timer>(ctx);
    timer->expires_from_now(boost::posix_time::seconds(10));
    timer->async_wait([timer, hanging] (boost::system::error_code const &)
                      {
                          std::cout << "Terminating hanging process" << std::endl;
                          hanging->terminate();
                      });
    
}
