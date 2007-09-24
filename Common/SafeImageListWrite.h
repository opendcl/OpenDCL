#pragma once

//define SafeImageListWrite that always writes the "old-style" (pre CommCtrl v 6) format
BOOL SafeImageListWrite(HIMAGELIST himl, LPSTREAM pstm);
