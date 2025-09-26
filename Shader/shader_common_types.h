#pragma once

enum class ShaderCacheDataType
{
    NONE_SHADER_CACHE_DATA_TYPE = 0,
    MATRIX = 1,
    VEC4,
    VEC3,
    VEC2,
    FLOAT,
    TEXTURE,
};

enum class ShaderType
{
    NONE_SHADER_TYPE = 0,
    SHADER_SETUP, // 用于简化相关代码
    VERTEX,
    FRAGMENT,
};

