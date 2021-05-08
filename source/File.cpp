#include "File.h"
#include "Error.h"
#include <Windows.h>

File::File():
    m_filesize(0),
    m_handle(0)
{}

File::~File() {}

bool File::Open(std::string_view path)
{
	m_readOnly = false;

	m_fullPath.reserve(MAX_PATH);
	auto m_filename_cstr = m_filename.data();
	GetFullPathName(path.data(), MAX_PATH, m_fullPath.data(), &m_filename_cstr);

	DWORD fileAttr = GetFileAttributes(path.data());
	if (fileAttr != INVALID_FILE_ATTRIBUTES)
		m_readOnly = (fileAttr & FILE_ATTRIBUTE_READONLY) != 0;

	DWORD access = GENERIC_READ;
	DWORD shareMode = FILE_SHARE_READ;
	if (!m_readOnly)
	{
		access |= GENERIC_WRITE;
		shareMode |= FILE_SHARE_DELETE | FILE_SHARE_WRITE;
	}

    m_handle = CreateFile(path.data(), access, shareMode, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (m_handle != INVALID_HANDLE_VALUE)
        m_filesize = GetFileSize(m_handle, 0);

    return m_handle != 0;
}

bool File::IsOpen() const
{
	return m_handle != INVALID_HANDLE_VALUE;
}

void File::Close()
{
    if (!m_handle)
        return;

    CloseHandle(m_handle);
}

void File::Seek(unsigned int position)
{
    if (!m_handle)
        return;

    LARGE_INTEGER distance;
    distance.QuadPart = position;
	if (!SetFilePointerEx(m_handle, distance, 0, FILE_BEGIN))
		Error("SetFilePointerEx failed");
}

void File::Read(void* buffer, unsigned int size)
{
    if (!m_handle)
        return;

    DWORD bytesRead;
    ReadFile(m_handle, buffer, size, &bytesRead, 0);
}

void File::Write(void* buffer, unsigned int size)
{
	if (!m_handle)
		return;

	DWORD bytesWritten;
	if (!WriteFile(m_handle, buffer, size, &bytesWritten, 0))
		Error("WriteFile failed");
}
