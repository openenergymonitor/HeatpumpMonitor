
/*
 Arduino library to read data from an Elster A100C electricity meter.

 Copyright (C) 2012 Dave Berkeley projects@rotwang.co.uk

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 USA
*/

#define BUFF_SIZE 128

struct Buffer
{
  int data[BUFF_SIZE];
  int in;
  int out;
};

static void buff_init(struct Buffer* b)
{
  b->in = b->out = 0;
}

static int buff_full(struct Buffer* b)
{
  int next = b->in + 1;
  if (next >= BUFF_SIZE)
    next = 0;
  return next == b->out;
}

static int buff_add(struct Buffer* b, int d)
{
  int next = b->in + 1;
  if (next >= BUFF_SIZE)
    next = 0;
  if (next == b->out)
    return -1; // overfow error

  b->data[b->in] = d;
  b->in = next;  
  return 0;  
}

static int buff_get(struct Buffer* b, int* d)
{
  if (b->in == b->out)
    return -1;
  int next = b->out + 1;
  if (next >= BUFF_SIZE)
    next = 0;
  *d = b->data[b->out];
  b->out = next;
  return 0;  
}

  /*
   *
   */

typedef void (*on_reading)(unsigned long reading);

class ElsterA100C
{
    struct Buffer bits;
    struct Buffer bytes;
    int bit_data;
    int bit_index;

    // see Appendix B of the product manual
    struct info {
        char product[12]; // eg. "Elster A100C..."
        char firmware[9];
        unsigned char mfg_serial[3];
        unsigned char config_serial[2];
        char utility_serial[16];
        unsigned char meter_definition[3];
        unsigned char rate_1_import_kWh[5]; // the reading we are interested in!
        unsigned char rate_1_reserved[5];
        unsigned char rate_1_reverse_kWh[5];
        unsigned char rate_2_import_kWh[5];
        unsigned char rate_2_reserved_kWh[5];
        unsigned char rate_2_reverse_kWh[5];
        unsigned char reserved_01[1];
        unsigned char status;
        unsigned char error;
        unsigned char anti_creep[3];
        unsigned char rate_1_time[3];
        unsigned char rate_2_time[3];
        unsigned char power_up[3];
        unsigned char power_fail[2];
        unsigned char watchdog;
        unsigned char reverse_warning;
        unsigned char reserved_02[10];
    };

    unsigned char data[sizeof(info)];
    unsigned int idx;
    int reading;
    unsigned char last_4[4];
    on_reading handler;

    int add_bit(int state);
    void on_bits(int bits);
    void on_timeout();
    int good_cs(unsigned char cs, unsigned char check);
    void good_packet();
    unsigned char bcc(unsigned char cs, const unsigned char* data, int count);

public:
    ElsterA100C(on_reading cb)
    : idx(0), reading(0), handler(cb)
    {
    }

    void init(int irq);
    void on_change(void);
    void on_data(unsigned char c);
    int decode_bit_stream(void);
};

// FIN
