//--------------------------------------------------------------------------//
/// Copyright (c) 2019 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rqt_pch.h>
#include <rqt/src/rqt_preprocessor.h>

namespace rqt {

void split(rtm_vector<rtm_string>& _tokens, const rtm_string& _text, char _sep)
{
	size_t start = 0, end = 0;
	while ((end = _text.find(_sep, start)) != rtm_string::npos)
	{
		_tokens.push_back(_text.substr(start, end - start));
		start = end + 1;
	}
	_tokens.push_back(_text.substr(start));
}

Preprocessor::Preprocessor(const rtm_string& _text)
{
	split(m_input, _text, '\n');
	m_inComment = false;
	m_enabled.push_back(true);
}

static bool isEnabled(const rtm_vector<bool>& _enabled)
{
	const size_t size = _enabled.size();
	if (size > 0)
		return _enabled[size - 1];
	else
		return true;
}

void Preprocessor::parse()
{
	const size_t lines = m_input.size();

	size_t idx = 0;
	while (idx<lines)
	{
		rtm_string& line = m_input[idx++];

		LineType::Enum dt = LineTypeType(line);
		if ((dt == LineType::None) && isEnabled(m_enabled))
		{
			if (!m_inComment)
				process(line);
		}
	}
}

bool strStartsWith(const char* _str, const char* _start, const char** _dst)
{
	const size_t len = strlen(_start);
	if (strncmp(_str, _start, len)==0)
	{
		*_dst = _str + len;
		while ((**_dst == ' ') || (**_dst == '\t')) ++*_dst;
		return true;
	}
	return false;
}

Preprocessor::LineType::Enum Preprocessor::LineTypeType(const rtm_string& _str)
{
	const char* src = _str.c_str();
	while ((*src == ' ') || (*src == '\t')) ++src;

	if (m_inComment)
	{
		if (strstr(src,"*/"))
			m_inComment = false;
		return LineType::CommentMultiLine;
	}

	if (*src == '#')
	{
		++src; // skip '#'
		while ((*src == ' ') || (*src == '\t')) ++src;

		const char* res = nullptr;
		if (strStartsWith(src, "define", &res))
		{
			addDefine(res);
			return LineType::Define;
		}

		if (strStartsWith(src, "ifdef", &res))
		{
			bool enabled = isDefined(res);
			m_enabled.push_back(enabled);
			return LineType::Ifdef;
		}
		
		if (strStartsWith(src, "endif", &res))
		{
			m_enabled.pop_back();
			return LineType::Endif;
		}
	}

	if ((src[0] == '/') && (src[1] == '/'))
		return LineType::CommentSingleLine;

	if ((src[0] == '/') && (src[1] == '*'))
	{
		m_inComment = true;
		return LineType::CommentMultiLine;
	}

	return LineType::None;
}

void Preprocessor::process(rtm_string& _str)
{
	const size_t numLineTypes = m_defines.size();
	for (size_t i=0; i<numLineTypes; ++i)
	{
		const char* define = m_defines[i].c_str();
		const char* pos = strstr(_str.c_str(), define);
		if (pos)
		{
			_str.replace(pos-_str.c_str(), strlen(define), m_defineValues[i].c_str());
		}
	}

	m_output += _str;
}

void Preprocessor::addDefine(const char* _define)
{
	if (!isEnabled(m_enabled))
		return;

	rtm_string define;
	rtm_string value;

	while ((*_define != ' ') && (*_define != '\t'))
	{
		define += *_define;
		++_define;
	};

	while (*_define && ((*_define == ' ') || (*_define == '\t'))) ++_define;
	if (*_define)
		value = _define;

	m_defines.push_back(define);
	m_defineValues.push_back(value);
}

bool Preprocessor::isDefined(const char* _LineType)
{
	const char* lastSpace = _LineType;
	while (*lastSpace && ((*lastSpace != ' ') && (*lastSpace != '\t'))) ++lastSpace;
	size_t numChars = lastSpace - _LineType;

	const size_t numLineTypes = m_defines.size();
	for (size_t i=0; i<numLineTypes; ++i)
	{
		if (strncmp(m_defines[i].c_str(), _LineType, numChars) == 0)
			return true;
	}

	return false;
}

} // namespace rqt
