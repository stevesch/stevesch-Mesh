#ifndef STEVESCH_RENDER_MESHIMPORT_TOKENIZER_H_
#define STEVESCH_RENDER_MESHIMPORT_TOKENIZER_H_
//#include <c_types.h>

//#include <string>

// Example usage:
//
// Tokenizer s;
// s.setDelimiters(" \n\r");
// s.begin("split this string");
// while (s.nextToken())
// {
//		// use s.getToken()
// }

#include <Arduino.h>
//#include <FS.h>
#include <Stream.h>

namespace stevesch
{
  typedef String tokenstr_t;

  class Tokenizer
  {
  public:
    Tokenizer();

    void setDelimiters(const tokenstr_t &delimiters, bool bTrimLeadingDelims = true);
    void begin(Stream &f);

    void resetEol() { eol = false; }
    void finishLine(); // remove from stream until eol (or eof)

    bool nextToken();
    bool nextToken(const tokenstr_t &delimiters);
    const tokenstr_t getToken() const { return m_token; }

  protected:
    Stream *_f;
    bool eol;
    bool mbTrimLeadingDelims;

    tokenstr_t m_token;
    tokenstr_t m_delimiters;
  };
}
#endif
