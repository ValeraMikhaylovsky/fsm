#pragma once

#include <memory>

class pimpl
{
public:
    pimpl();
    ~pimpl();

    void start();
    void stop();
    bool is_started() const;

private:
    struct impl;
    std::unique_ptr<impl> pImpl;
};

