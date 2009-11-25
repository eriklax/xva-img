#ifndef _XVA_DISK_HPP_
#define _XVA_DISK_HPP_

#include <string>
#include <stdexcept>

namespace XVA {
	class Disk
	{
		public:
			Disk(const std::string& path);
			bool Export(const std::string& diskpath)
				throw (std::runtime_error);
			bool Import(const std::string& diskpath)
				throw (std::runtime_error);
		private:
			std::string m_path;
	};
}

#endif
