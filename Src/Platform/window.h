#pragma once
#pragma execution_character_set("shift_jis")
#include <functional>

#include "../Core/common.h"

constexpr uint32_t kFrameCount = 2;

// �E�B���h�E�̃T�C�Y���܂Ƃ߂�struct
struct WindowSize
{
    uint32_t width;
    uint32_t height;
};

// Win32�E�B���h�E�S�̂��Ǘ�����N���X
class Window
{
public:
    Window();
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;
    // �E�B���h�E���쐬����
    // title: �^�C�g���o�[�ɕ\�����镶����
    // width: �E�B���h�E�̉���
    // height: �E�B���h�E�̏c��
    // �Ԃ�l: �쐬�ł�����true
    bool Create(const wchar_t* title, uint32_t width, uint32_t height);

    // �E�B���h�E��\������
    void Show() const;

    // ���t���[��Windows�C�x���g����������
    bool ProcessMessages() const;

    // �E�B���h�E�n���h�����擾����
    HWND GetHwnd() const;

    // �E�B���h�E�̉��̃T�C�Y���擾����
    uint32_t GetWidth() const;

    // �E�B���h�E�̏c�̃T�C�Y���擾����
    uint32_t GetHeight() const;

    // �E�B���h�E�̃T�C�Y���擾����
    WindowSize GetSize() ;
    void DispFPS(const wchar_t* title);


    using ResizeCallback = std::function<void(uint32_t, uint32_t)>;
    void SetResizeCallback(ResizeCallback callback);

private:
    // Win32����Ăяo����郁�b�Z�[�W��M�p�̊֐�
    // HWND�ɕۑ����ꂽthis�����o���āAHandleMessage()�֐��֓n���B
    // hwnd: �ʒm��E�B���h�E
    // msg: ���b�Z�[�W���
    // wParam: msg���Ƃ̒ǉ����1
    // lParam: msg���Ƃ̒ǉ����2
    // return: ���b�Z�[�W�̏�������
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // WndProc����n���ꂽ���b�Z�[�W�����ۂɏ�������
    // msg: ���b�Z�[�W���
    // wParam: msg���Ƃ̒ǉ����1
    // lParam: msg���Ƃ̒ǉ����2
    // return: ���b�Z�[�W�̏�������
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    HINSTANCE instance_ = nullptr;
    HWND hwnd_ = nullptr;
    WindowSize size_;
    bool closed_ = false;
    std::wstring class_name_ = L"FrameWorkWindowClass";
    ResizeCallback resize_callback_ = nullptr;
};