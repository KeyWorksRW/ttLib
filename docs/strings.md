# **ttLib** string classes

**ttLib** provides two classes for handling UTF8 strings, two classes for handling wide-character (unicode) strings, and one class for handling OLE strings.

## ttStr

This is a header-only class designed for a fixed-length string, or a fixed-length buffer that will contain a string. The buffer size will not change unless you specifically tell it to (by calling **resize**()).

## ttString

This class is designed for strings whose length will change during the lifetime of the string. The methods for this class will automatically reallocate the buffer holding the string to match it's current length requirements.

## ttWStr

This class is identical to the **ttStr** class except that it uses wide-character (**wchar_t**) strings.

## ttWString

This class is identical to the **ttString** class except that it uses wide-character (**wchar_t**) strings.

## ttBSTR

This class is designed to be used for OLE strings and is only available if you are building for Windows. Memory allocation is done using [SysAllocString](https://docs.microsoft.com/en-us/windows/desktop/api/oleauto/nf-oleauto-sysallocstring), and the destructor will free the string using [SysFreeString](https://docs.microsoft.com/en-us/windows/desktop/api/oleauto/nf-oleauto-sysfreestring).
