#include "texture_loder.h"
#include <filesystem>
#include "../Core/common.h"
#include  <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")

bool TextureLoader::LoadFromFile(const wchar_t* filename, LoadedImage& image)
{
    //IWIC の I は Interface、WIC は Windows Imaging Component。 
    ComPtr<IWICImagingFactory> factory;
    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> frame;
    ComPtr<IWICFormatConverter> converter;

    //Comライブラリの初期化（exは拡張版って意味)
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
    {
        return false;
    }
    //Comオブジェクトのインスタンスを作る関数。
    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
    if (FAILED(hr))
    {
        return false;
    }

    hr = factory->CreateDecoderFromFilename(filename, nullptr,GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
    if (FAILED(hr))
    {
        return false;
    }

    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr))
    {
        return false;
    }

    hr = factory->CreateFormatConverter(&converter);
    if (FAILED(hr))
    {
        return false;
    }
    hr = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.f,
                               WICBitmapPaletteTypeMedianCut);
    if (FAILED(hr))
    {
        return false;
    }

    UINT width, height;
    hr = converter->GetSize(&width, &height);
    if (FAILED(hr))
    {
        return false;
    }
    image.width = width;
    image.height = height;
    
    uint32_t row_pitch = width * 4;
    uint32_t size = row_pitch * height;
    image.pixels = std::vector<uint8_t>(size);
    hr = converter->CopyPixels(nullptr, row_pitch, size,image.pixels.data());
    return true;
}
