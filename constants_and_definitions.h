//
// Created by nanoatic on 24/09/17.
//

//
// Created by nanoatic on 24/09/17.
//

#ifndef FLOWOVER_GLOBAL_H
#define FLOWOVER_GLOBAL_H

#define SERVER_PORT 50000
#define MAX_BUFFER_SIZE 30000
#define APP_ID 0x30


// locations
#define BUFFER_LENGTH_LOCATION 0
#define ID_LOCATION 2
#define INS_LOCATION 3
#define TAG_LOCATION 4
#define ROW_INDEX_LOCATION 5
#define COLUMN_INDEX_LOCATION 7
#define MATRIX_MULTIPLICATION_LENGTH_LOCATION 9
#define MATRIX_MULTIPLICATION_BEGIN_OF_DATA_LOCATION 11
#define pizza
#define LENGTH_PADDING 12

#define LENGTH_PADDING_MANDATORY 5

#define  KEEP_ALIVE 3
// List of instructions
#define INS_MULTIPLY_AND_SUM_R_X_C 0x02

#define MAX_CLIENTS 4095


#define SERVER_IP "192.168.1.100"
typedef unsigned char byte;
#endif //FLOWOVER_GLOBAL_H
