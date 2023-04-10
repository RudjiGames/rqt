//--------------------------------------------------------------------------//
/// Copyright (c) 2019 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RQT_PREPROCESSOR_H
#define RTM_RQT_PREPROCESSOR_H

namespace rqt {

class Preprocessor
{
	rtm_vector<bool>			m_enabled;
	rtm_vector<rtm_string>		m_input;
	rtm_vector<rtm_string>		m_defines;
	rtm_vector<rtm_string>		m_defineValues;
	bool						m_inComment;
	rtm_string					m_output;

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
	Preprocessor(const rtm_string& _text);

	void parse();
	LineType::Enum LineTypeType(const rtm_string& _str);
	void process(rtm_string& _str);
	void addDefine(const char* _define);
	bool isDefined(const char* _LineType);
	const char* getOutput() const { return m_output.c_str(); }
};

} // namespace rqt

#endif // RTM_RQT_PREPROCESSOR_H
