#define STB_IMAGE_IMPLEMENTATION
#include "imagemanager.h"

bool ImageManager::ImageMemory(unsigned char* imagebytes, UINT size, TextureType* out)
{
    if (!pDevice || !out)
        return false;

    int            width = 0, height = 0;
    unsigned char* data = stbi_load_from_memory(imagebytes, size, &width, &height, nullptr, 4);
    if (!data)
        return false;

#ifdef USE_DX11

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width                = width;
    desc.Height               = height;
    desc.MipLevels            = 1;
    desc.ArraySize            = 1;
    desc.Format               = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count     = 1;
    desc.Usage                = D3D11_USAGE_DEFAULT;
    desc.BindFlags            = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA sub = {};
    sub.pSysMem                = data;
    sub.SysMemPitch            = width * 4;

    ID3D11Texture2D* tex = nullptr;
    if (FAILED(pDevice->CreateTexture2D(&desc, &sub, &tex)))
    {
        stbi_image_free(data);
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                          = desc.Format;
    srvDesc.ViewDimension                   = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels             = 1;

    if (FAILED(pDevice->CreateShaderResourceView(tex, &srvDesc, out)))
    {
        tex->Release();
        stbi_image_free(data);
        return false;
    }

    tex->Release();

#else // DX9

    IDirect3DTexture9* tex = nullptr;
    if (FAILED(pDevice->CreateTexture(
            width, height, 1,
            D3DUSAGE_DYNAMIC,
            D3DFMT_A8R8G8B8,
            D3DPOOL_DEFAULT,
            &tex, nullptr)))
    {
        stbi_image_free(data);
        return false;
    }

    D3DLOCKED_RECT rect;
    if (FAILED(tex->LockRect(0, &rect, nullptr, 0)))
    {
        tex->Release();
        stbi_image_free(data);
        return false;
    }

    for (int y = 0; y < height; y++)
    {
        unsigned char* dst = (unsigned char*)rect.pBits + y * rect.Pitch;
        unsigned char* src = data + y * width * 4;

        for (int x = 0; x < width; x++)
        {
            dst[0] = src[2]; // B
            dst[1] = src[1]; // G
            dst[2] = src[0]; // R
            dst[3] = src[3]; // A
            dst += 4;
            src += 4;
        }
    }

    tex->UnlockRect(0);
    *out = tex;

#endif

    stbi_image_free(data);
    return true;
}

TextureType ImageManager::CreateTextureFromPixels(unsigned char* pixels, int width, int height)
{
    if (!pDevice || !pixels)
        return nullptr;

#ifdef USE_DX11

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width                = width;
    desc.Height               = height;
    desc.MipLevels            = 1;
    desc.ArraySize            = 1;
    desc.Format               = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count     = 1;
    desc.Usage                = D3D11_USAGE_DEFAULT;
    desc.BindFlags            = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem                = pixels;
    data.SysMemPitch            = width * 4;

    ID3D11Texture2D* tex = nullptr;
    if (FAILED(pDevice->CreateTexture2D(&desc, &data, &tex)))
        return nullptr;

    ID3D11ShaderResourceView* srv = nullptr;
    if (FAILED(pDevice->CreateShaderResourceView(tex, nullptr, &srv)))
    {
        tex->Release();
        return nullptr;
    }

    tex->Release();
    return srv;

#else // DX9

    IDirect3DTexture9* tex = nullptr;
    if (FAILED(pDevice->CreateTexture(
            width, height, 1,
            D3DUSAGE_DYNAMIC,
            D3DFMT_A8R8G8B8,
            D3DPOOL_DEFAULT,
            &tex, nullptr)))
        return nullptr;

    D3DLOCKED_RECT rect;
    if (FAILED(tex->LockRect(0, &rect, nullptr, 0)))
    {
        tex->Release();
        return nullptr;
    }

    for (int y = 0; y < height; y++)
    {
        unsigned char* dst = (unsigned char*)rect.pBits + y * rect.Pitch;
        unsigned char* src = pixels + y * width * 4;

        for (int x = 0; x < width; x++)
        {
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
            dst[3] = src[3];
            dst += 4;
            src += 4;
        }
    }

    tex->UnlockRect(0);
    return tex;

#endif
}