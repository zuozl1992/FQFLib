#pragma once
class FQFCore
{
public:
    static FQFCore * getObject()
    {
        static FQFCore f;
        return &f;
    }

protected:
    FQFCore();
};
