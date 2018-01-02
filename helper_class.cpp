//
// Created by alaeddine on 10/21/17.
//

#include "helper_class.h"

void helper_class::display_info_syncronized_ln(std::string message) {
    std::stringstream stream;
    stream << message << "\n";
    std::cout << stream.str();
}

void helper_class::display_error_syncronized_ln(std::string message) {
    std::stringstream stream;
    stream << message << "\n";
    std::cerr << stream.str();
}

void helper_class::display_info_syncronized(std::string message) {
    std::stringstream stream;
    stream << message;
    std::cout << stream.str();
}

void helper_class::display_error_syncronized(std::string message) {
    std::stringstream stream;
    stream << message;
    std::cerr << stream.str();
}

vector<byte> helper_class::buffer_to_array(const byte *buffer,size_t size) {
    vector<byte> array_(size);
    for (int i = 0; i < size; i++) {
        array_[i] = buffer[i];
    }
    return array_;

}



double helper_class::array_to_double(const byte *array, int offset=0) {

    double *retp = nullptr;
    retp= new double;
    memcpy(retp,array+offset, sizeof(double));
    double ret=*retp;
    delete retp;
    return ret;
}

byte *helper_class::double_to_array(double value) {
    byte * array = static_cast<byte *>(malloc(sizeof(double)));
    memcpy(array,&value,sizeof(double));
    return array;
}


