#ifndef __Logger_h__
#define __Logger_h__

namespace Artemis::Utilities
{
	static constexpr unsigned int SEVERITY_NONE  = 0;
	static constexpr unsigned int SEVERITY_INFO  = 1;
	static constexpr unsigned int SEVERITY_WARN  = SEVERITY_INFO << 1;
	static constexpr unsigned int SEVERITY_ERR   = SEVERITY_INFO << 2;
	static constexpr unsigned int SEVERITY_FATAL = SEVERITY_INFO << 3;

	static constexpr unsigned int CATEGORY_NONE      = 0;
	static constexpr unsigned int CATEGORY_APP       = 1;
	static constexpr unsigned int CATEGORY_RENDERER  = CATEGORY_APP << 1;
	static constexpr unsigned int CATEGORY_UTILITIES = CATEGORY_APP << 2;
	static constexpr unsigned int CATEGORY_SHADERS   = CATEGORY_APP << 3;

	class Logger
	{
	public:
		static void SetSeverity( unsigned int _severity = SEVERITY_INFO );
		static void SetCategory( unsigned int _category = CATEGORY_NONE );

		static void Log( unsigned int _severity, unsigned int _category, const char* _pFormat, ... );

	private:
		static unsigned int m_sActiveSeverities;
		static unsigned int m_sActiveCategory;
	};
}

#endif // __Logger_h__
