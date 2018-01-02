//
// Created by alaeddine on 10/21/17.
//

#ifndef FLOWOVER_HELPER_CLASS_H
#define FLOWOVER_HELPER_CLASS_H

#include "includes.h"
#include "constants_and_definitions.h"


class helper_class {
public :
static void display_error_syncronized(std::string message);
    static void display_info_syncronized(std::string message);

    template<typename T>   static T array_to_elem( byte *array, int offset){
        union {
            T elem;
            byte buffer[sizeof(T)];
        }converter;
        for (int i = 0; i < sizeof(T) ; ++i) {
            converter.buffer[i]=array[i+offset];
        }
        return converter.elem;
    };
  static  void display_info_syncronized_ln(std::string message="");

   static void display_error_syncronized_ln(std::string message);

    static vector<byte> buffer_to_array(const byte [],size_t);
    static double array_to_double(const byte * array, int offset);
    static byte * double_to_array(double value);
    static ssize_t send_all(int socket, const void *data, size_t len)
    {
        while (len > 0)
        {
            ssize_t r = send(socket, data, len, 0);
            if (r < 0)
            {

                    return  -1;

            }
            len -= r;
            data = (const char*)data + r;
        }
        return len;
    }
    static void ReadXBytes(int socket, unsigned int x, void* buffer)
    {
        int bytesRead = 0;
        int result;
        while (bytesRead < x)
        {
            result = read(socket, buffer + bytesRead, x - bytesRead);
            if (result < 1 )
            {
                // Throw your error.
            }

            bytesRead += result;
        }
    }
};


#endif //FLOWOVER_HELPER_CLASS_H
