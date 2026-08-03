#pragma once
#include <stb_image/stb_image.h>

#ifdef USE_DX11
#include <d3d11.h>
using TextureType = ID3D11ShaderResourceView*;
#else
#include <d3d9.h>
using TextureType = IDirect3DTexture9*;
#endif

inline class ImageManager
{
public:
    bool        ImageMemory(unsigned char* imagebytes, UINT size, TextureType* out);
    TextureType CreateTextureFromPixels(unsigned char* pixels, int width, int height);
public:
#ifdef USE_DX11
    ID3D11Device* pDevice = nullptr;
#else
    IDirect3DDevice9* pDevice = nullptr;
#endif

} imagemanager;

#define CREATE_TEXTURE(bytes, tex, w, h)                               \
    do                                                                 \
    {                                                                  \
        if (!(tex))                                                    \
        {                                                              \
            (tex) = imagemanager.CreateTextureFromPixels(bytes, w, h); \
        }                                                              \
    } while (0)

#define LOAD_IMAGE(bytes, out) imagemanager.ImageMemory((unsigned char*)bytes, sizeof(bytes), &(out))