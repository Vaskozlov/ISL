#ifndef ISL_PROJECT_TEST_STRUCTURES_HPP
#define ISL_PROJECT_TEST_STRUCTURES_HPP

struct A
{
    A() = default;

    virtual ~A() = default;

    virtual auto getId() const -> int
    {
        return 10;
    }
};

struct B : A
{
    auto getId() const -> int override
    {
        return 20;
    }
};

#endif /* ISL_PROJECT_TEST_STRUCTURES_HPP */
