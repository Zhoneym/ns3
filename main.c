#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <windows.h>

#define ENCODING_UTF8 1
#define ENCODING_UNKNOWN 0

// 假设你有这个宏定义来处理调试输出
#define WINE_TRACE(fmt, ...) wprintf(fmt, __VA_ARGS__)

wchar_t* utf8_to_utf16(const unsigned char* utf8Str, size_t length) {
    int wideCharCount = MultiByteToWideChar(CP_UTF8, 0, (const char*)utf8Str, length, NULL, 0);
    if (wideCharCount == 0) {
        return NULL;
    }

    // 分配内存用于存储 UTF-16 字符串
    wchar_t* utf16Str = (wchar_t*)malloc(wideCharCount * sizeof(wchar_t));
    if (utf16Str == NULL) {
        return NULL;
    }

    // 执行转换
    MultiByteToWideChar(CP_UTF8, 0, (const char*)utf8Str, length, utf16Str, wideCharCount);

    return utf16Str;
}

// 假设这里是编码检测函数
int detect_encoding(const wchar_t* filePath) {
    // 对于这个示例，简单返回 UTF-8 编码（你可以根据需要做更复杂的编码检测）
    return ENCODING_UTF8;
}

int main() {
    wchar_t confPath[MAX_PATH];
    wchar_t buffer[2048];  // 假设文件内容不超过 2048 字节
    wchar_t* utf16Str = NULL;
    FILE* confFile = NULL;
    wchar_t* tempBuffer = NULL;
    wchar_t fullDesktopPath[MAX_PATH];
    wchar_t* value = NULL;

    // 假设 env_username_w 是已知的用户名（例如："username"）
    wchar_t env_username_w[] = L"username";

    // 构建配置文件路径
    wcscat(confPath,env_username_w);
    wcscat(confPath,L"/.config/user-dirs.dirs");

    // 检测文件编码
    int encoding = detect_encoding(confPath);

    // 打开文件读取
    confFile = _wfopen(confPath, L"r");
    if (confFile == NULL) {
        wprintf(L"Failed to open file: %ls\n", confPath);
        return FALSE;
    }

    // 读取文件内容
    size_t bytesRead = fread(buffer, 1, sizeof(buffer), confFile);
    fclose(confFile);

    // 如果是 UTF-8 编码，将其转换为 UTF-16
    if (encoding == ENCODING_UTF8) {
        utf16Str = utf8_to_utf16((const unsigned char*)buffer, bytesRead);
        if (utf16Str == NULL) {
            wprintf(L"Conversion to UTF-16 failed.\n");
            return FALSE;
        }
    }

    // 复制文本到缓冲区，因为 wcstok 会修改字符串
    tempBuffer = _wcsdup(utf16Str);
    if (tempBuffer == NULL) {
        wprintf(L"Failed to allocate memory\n");
        return FALSE;
    }

    // 使用宽字符版本的 wcstok 分割字符串
    wchar_t* context = NULL;  // wcstok 的上下文
    wchar_t* line = wcstok(tempBuffer, L"\n", &context);
    while (line != NULL) {
        // 检查行是否包含 "XDG_DESKTOP_DIR=\"$HOME/"
        wprintf(L"line :%s\n", line);
        if (wcslen(line) >= 23 && wcsncmp(line, L"XDG_DESKTOP_DIR=\"$HOME/", 23) == 0) {
            wchar_t* slash = wcschr(line, L'/');
            if (slash != NULL) {
                value = slash + 1;
                wcscpy(fullDesktopPath, L"Z:\\home\\");
                wcscat(fullDesktopPath, env_username_w);
                wcscat(fullDesktopPath, L"\\");
                wchar_t* quote = wcschr(fullDesktopPath, L'\"');
                if (quote != NULL) {
                    *quote = L'\0';
                }
                wcscat(fullDesktopPath, L"\\");
                wprintf(L"fullDesktopPath :%s\n", fullDesktopPath);  // 打印结果
            }
        }
        line = wcstok(NULL, L"\n", &context);  // 获取下一行
    }

    // 释放动态分配的缓冲区
    free(tempBuffer);

    return TRUE;
}
