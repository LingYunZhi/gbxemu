#pragma once

#include <mmreg.h>

class WavWriter
{
 private:
  FILE *m_file;
  int m_len;
  long m_posSize;

 public:
  WavWriter();
  ~WavWriter();

  bool Open(const char *name);
  void SetFormat(const WAVEFORMATEX *format);
  void AddSound(const u8 *data, int len);

 private:
  void Close();
};
