/* vim: set tabstop=4 : */
#if defined(_MSC_VER)

#include "MfcFileStream.hpp"
#include "byte_io_impl.hpp"

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma warning(push)
# pragma warning(disable: 4267)
#endif

namespace nark {

	size_t MfcFileStream::read(void* vbuf, size_t length)
		{
			return m_fp->Read(vbuf, length);
		}
		size_t MfcFileStream::write(const void* vbuf, size_t length)
		{
			TRY
				m_fp->Write(vbuf, length);
			CATCH(CException, e)
				throw OutOfSpaceException("MfcFileStream::Write");
			END_CATCH
			return length;
		}
		bool MfcFileStream::seek(stream_offset_t offset, int origin)
		{
			TRY
				m_fp->Seek(offset, origin);
			CATCH(CException, e)
				throw OutOfSpaceException("MfcFileStream::Write");
			END_CATCH
			return true;
		}
		void MfcFileStream::flush()
		{
			m_fp->Flush();
		}

		FEBIRD_GEN_ensureRead (MfcFileStream::)
		FEBIRD_GEN_ensureWrite(MfcFileStream::)
		FEBIRD_GEN_getByte(MfcFileStream::)
		FEBIRD_GEN_readByte(MfcFileStream::)
		FEBIRD_GEN_writeByte(MfcFileStream::)

		size_t MfcArchiveStream::read(void* vbuf, size_t length)
		{
			return m_fp->Read(vbuf, length);
		}
		size_t MfcArchiveStream::write(const void* vbuf, size_t length)
		{
			TRY
				m_fp->Write(vbuf, length);
			CATCH(CException, e)
				throw OutOfSpaceException("MfcFileStream::Write");
			END_CATCH
			return length;
		}
		void MfcArchiveStream::flush() { m_fp->Flush(); }

		FEBIRD_GEN_ensureRead (MfcArchiveStream::)
		FEBIRD_GEN_ensureWrite(MfcArchiveStream::)
		FEBIRD_GEN_getByte(MfcArchiveStream::)
		FEBIRD_GEN_readByte(MfcArchiveStream::)
		FEBIRD_GEN_writeByte(MfcArchiveStream::)
}

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma warning(pop)
#endif

#endif
