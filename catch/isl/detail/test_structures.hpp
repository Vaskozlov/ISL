#ifndef ISL_PROJECT_TEST_STRUCTURES_HPP
#define ISL_PROJECT_TEST_STRUCTURES_HPP

#include <isl/isl.hpp>

struct A
{
    A() = default;

    virtual ~A() = default;

    ISL_DECL virtual auto getId() const -> int
    {
        return 10;
    }
};

struct B : A
{
    ISL_DECL auto getId() const -> int override
    {
        return 20;
    }
};


struct C : A
{
    ISL_DECL auto getId() const -> int override
    {
        return 42;
    }
};

#endif /* ISL_PROJECT_TEST_STRUCTURES_HPP */
