#ifndef LOADAPNG_H
#define LOADAPNG_H

#include <QIODevice>
#include <vector>

struct APNGFrame { unsigned char * p, ** rows; unsigned int w, h, delay_num, delay_den; };

int load_apng(QIODevice *device, std::vector<APNGFrame>& frames);

#endif // LOADAPNG_H
