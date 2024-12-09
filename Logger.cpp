#include "Logger.h"
#include<dxgidebug.h>

namespace Logger
{
void Log(const std::string& message) { OutputDebugStringA(message.c_str()); }

void Log(const std::wstring& message) { OutputDebugStringA(StringUtility::ConvertString(message).c_str()); }

};
