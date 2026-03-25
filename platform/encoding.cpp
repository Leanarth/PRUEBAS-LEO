#include "encoding.hpp"
#include <cstdint>  // Llama a la librería de C relacionado a tipos de enteros de tamaño muy específico, esto para la traducción correcta de bytes entre distintas codificaciones
#include <stdexcept>  // Llama a la librería stdexcept para manejar excepciones
#include <string>  // Llama a la librería string, para poder declarar las variables de tipo string, u32string, etc

/*
  Estas tres funciones sinceramente fueron hechas con ayuda de IA, no tengo idea de como traducir las codificaciones, y lo intenté a mi forma pero era muy poco
  óptima en su momento, entonces decidí recurrir a la IA, preferiblemente recomendaría ayuda de inteligencia artificial para el entendimiento de este código
  ⚰️
*/

std::u32string UTF8ToUTF32(const std::string& utf8)
{
  std::u32string result;
  size_t i = 0;
  while (i < utf8.size())
  {
    uint32_t codepoint = 0;
    unsigned char c = static_cast<unsigned char>(utf8[i]);
    // 1 byte (ASCII)
    if (c <= 0x7F)
    {
        codepoint = c;
        i += 1;
    }
    // 2 bytes
    else if ((c & 0xE0) == 0xC0)
    {
      if (i + 1 >= utf8.size()) {
        throw std::runtime_error("Truncated UTF-8 sequence");
      }
      unsigned char c1 = static_cast<unsigned char>(utf8[i+1]);
      if ((c1 & 0xC0) != 0x80) {
        throw std::runtime_error("Invalid UTF-8 continuation byte");
      }
      codepoint = ((c & 0x1F) << 6) | (c1 & 0x3F);
      if (codepoint < 0x80) {
        throw std::runtime_error("Overlong UTF-8 sequence");
      }
      i += 2;
    }
    // 3 bytes
    else if ((c & 0xF0) == 0xE0)
    {
      if (i + 2 >= utf8.size()) {
        throw std::runtime_error("Truncated UTF-8 sequence");
      }
      unsigned char c1 = static_cast<unsigned char>(utf8[i+1]);
      unsigned char c2 = static_cast<unsigned char>(utf8[i+2]);
      if ((c1 & 0xC0) != 0x80 || (c2 & 0xC0) != 0x80) {
        throw std::runtime_error("Invalid UTF-8 continuation byte");
      }
      codepoint = ((c & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
      if (codepoint < 0x800) {
        throw std::runtime_error("Overlong UTF-8 sequence");
      }
      if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
        throw std::runtime_error("Invalid UTF-8 surrogate");
      }
      i += 3;
    }
    // 4 bytes
    else if ((c & 0xF8) == 0xF0)
    {
      if (i + 3 >= utf8.size()) {
        throw std::runtime_error("Truncated UTF-8 sequence");
      }
      unsigned char c1 = static_cast<unsigned char>(utf8[i+1]);
      unsigned char c2 = static_cast<unsigned char>(utf8[i+2]);
      unsigned char c3 = static_cast<unsigned char>(utf8[i+3]);
      if ((c1 & 0xC0) != 0x80 || (c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80) {
        throw std::runtime_error("Invalid UTF-8 continuation byte");
      }
      codepoint = ((c & 0x07) << 18) | ((c1 & 0x3F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
      if (codepoint < 0x10000 || codepoint > 0x10FFFF) {
        throw std::runtime_error("Invalid UTF-8 4-byte sequence");
      }
      i += 4;
    }
    else
    {
      throw std::runtime_error("Invalid UTF-8 leading byte");
    }
  result.push_back(static_cast<char32_t>(codepoint));
  }
  return result;
}

std::string UTF32ToUTF8(const std::u32string& input)
{
  std::string output;
  for (char32_t ch : input)
  {
    if (ch <= 0x7F)
    {
      output.push_back((char)ch);
    }
    else if (ch <= 0x7FF)
    {
      output.push_back((char)(0xC0 | ((ch >> 6) & 0x1F)));
      output.push_back((char)(0x80 | (ch & 0x3F)));
    }
    else if (ch <= 0xFFFF)
    {
      output.push_back((char)(0xE0 | ((ch >> 12) & 0x0F)));
      output.push_back((char)(0x80 | ((ch >> 6) & 0x3F)));
      output.push_back((char)(0x80 | (ch & 0x3F)));
    }
    else
    {
      output.push_back((char)(0xF0 | ((ch >> 18) & 0x07)));
      output.push_back((char)(0x80 | ((ch >> 12) & 0x3F)));
      output.push_back((char)(0x80 | ((ch >> 6) & 0x3F)));
      output.push_back((char)(0x80 | (ch & 0x3F)));
    }
  }
  return output;
}

std::u32string UTF16ToUTF32(const std::u16string& input)
{
  std::u32string output;
  output.reserve(input.size());
  for (size_t i = 0; i < input.size(); ++i)
  {
    char16_t wc = input[i];
    // High surrogate
    if (wc >= 0xD800 && wc <= 0xDBFF)
    {
      if (i + 1 < input.size())
      {
        char16_t wc2 = input[i + 1];
        if (wc2 >= 0xDC00 && wc2 <= 0xDFFF)
        {
          char32_t codepoint =
            ((static_cast<char32_t>(wc - 0xD800) << 10) | (wc2 - 0xDC00)) + 0x10000;
          output.push_back(codepoint);
          ++i;
        }
        else
        {
          // High surrogate sin low surrogate valido
          output.push_back(0xFFFD);
        }
      }
      else
      {
        // High surrogate al final del string
        output.push_back(0xFFFD);
      }
    }
    // Low surrogate sin high surrogate previo
    else if (wc >= 0xDC00 && wc <= 0xDFFF)
    {
      output.push_back(0xFFFD);
    }
    else
    {
      output.push_back(static_cast<char32_t>(wc));
    }
  }
  return output;
}
