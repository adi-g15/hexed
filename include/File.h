#pragma once

#include <string_view>
#include <string>

class File
{
    public:
        File();
		~File();

        bool Open(std::string_view path);
        void Close();
        unsigned int GetSize() const;
        void Seek(unsigned int position);
        void Read(void* buffer, unsigned int size);
		void Write(void* buffer, unsigned int size);
        bool IsOpen() const;
		bool IsReadOnly() const;
		const std::string& GetFullPath() const;
		const std::string& GetFileName() const;

    private:
        std::string m_filename;
		std::string m_fullPath;
        unsigned int m_filesize;
        void* m_handle;
		bool m_readOnly;
};

inline unsigned int File::GetSize() const
{
    return m_filesize;
}

inline bool File::IsReadOnly() const
{
	return m_readOnly;
}

inline const std::string& File::GetFullPath() const
{
	return m_fullPath;
}

inline const std::string& File::GetFileName() const
{
	return m_filename;
}
