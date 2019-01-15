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
    //注册ffmpeg支持文件类型、网络
    FQFCore();
};
