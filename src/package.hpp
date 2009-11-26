#ifndef _XVA_PACKAGE_HPP_
#define _XVA_PACKAGE_HPP_

#include <string>
#include <list>
#include <stdexcept>

namespace XVA
{
	class XVAPackage
	{
		public:
			XVAPackage();
			void AddFile(const std::string& file);
			void AddDir(const std::string& path)
				throw (std::runtime_error);

			bool Write(const std::string& file)
				throw (std::runtime_error);

			void Verbose();
		private:
			std::list<std::string> m_files;
			bool m_verbose;
	};
}

#endif
