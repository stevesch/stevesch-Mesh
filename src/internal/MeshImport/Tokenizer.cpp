#include "Tokenizer.h"

#ifndef ICACHE_FLASH_ATTR
#define ICACHE_FLASH_ATTR
#endif

namespace stevesch
{

  namespace
  {
    const tokenstr_t kDefaultDelimiters(" \t\n\r");
  }

  Tokenizer::Tokenizer() : _f(nullptr),
                           eol(false),
                           mbTrimLeadingDelims(true),
                           m_delimiters(kDefaultDelimiters)
  {
  }

  void ICACHE_FLASH_ATTR Tokenizer::setDelimiters(const tokenstr_t &delimiters, bool bTrimLeadingDelims)
  {
    m_delimiters = delimiters;
    mbTrimLeadingDelims = bTrimLeadingDelims;
  }

  void ICACHE_FLASH_ATTR Tokenizer::begin(Stream &f)
  {
    //m_string = str;
    //m_offset = 0;
    _f = &f;
    resetEol();
  }

  bool ICACHE_FLASH_ATTR Tokenizer::nextToken()
  {
    return nextToken(m_delimiters);
  }

  /*
namespace {
	constexpr int str_npos = -1;
}
int ICACHE_FLASH_ATTR find_first_not_of(const String& str, const String& delims, size_t fromIndex)
{
	size_t end = str.length();
	size_t delimCount = delims.length();
	for (size_t i = fromIndex; i < end; ++i)
	{
		char ch = str[i];
		int j;
		for (j = 0; j < delimCount; ++j)
		{
			if (ch == delims[j])
			{
				break;	// found delimiter (break here, but continue in outer loop)
			}
		}
		if (j == delimCount)
		{
			return i;	// found non-delimiter
		}
	}
	return str_npos;
}

int ICACHE_FLASH_ATTR find_first_of(const String& str, const String& delims, size_t fromIndex)
{
	size_t end = str.length();
	size_t delimCount = delims.length();
	for (size_t i = fromIndex; i < end; ++i)
	{
		char ch = str[i];
		if (delims.indexOf(ch) >= 0)
		{
			return i;	// found delimiter
		}
	}
	return str_npos;
}
*/

  // reads characters preceding delimiter into dst.  If delimiter is encountered, it is removed from the stream.
  bool ICACHE_FLASH_ATTR readUntilEolOrDelim(Stream &f, tokenstr_t &dst, const tokenstr_t &delimiters)
  {
    dst = "";
    while (f.available())
    {
      char ch = (char)f.read();
      if (ch == '\n')
      {
        return true; // eol
      }

      if (delimiters.indexOf(ch) >= 0)
      {
        return false;
      }

      dst += ch;
    }

    return true; // !available -> eol
  }

  // advance to first character that is not a delimiter.  stops at (and consumes) newline.
  bool ICACHE_FLASH_ATTR find_first_not_of_on_line(Stream &f, const tokenstr_t &delimiters)
  {
    while (f.available())
    {
      char ch = (char)f.peek();
      if (delimiters.indexOf(ch) < 0)
      {
        // not a delimiter-- leave character in stream and return
        return false; // not eol/eof
      }

      // delimiter-- advance past it
      f.read();
      if (ch == '\n')
      {
        // if it's a newline, stop
        return true; // eol
      }
    }
    return true; // eol/eof
  }

  void ICACHE_FLASH_ATTR Tokenizer::finishLine()
  {
    if (_f && !eol)
    {
      while (_f->available())
      {
        char ch = (char)_f->read();
        if (ch == '\n')
        {
          break;
        }
      }
    }
    eol = true;
  }

  bool ICACHE_FLASH_ATTR Tokenizer::nextToken(const tokenstr_t &delimiters)
  {
    /*
	size_t i = m_offset;
	//size_t i = m_string.find_first_not_of(delimiters, m_offset);
	//if (tokenstr_t::npos == i)
	if (mbTrimLeadingDelims)
	{
		i = find_first_not_of(m_string, delimiters, i);
		if (str_npos == i)
		{
			m_offset = m_string.length();
			return false;
		}
	}
*/
    //Serial.printf("  0 f pos=%d\n", _f->position());

    m_token = "";
    if (eol)
    {
      return false;
    }

    if (mbTrimLeadingDelims)
    {
      if (find_first_not_of_on_line(*_f, delimiters))
      {
        eol = true;
        return false;
      }
    }
    //Serial.printf("  1 f pos=%d\n", _f->position());
    /*
	//size_t j = m_string.find_first_of(delimiters, i);
	//if (tokenstr_t::npos == j)
	int j = find_first_of(m_string, delimiters, i);
	if (str_npos == j)
	{
		//m_token = m_string.substr(i);
		m_token = m_string.substring(i);
		m_offset = m_string.length();
		return true;
	}

	//m_token = m_string.substr(i, j - i);
	m_token = m_string.substring(i, j);
	m_offset = j + 1;
*/

    if (readUntilEolOrDelim(*_f, m_token, delimiters))
    {
      eol = true;
    }

    //Serial.printf("  2 f pos=%d\n", _f->position());

    if (eol && (m_token.length() == 0))
    {
      return false;
    }

    return true;
  }

}
