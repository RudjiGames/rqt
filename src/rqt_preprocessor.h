//--------------------------------------------------------------------------//
/// Copyright 2024 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RQT_PREPROCESSOR_H
#define RTM_RQT_PREPROCESSOR_H

namespace rqt {

class Preprocessor
{
	std::vector<bool>			m_enabled;
	std::vector<std::string>	m_input;
	std::vector<std::string>	m_defines;
	std::vector<std::string>	m_defineValues;
	bool						m_inComment;
	std::string					m_output;

	struct LineType
	{
		enum Enum
		{
			Define,
			Ifdef,
			Endif,
			CommentSingleLine,
			CommentMultiLine,
			None
		};
	};

public:
	Preprocessor(const std::string& _text);

	void parse();
	LineType::Enum LineTypeType(const std::string& _str);
	void process(std::string& _str);
	void addDefine(const char* _define);
	bool isDefined(const char* _LineType);
	const char* getOutput() const { return m_output.c_str(); }
};

} // namespace rqt

#endif // RTM_RQT_PREPROCESSOR_H
