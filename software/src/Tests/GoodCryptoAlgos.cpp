#include "testbench.hpp"
#include "unitConfiguration.hpp"

const uint8_t sbox[256] = {
   0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
   0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
   0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
   0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
   0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
   0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
   0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
   0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
   0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
   0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
   0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
   0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
   0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
   0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
   0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
   0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

const uint8_t mul2[] = {
   0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x16,0x18,0x1a,0x1c,0x1e,
   0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e,
   0x40,0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e,
   0x60,0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,0x70,0x72,0x74,0x76,0x78,0x7a,0x7c,0x7e,
   0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0x8e,0x90,0x92,0x94,0x96,0x98,0x9a,0x9c,0x9e,
   0xa0,0xa2,0xa4,0xa6,0xa8,0xaa,0xac,0xae,0xb0,0xb2,0xb4,0xb6,0xb8,0xba,0xbc,0xbe,
   0xc0,0xc2,0xc4,0xc6,0xc8,0xca,0xcc,0xce,0xd0,0xd2,0xd4,0xd6,0xd8,0xda,0xdc,0xde,
   0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xee,0xf0,0xf2,0xf4,0xf6,0xf8,0xfa,0xfc,0xfe,
   0x1b,0x19,0x1f,0x1d,0x13,0x11,0x17,0x15,0x0b,0x09,0x0f,0x0d,0x03,0x01,0x07,0x05,
   0x3b,0x39,0x3f,0x3d,0x33,0x31,0x37,0x35,0x2b,0x29,0x2f,0x2d,0x23,0x21,0x27,0x25,
   0x5b,0x59,0x5f,0x5d,0x53,0x51,0x57,0x55,0x4b,0x49,0x4f,0x4d,0x43,0x41,0x47,0x45,
   0x7b,0x79,0x7f,0x7d,0x73,0x71,0x77,0x75,0x6b,0x69,0x6f,0x6d,0x63,0x61,0x67,0x65,
   0x9b,0x99,0x9f,0x9d,0x93,0x91,0x97,0x95,0x8b,0x89,0x8f,0x8d,0x83,0x81,0x87,0x85,
   0xbb,0xb9,0xbf,0xbd,0xb3,0xb1,0xb7,0xb5,0xab,0xa9,0xaf,0xad,0xa3,0xa1,0xa7,0xa5,
   0xdb,0xd9,0xdf,0xdd,0xd3,0xd1,0xd7,0xd5,0xcb,0xc9,0xcf,0xcd,0xc3,0xc1,0xc7,0xc5,
   0xfb,0xf9,0xff,0xfd,0xf3,0xf1,0xf7,0xf5,0xeb,0xe9,0xef,0xed,0xe3,0xe1,0xe7,0xe5
};

const uint8_t mul3[] = {
   0x00,0x03,0x06,0x05,0x0c,0x0f,0x0a,0x09,0x18,0x1b,0x1e,0x1d,0x14,0x17,0x12,0x11,
   0x30,0x33,0x36,0x35,0x3c,0x3f,0x3a,0x39,0x28,0x2b,0x2e,0x2d,0x24,0x27,0x22,0x21,
   0x60,0x63,0x66,0x65,0x6c,0x6f,0x6a,0x69,0x78,0x7b,0x7e,0x7d,0x74,0x77,0x72,0x71,
   0x50,0x53,0x56,0x55,0x5c,0x5f,0x5a,0x59,0x48,0x4b,0x4e,0x4d,0x44,0x47,0x42,0x41,
   0xc0,0xc3,0xc6,0xc5,0xcc,0xcf,0xca,0xc9,0xd8,0xdb,0xde,0xdd,0xd4,0xd7,0xd2,0xd1,
   0xf0,0xf3,0xf6,0xf5,0xfc,0xff,0xfa,0xf9,0xe8,0xeb,0xee,0xed,0xe4,0xe7,0xe2,0xe1,
   0xa0,0xa3,0xa6,0xa5,0xac,0xaf,0xaa,0xa9,0xb8,0xbb,0xbe,0xbd,0xb4,0xb7,0xb2,0xb1,
   0x90,0x93,0x96,0x95,0x9c,0x9f,0x9a,0x99,0x88,0x8b,0x8e,0x8d,0x84,0x87,0x82,0x81,
   0x9b,0x98,0x9d,0x9e,0x97,0x94,0x91,0x92,0x83,0x80,0x85,0x86,0x8f,0x8c,0x89,0x8a,
   0xab,0xa8,0xad,0xae,0xa7,0xa4,0xa1,0xa2,0xb3,0xb0,0xb5,0xb6,0xbf,0xbc,0xb9,0xba,
   0xfb,0xf8,0xfd,0xfe,0xf7,0xf4,0xf1,0xf2,0xe3,0xe0,0xe5,0xe6,0xef,0xec,0xe9,0xea,
   0xcb,0xc8,0xcd,0xce,0xc7,0xc4,0xc1,0xc2,0xd3,0xd0,0xd5,0xd6,0xdf,0xdc,0xd9,0xda,
   0x5b,0x58,0x5d,0x5e,0x57,0x54,0x51,0x52,0x43,0x40,0x45,0x46,0x4f,0x4c,0x49,0x4a,
   0x6b,0x68,0x6d,0x6e,0x67,0x64,0x61,0x62,0x73,0x70,0x75,0x76,0x7f,0x7c,0x79,0x7a,
   0x3b,0x38,0x3d,0x3e,0x37,0x34,0x31,0x32,0x23,0x20,0x25,0x26,0x2f,0x2c,0x29,0x2a,
   0x0b,0x08,0x0d,0x0e,0x07,0x04,0x01,0x02,0x13,0x10,0x15,0x16,0x1f,0x1c,0x19,0x1a
};

const uint8_t mul9[] = {
   0x00,0x09,0x12,0x1b,0x24,0x2d,0x36,0x3f,0x48,0x41,0x5a,0x53,0x6c,0x65,0x7e,0x77,
   0x90,0x99,0x82,0x8b,0xb4,0xbd,0xa6,0xaf,0xd8,0xd1,0xca,0xc3,0xfc,0xf5,0xee,0xe7,
   0x3b,0x32,0x29,0x20,0x1f,0x16,0x0d,0x04,0x73,0x7a,0x61,0x68,0x57,0x5e,0x45,0x4c,
   0xab,0xa2,0xb9,0xb0,0x8f,0x86,0x9d,0x94,0xe3,0xea,0xf1,0xf8,0xc7,0xce,0xd5,0xdc,
   0x76,0x7f,0x64,0x6d,0x52,0x5b,0x40,0x49,0x3e,0x37,0x2c,0x25,0x1a,0x13,0x08,0x01,
   0xe6,0xef,0xf4,0xfd,0xc2,0xcb,0xd0,0xd9,0xae,0xa7,0xbc,0xb5,0x8a,0x83,0x98,0x91,
   0x4d,0x44,0x5f,0x56,0x69,0x60,0x7b,0x72,0x05,0x0c,0x17,0x1e,0x21,0x28,0x33,0x3a,
   0xdd,0xd4,0xcf,0xc6,0xf9,0xf0,0xeb,0xe2,0x95,0x9c,0x87,0x8e,0xb1,0xb8,0xa3,0xaa, 
   0xec,0xe5,0xfe,0xf7,0xc8,0xc1,0xda,0xd3,0xa4,0xad,0xb6,0xbf,0x80,0x89,0x92,0x9b, 
   0x7c,0x75,0x6e,0x67,0x58,0x51,0x4a,0x43,0x34,0x3d,0x26,0x2f,0x10,0x19,0x02,0x0b,
   0xd7,0xde,0xc5,0xcc,0xf3,0xfa,0xe1,0xe8,0x9f,0x96,0x8d,0x84,0xbb,0xb2,0xa9,0xa0,
   0x47,0x4e,0x55,0x5c,0x63,0x6a,0x71,0x78,0x0f,0x06,0x1d,0x14,0x2b,0x22,0x39,0x30,
   0x9a,0x93,0x88,0x81,0xbe,0xb7,0xac,0xa5,0xd2,0xdb,0xc0,0xc9,0xf6,0xff,0xe4,0xed,
   0x0a,0x03,0x18,0x11,0x2e,0x27,0x3c,0x35,0x42,0x4b,0x50,0x59,0x66,0x6f,0x74,0x7d, 
   0xa1,0xa8,0xb3,0xba,0x85,0x8c,0x97,0x9e,0xe9,0xe0,0xfb,0xf2,0xcd,0xc4,0xdf,0xd6,
   0x31,0x38,0x23,0x2a,0x15,0x1c,0x07,0x0e,0x79,0x70,0x6b,0x62,0x5d,0x54,0x4f,0x46
};

const uint8_t mul11[] = {
   0x00,0x0b,0x16,0x1d,0x2c,0x27,0x3a,0x31,0x58,0x53,0x4e,0x45,0x74,0x7f,0x62,0x69,
   0xb0,0xbb,0xa6,0xad,0x9c,0x97,0x8a,0x81,0xe8,0xe3,0xfe,0xf5,0xc4,0xcf,0xd2,0xd9,
   0x7b,0x70,0x6d,0x66,0x57,0x5c,0x41,0x4a,0x23,0x28,0x35,0x3e,0x0f,0x04,0x19,0x12,
   0xcb,0xc0,0xdd,0xd6,0xe7,0xec,0xf1,0xfa,0x93,0x98,0x85,0x8e,0xbf,0xb4,0xa9,0xa2,
   0xf6,0xfd,0xe0,0xeb,0xda,0xd1,0xcc,0xc7,0xae,0xa5,0xb8,0xb3,0x82,0x89,0x94,0x9f,
   0x46,0x4d,0x50,0x5b,0x6a,0x61,0x7c,0x77,0x1e,0x15,0x08,0x03,0x32,0x39,0x24,0x2f,
   0x8d,0x86,0x9b,0x90,0xa1,0xaa,0xb7,0xbc,0xd5,0xde,0xc3,0xc8,0xf9,0xf2,0xef,0xe4,
   0x3d,0x36,0x2b,0x20,0x11,0x1a,0x07,0x0c,0x65,0x6e,0x73,0x78,0x49,0x42,0x5f,0x54,
   0xf7,0xfc,0xe1,0xea,0xdb,0xd0,0xcd,0xc6,0xaf,0xa4,0xb9,0xb2,0x83,0x88,0x95,0x9e,
   0x47,0x4c,0x51,0x5a,0x6b,0x60,0x7d,0x76,0x1f,0x14,0x09,0x02,0x33,0x38,0x25,0x2e,
   0x8c,0x87,0x9a,0x91,0xa0,0xab,0xb6,0xbd,0xd4,0xdf,0xc2,0xc9,0xf8,0xf3,0xee,0xe5,
   0x3c,0x37,0x2a,0x21,0x10,0x1b,0x06,0x0d,0x64,0x6f,0x72,0x79,0x48,0x43,0x5e,0x55,
   0x01,0x0a,0x17,0x1c,0x2d,0x26,0x3b,0x30,0x59,0x52,0x4f,0x44,0x75,0x7e,0x63,0x68,
   0xb1,0xba,0xa7,0xac,0x9d,0x96,0x8b,0x80,0xe9,0xe2,0xff,0xf4,0xc5,0xce,0xd3,0xd8,
   0x7a,0x71,0x6c,0x67,0x56,0x5d,0x40,0x4b,0x22,0x29,0x34,0x3f,0x0e,0x05,0x18,0x13,
   0xca,0xc1,0xdc,0xd7,0xe6,0xed,0xf0,0xfb,0x92,0x99,0x84,0x8f,0xbe,0xb5,0xa8,0xa3
};

const uint8_t mul13[] = {
   0x00,0x0d,0x1a,0x17,0x34,0x39,0x2e,0x23,0x68,0x65,0x72,0x7f,0x5c,0x51,0x46,0x4b,
   0xd0,0xdd,0xca,0xc7,0xe4,0xe9,0xfe,0xf3,0xb8,0xb5,0xa2,0xaf,0x8c,0x81,0x96,0x9b,
   0xbb,0xb6,0xa1,0xac,0x8f,0x82,0x95,0x98,0xd3,0xde,0xc9,0xc4,0xe7,0xea,0xfd,0xf0,
   0x6b,0x66,0x71,0x7c,0x5f,0x52,0x45,0x48,0x03,0x0e,0x19,0x14,0x37,0x3a,0x2d,0x20,
   0x6d,0x60,0x77,0x7a,0x59,0x54,0x43,0x4e,0x05,0x08,0x1f,0x12,0x31,0x3c,0x2b,0x26,
   0xbd,0xb0,0xa7,0xaa,0x89,0x84,0x93,0x9e,0xd5,0xd8,0xcf,0xc2,0xe1,0xec,0xfb,0xf6,
   0xd6,0xdb,0xcc,0xc1,0xe2,0xef,0xf8,0xf5,0xbe,0xb3,0xa4,0xa9,0x8a,0x87,0x90,0x9d,
   0x06,0x0b,0x1c,0x11,0x32,0x3f,0x28,0x25,0x6e,0x63,0x74,0x79,0x5a,0x57,0x40,0x4d,
   0xda,0xd7,0xc0,0xcd,0xee,0xe3,0xf4,0xf9,0xb2,0xbf,0xa8,0xa5,0x86,0x8b,0x9c,0x91,
   0x0a,0x07,0x10,0x1d,0x3e,0x33,0x24,0x29,0x62,0x6f,0x78,0x75,0x56,0x5b,0x4c,0x41,
   0x61,0x6c,0x7b,0x76,0x55,0x58,0x4f,0x42,0x09,0x04,0x13,0x1e,0x3d,0x30,0x27,0x2a,
   0xb1,0xbc,0xab,0xa6,0x85,0x88,0x9f,0x92,0xd9,0xd4,0xc3,0xce,0xed,0xe0,0xf7,0xfa,
   0xb7,0xba,0xad,0xa0,0x83,0x8e,0x99,0x94,0xdf,0xd2,0xc5,0xc8,0xeb,0xe6,0xf1,0xfc,
   0x67,0x6a,0x7d,0x70,0x53,0x5e,0x49,0x44,0x0f,0x02,0x15,0x18,0x3b,0x36,0x21,0x2c,
   0x0c,0x01,0x16,0x1b,0x38,0x35,0x22,0x2f,0x64,0x69,0x7e,0x73,0x50,0x5d,0x4a,0x47,
   0xdc,0xd1,0xc6,0xcb,0xe8,0xe5,0xf2,0xff,0xb4,0xb9,0xae,0xa3,0x80,0x8d,0x9a,0x97
};

const uint8_t mul14[] = {
   0x00,0x0e,0x1c,0x12,0x38,0x36,0x24,0x2a,0x70,0x7e,0x6c,0x62,0x48,0x46,0x54,0x5a,
   0xe0,0xee,0xfc,0xf2,0xd8,0xd6,0xc4,0xca,0x90,0x9e,0x8c,0x82,0xa8,0xa6,0xb4,0xba,
   0xdb,0xd5,0xc7,0xc9,0xe3,0xed,0xff,0xf1,0xab,0xa5,0xb7,0xb9,0x93,0x9d,0x8f,0x81,
   0x3b,0x35,0x27,0x29,0x03,0x0d,0x1f,0x11,0x4b,0x45,0x57,0x59,0x73,0x7d,0x6f,0x61,
   0xad,0xa3,0xb1,0xbf,0x95,0x9b,0x89,0x87,0xdd,0xd3,0xc1,0xcf,0xe5,0xeb,0xf9,0xf7,
   0x4d,0x43,0x51,0x5f,0x75,0x7b,0x69,0x67,0x3d,0x33,0x21,0x2f,0x05,0x0b,0x19,0x17,
   0x76,0x78,0x6a,0x64,0x4e,0x40,0x52,0x5c,0x06,0x08,0x1a,0x14,0x3e,0x30,0x22,0x2c,
   0x96,0x98,0x8a,0x84,0xae,0xa0,0xb2,0xbc,0xe6,0xe8,0xfa,0xf4,0xde,0xd0,0xc2,0xcc,
   0x41,0x4f,0x5d,0x53,0x79,0x77,0x65,0x6b,0x31,0x3f,0x2d,0x23,0x09,0x07,0x15,0x1b,
   0xa1,0xaf,0xbd,0xb3,0x99,0x97,0x85,0x8b,0xd1,0xdf,0xcd,0xc3,0xe9,0xe7,0xf5,0xfb,
   0x9a,0x94,0x86,0x88,0xa2,0xac,0xbe,0xb0,0xea,0xe4,0xf6,0xf8,0xd2,0xdc,0xce,0xc0,
   0x7a,0x74,0x66,0x68,0x42,0x4c,0x5e,0x50,0x0a,0x04,0x16,0x18,0x32,0x3c,0x2e,0x20,
   0xec,0xe2,0xf0,0xfe,0xd4,0xda,0xc8,0xc6,0x9c,0x92,0x80,0x8e,0xa4,0xaa,0xb8,0xb6,
   0x0c,0x02,0x10,0x1e,0x34,0x3a,0x28,0x26,0x7c,0x72,0x60,0x6e,0x44,0x4a,0x58,0x56,
   0x37,0x39,0x2b,0x25,0x0f,0x01,0x13,0x1d,0x47,0x49,0x5b,0x55,0x7f,0x71,0x63,0x6d,
   0xd7,0xd9,0xcb,0xc5,0xef,0xe1,0xf3,0xfd,0xa7,0xa9,0xbb,0xb5,0x9f,0x91,0x83,0x8d
};

static const uint8_t rsbox[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

void FillLookupTable(LookupTableAddr addr,const uint8_t* mem,int size){
   VersatMemoryCopy(addr.addr,(int*) mem,size * sizeof(uint8_t));
}

#if 0
void FillKeySchedule(GenericKeySchedule256Addr addr){
   FillLookupTable(addr.s.b_0,sbox,ARRAY_SIZE(sbox));
   FillLookupTable(addr.s.b_1,sbox,ARRAY_SIZE(sbox));
}
#endif

#if 0
void FillRow(DoRowAddr addr){
   FillLookupTable(addr.mul2_0,mul2,ARRAY_SIZE(mul2));
   FillLookupTable(addr.mul2_1,mul2,ARRAY_SIZE(mul2));
   FillLookupTable(addr.mul3_0,mul3,ARRAY_SIZE(mul3));
   FillLookupTable(addr.mul3_1,mul3,ARRAY_SIZE(mul3));
}

void FillMixColumns(MixColumnsAddr addr){
   FillRow(addr.d_0);
   FillRow(addr.d_1);
   FillRow(addr.d_2);
   FillRow(addr.d_3);
}

void FillMainRound(FullAESRoundsAddr addr){
   FillMixColumns(addr.mixColumns);
}
#endif

#if 0
void FillInvDoRowAddr(InvDoRowAddr addr){
  FillLookupTable(addr.mul9_0,mul9,ARRAY_SIZE(mul9));
  FillLookupTable(addr.mul9_1,mul9,ARRAY_SIZE(mul9));
  FillLookupTable(addr.mul11_0,mul11,ARRAY_SIZE(mul11));
  FillLookupTable(addr.mul11_1,mul11,ARRAY_SIZE(mul11));
  FillLookupTable(addr.mul13_0,mul13,ARRAY_SIZE(mul13));
  FillLookupTable(addr.mul13_1,mul13,ARRAY_SIZE(mul13));
  FillLookupTable(addr.mul14_0,mul14,ARRAY_SIZE(mul14));
  FillLookupTable(addr.mul14_1,mul14,ARRAY_SIZE(mul14));
}

void FillInvMixColumns(InvMixColumnsAddr addr){
   FillInvDoRowAddr(addr.d_0);
   FillInvDoRowAddr(addr.d_1);
   FillInvDoRowAddr(addr.d_2);
   FillInvDoRowAddr(addr.d_3);
}

void FillInvMainRound(FullAESRoundsAddr addr){
   FillInvMixColumns(addr.invMixColumns);
}
#endif

void ExpandKey128(uint8_t* key){
   GoodCryptoAlgosConfig* config = (GoodCryptoAlgosConfig*) accelConfig;

   GoodCryptoAlgosAddr addr = ACCELERATOR_TOP_ADDR_INIT;
   RegFileAddr* view = &addr.key_0;
   for(int i = 0; i < 16; i++){
      VersatUnitWrite(view[i].addr,0,key[i]);
   }

   int rcon[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36};

   config->FullAES_AESFirstAdd.key_0.disabled = 0;
   config->FullAES_AESFirstAdd.schedule_s_mux_0.sel = 1;
   config->FullAES_AESFirstAdd.schedule_s_mux_1.sel = 1;
   config->FullAES_AESFirstAdd.schedule_s_mux_2.sel = 1;
   config->FullAES_AESFirstAdd.schedule_s_mux_3.sel = 1;
   for(int i = 0; i < ARRAY_SIZE(rcon); i++){
      config->FullAES_AESFirstAdd.rcon.constant = rcon[i];
      config->FullAES_AESFirstAdd.key_0.selectedOutput0 = i;
      config->FullAES_AESFirstAdd.key_0.selectedOutput1 = i;
      config->FullAES_AESFirstAdd.key_0.selectedInput = i + 1;

      EndAccelerator();
      StartAccelerator();
  }

  EndAccelerator();   
  config->FullAES_AESFirstAdd.key_0.disabled = 1;
}

void Encrypt(uint8_t* data,uint8_t* result){
   GoodCryptoAlgosConfig* config = (GoodCryptoAlgosConfig*) accelConfig;

   GoodCryptoAlgosAddr addr = ACCELERATOR_TOP_ADDR_INIT;

   RegAddr* view = &addr.state_0;
   for(int i = 0; i < 16; i++){
      VersatUnitWrite(view[i].addr,0,data[i]);
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESFirstAdd);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 0;

   StartAccelerator();

   ActivateMergedAccelerator(MergeType_FullAES_AESRound);
   
   for(int i = 0; i < 9; i++){
      config->FullAES_AESFirstAdd.key_0.selectedOutput0 = i + 1;
      EndAccelerator();
      StartAccelerator();
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESLastRound);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 10; 

   EndAccelerator();
   StartAccelerator();
   EndAccelerator();

   for(int ii = 0; ii < 16; ii++){
      result[ii] = VersatUnitRead(view[ii].addr,0);
   }
}

void Decrypt(uint8_t* data,uint8_t* result){
   GoodCryptoAlgosConfig* config = (GoodCryptoAlgosConfig*) accelConfig;

   GoodCryptoAlgosAddr addr = ACCELERATOR_TOP_ADDR_INIT;

   RegAddr* view = &addr.state_0;
   for(int i = 0; i < 16; i++){
      VersatUnitWrite(view[i].addr,0,data[i]);
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESInvFirstAdd);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 10;

   StartAccelerator();

   ActivateMergedAccelerator(MergeType_FullAES_AESInvRound);
   
   for(int i = 9; i > 0; i--){
      config->FullAES_AESFirstAdd.key_0.selectedOutput0 = i;
      EndAccelerator();
      StartAccelerator();
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESInvLastRound);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 0;

   EndAccelerator();
   StartAccelerator();
   EndAccelerator();

   for(int ii = 0; ii < 16; ii++){
      result[ii] = VersatUnitRead(view[ii].addr,0);
   }
}

void ExpandKey256(uint8_t* key){
   GoodCryptoAlgosConfig* config = (GoodCryptoAlgosConfig*) accelConfig;

   GoodCryptoAlgosAddr addr = ACCELERATOR_TOP_ADDR_INIT;
   RegFileAddr* view = &addr.key_0;
   for(int i = 0; i < 16; i++){
      VersatUnitWrite(view[i].addr,0,key[i]);
   }
   for(int i = 0; i < 16; i++){
      VersatUnitWrite(view[i].addr,1,key[i+16]);
   }   

   int rcon[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40};

   config->FullAES_AESFirstAdd.key_0.disabled = 0;
   for(int i = 0; i < ARRAY_SIZE(rcon) * 2 - 1; i++){
      if(i % 2 == 1) {
         config->FullAES_AESFirstAdd.rcon.constant = 0;
      } else {
         config->FullAES_AESFirstAdd.rcon.constant = rcon[i / 2];
      }
      config->FullAES_AESFirstAdd.key_0.selectedOutput0 = i;
      config->FullAES_AESFirstAdd.key_0.selectedOutput1 = i + 1;
      config->FullAES_AESFirstAdd.key_0.selectedInput = i + 2;
      config->FullAES_AESFirstAdd.schedule_s_mux_0.sel = (i + 1) % 2;
      config->FullAES_AESFirstAdd.schedule_s_mux_1.sel = (i + 1) % 2;
      config->FullAES_AESFirstAdd.schedule_s_mux_2.sel = (i + 1) % 2;
      config->FullAES_AESFirstAdd.schedule_s_mux_3.sel = (i + 1) % 2;

      EndAccelerator();
      StartAccelerator();
  }

  EndAccelerator();   
  config->FullAES_AESFirstAdd.key_0.disabled = 1;
}

void Encrypt256(uint8_t* data,uint8_t* result){
   GoodCryptoAlgosConfig* config = (GoodCryptoAlgosConfig*) accelConfig;

   GoodCryptoAlgosAddr addr = ACCELERATOR_TOP_ADDR_INIT;

   RegAddr* view = &addr.state_0;
   for(int i = 0; i < 16; i++){
      VersatUnitWrite(view[i].addr,0,data[i]);
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESFirstAdd);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 0;

   StartAccelerator();

   ActivateMergedAccelerator(MergeType_FullAES_AESRound);
   
   for(int i = 0; i < 13; i++){
      config->FullAES_AESFirstAdd.key_0.selectedOutput0 = i + 1;
      EndAccelerator();
      StartAccelerator();
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESLastRound);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 14; 

   EndAccelerator();
   StartAccelerator();
   EndAccelerator();

   for(int ii = 0; ii < 16; ii++){
      result[ii] = VersatUnitRead(view[ii].addr,0);
   }
}

void Decrypt256(uint8_t* data,uint8_t* result){
   GoodCryptoAlgosConfig* config = (GoodCryptoAlgosConfig*) accelConfig;

   GoodCryptoAlgosAddr addr = ACCELERATOR_TOP_ADDR_INIT;

   RegAddr* view = &addr.state_0;
   for(int i = 0; i < 16; i++){
      VersatUnitWrite(view[i].addr,0,data[i]);
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESInvFirstAdd);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 14;

   StartAccelerator();

   ActivateMergedAccelerator(MergeType_FullAES_AESInvRound);
   
   for(int i = 13; i > 0; i--){
      config->FullAES_AESFirstAdd.key_0.selectedOutput0 = i;
      EndAccelerator();
      StartAccelerator();
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESInvLastRound);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 0; 

   EndAccelerator();
   StartAccelerator();
   EndAccelerator();

   for(int ii = 0; ii < 16; ii++){
      result[ii] = VersatUnitRead(view[ii].addr,0);
   }
}

void PrintKey(int size){
   GoodCryptoAlgosAddr addr = ACCELERATOR_TOP_ADDR_INIT;
   RegFileAddr* view = &addr.key_0;

   for(int i = 0; i < size; i++){
      printf("Index: %d:",i);
      for(int ii = 0; ii < 16; ii++){
         printf("%02x",VersatUnitRead(view[ii].addr,i));
      }
      printf("\n");
   }   
}

//void FillRow(DoRowAddr addr){
//   FillLookupTable(addr.mul2_0,mul2,ARRAY_SIZE(mul2));
//   FillLookupTable(addr.mul2_1,mul2,ARRAY_SIZE(mul2));
//   FillLookupTable(addr.mul3_0,mul3,ARRAY_SIZE(mul3));
//   FillLookupTable(addr.mul3_1,mul3,ARRAY_SIZE(mul3));
//}
//void FillMixColumns(MixColumnsAddr addr){
//   FillRow(addr.d_0);
//   FillRow(addr.d_1);
//   FillRow(addr.d_2);
//   FillRow(addr.d_3);
//}
//void FillMainRound(FullAESRoundsAddr addr){
//   FillMixColumns(addr.mixColumns);
//}

void InitAES(){
   GoodCryptoAlgosAddr addr = ACCELERATOR_TOP_ADDR_INIT;
   
   //FillMainRound(addr.aes.round);
   FillLookupTable(addr.round_mixColumns_d_0_mul2_0,mul2,ARRAY_SIZE(mul2));
   FillLookupTable(addr.round_mixColumns_d_0_mul2_1,mul2,ARRAY_SIZE(mul2));
   FillLookupTable(addr.round_mixColumns_d_0_mul3_0,mul3,ARRAY_SIZE(mul3));
   FillLookupTable(addr.round_mixColumns_d_0_mul3_1,mul3,ARRAY_SIZE(mul3));
   FillLookupTable(addr.round_mixColumns_d_1_mul2_0,mul2,ARRAY_SIZE(mul2));
   FillLookupTable(addr.round_mixColumns_d_1_mul2_1,mul2,ARRAY_SIZE(mul2));
   FillLookupTable(addr.round_mixColumns_d_1_mul3_0,mul3,ARRAY_SIZE(mul3));
   FillLookupTable(addr.round_mixColumns_d_1_mul3_1,mul3,ARRAY_SIZE(mul3));
   FillLookupTable(addr.round_mixColumns_d_2_mul2_0,mul2,ARRAY_SIZE(mul2));
   FillLookupTable(addr.round_mixColumns_d_2_mul2_1,mul2,ARRAY_SIZE(mul2));
   FillLookupTable(addr.round_mixColumns_d_2_mul3_0,mul3,ARRAY_SIZE(mul3));
   FillLookupTable(addr.round_mixColumns_d_2_mul3_1,mul3,ARRAY_SIZE(mul3));
   FillLookupTable(addr.round_mixColumns_d_3_mul2_0,mul2,ARRAY_SIZE(mul2));
   FillLookupTable(addr.round_mixColumns_d_3_mul2_1,mul2,ARRAY_SIZE(mul2));
   FillLookupTable(addr.round_mixColumns_d_3_mul3_0_invMixColumns_d_3_mul14_1,mul3,ARRAY_SIZE(mul3));
   FillLookupTable(addr.round_mixColumns_d_3_mul3_1_invMixColumns_d_0_mul9_1,mul3,ARRAY_SIZE(mul3));

   FillLookupTable(addr.schedule_s_b_0,sbox,ARRAY_SIZE(sbox));
   FillLookupTable(addr.schedule_s_b_1,sbox,ARRAY_SIZE(sbox));

   //FillKeySchedule(addr.aes.schedule);
   GoodCryptoAlgosConfig* config = (GoodCryptoAlgosConfig*) accelConfig;

   {
      RegAddr* view = &addr.lastResult_0;
      for(int i = 0; i < 16; i++){
         VersatUnitWrite(view[i].addr,0,0);
      }
   }
   {
      RegAddr* view = &addr.lastValToAdd_0;
      for(int i = 0; i < 16; i++){
         VersatUnitWrite(view[i].addr,0,0);
      }
   }

   config->FullAES_AESFirstAdd.lastResult_0.disabled = 1;
}

void InitInvAES(){
   GoodCryptoAlgosAddr addr = ACCELERATOR_TOP_ADDR_INIT;
   //FillInvMainRound(addr.aes.round);

   FillLookupTable(addr.round_invMixColumns_d_0_mul9_0,mul9,ARRAY_SIZE(mul9));
   FillLookupTable(addr.round_mixColumns_d_3_mul3_1_invMixColumns_d_0_mul9_1,mul9,ARRAY_SIZE(mul9));
   FillLookupTable(addr.round_invMixColumns_d_0_mul11_0,mul11,ARRAY_SIZE(mul11));
   FillLookupTable(addr.round_invMixColumns_d_0_mul11_1,mul11,ARRAY_SIZE(mul11));
   FillLookupTable(addr.round_invMixColumns_d_0_mul13_0,mul13,ARRAY_SIZE(mul13));
   FillLookupTable(addr.round_invMixColumns_d_0_mul13_1,mul13,ARRAY_SIZE(mul13));
   FillLookupTable(addr.round_invMixColumns_d_0_mul14_0,mul14,ARRAY_SIZE(mul14));
   FillLookupTable(addr.round_invMixColumns_d_0_mul14_1,mul14,ARRAY_SIZE(mul14));
   FillLookupTable(addr.round_invMixColumns_d_1_mul9_0,mul9,ARRAY_SIZE(mul9));
   FillLookupTable(addr.round_invMixColumns_d_1_mul9_1,mul9,ARRAY_SIZE(mul9));
   FillLookupTable(addr.round_invMixColumns_d_1_mul11_0,mul11,ARRAY_SIZE(mul11));
   FillLookupTable(addr.round_invMixColumns_d_1_mul11_1,mul11,ARRAY_SIZE(mul11));
   FillLookupTable(addr.round_invMixColumns_d_1_mul13_0,mul13,ARRAY_SIZE(mul13));
   FillLookupTable(addr.round_invMixColumns_d_1_mul13_1,mul13,ARRAY_SIZE(mul13));
   FillLookupTable(addr.round_invMixColumns_d_1_mul14_0,mul14,ARRAY_SIZE(mul14));
   FillLookupTable(addr.round_invMixColumns_d_1_mul14_1,mul14,ARRAY_SIZE(mul14));
   FillLookupTable(addr.round_invMixColumns_d_2_mul9_0,mul9,ARRAY_SIZE(mul9));
   FillLookupTable(addr.round_invMixColumns_d_2_mul9_1,mul9,ARRAY_SIZE(mul9));
   FillLookupTable(addr.round_invMixColumns_d_2_mul11_0,mul11,ARRAY_SIZE(mul11));
   FillLookupTable(addr.round_invMixColumns_d_2_mul11_1,mul11,ARRAY_SIZE(mul11));
   FillLookupTable(addr.round_invMixColumns_d_2_mul13_0,mul13,ARRAY_SIZE(mul13));
   FillLookupTable(addr.round_invMixColumns_d_2_mul13_1,mul13,ARRAY_SIZE(mul13));
   FillLookupTable(addr.round_invMixColumns_d_2_mul14_0,mul14,ARRAY_SIZE(mul14));
   FillLookupTable(addr.round_invMixColumns_d_2_mul14_1,mul14,ARRAY_SIZE(mul14));
   FillLookupTable(addr.round_invMixColumns_d_3_mul9_0,mul9,ARRAY_SIZE(mul9));
   FillLookupTable(addr.round_invMixColumns_d_3_mul9_1,mul9,ARRAY_SIZE(mul9));
   FillLookupTable(addr.round_invMixColumns_d_3_mul11_0,mul11,ARRAY_SIZE(mul11));
   FillLookupTable(addr.round_invMixColumns_d_3_mul11_1,mul11,ARRAY_SIZE(mul11));
   FillLookupTable(addr.round_invMixColumns_d_3_mul13_0,mul13,ARRAY_SIZE(mul13));
   FillLookupTable(addr.round_invMixColumns_d_3_mul13_1,mul13,ARRAY_SIZE(mul13));
   FillLookupTable(addr.round_invMixColumns_d_3_mul14_0,mul14,ARRAY_SIZE(mul14));
   FillLookupTable(addr.round_mixColumns_d_3_mul3_0_invMixColumns_d_3_mul14_1,mul14,ARRAY_SIZE(mul14));
}

void PrintResult(uint8_t* buffer){
   for(int i = 0; i < 16; i++){
      printf("%02x",buffer[i]);
   }   
}

void ECB128(const char* key,const char* plaintext,const char* expected){
   uint8_t keyBuffer[16];
   uint8_t dataBuffer[32];
   uint8_t encrypted[32];
   uint8_t decrypted[32];

   InitAES();

   HexStringToHex((char*) keyBuffer,key);
   HexStringToHex((char*) dataBuffer,plaintext);

   ExpandKey128(keyBuffer);

   Encrypt(dataBuffer,encrypted);
   Encrypt(dataBuffer + 16,encrypted + 16);

   InitInvAES();
   Decrypt(encrypted,decrypted);
   Decrypt(encrypted + 16,decrypted + 16);

   printf("ECB128:\n");
   printf("Encrypted:");
   PrintResult(encrypted);
   printf(" ");
   PrintResult(encrypted + 16);
   printf("\n");
   printf(" Expected:%s",expected);
   printf("\n");
   printf("\n");
   printf("Decrypted:");
   PrintResult(decrypted);
   printf(" ");
   PrintResult(decrypted + 16);
   printf("\n");
   printf(" Expected:%.32s",plaintext);
   printf(" %s",plaintext + 32);
   printf("\n");
}

void ECB256(const char* key,const char* plaintext,const char* expected){
   uint8_t keyBuffer[32];
   uint8_t dataBuffer[32];
   uint8_t encrypted[32];
   uint8_t decrypted[32];

   InitAES();

   HexStringToHex((char*) keyBuffer,key);
   HexStringToHex((char*) dataBuffer,plaintext);

   SignalLoop();
   ExpandKey256(keyBuffer);

   SignalLoop();
   Encrypt256(dataBuffer,encrypted);

   SignalLoop();
   Encrypt256(dataBuffer + 16,encrypted + 16);

   InitInvAES();
   Decrypt256(encrypted,decrypted);
   Decrypt256(encrypted + 16,decrypted + 16);

   printf("ECB256:\n");
   printf("Encrypted:");
   PrintResult(encrypted);
   printf(" ");
   PrintResult(encrypted + 16);
   printf("\n");
   printf(" Expected:%s",expected);
   printf("\n");
   printf("\n");
   printf("Decrypted:");
   PrintResult(decrypted);
   printf(" ");
   PrintResult(decrypted + 16);
   printf("\n");
   printf(" Expected:%.32s",plaintext);
   printf(" %s",plaintext + 32);
   printf("\n");
}

//CBC - Result of ciphertext is xor for next block

void LoadIV(uint8_t* iv){
   GoodCryptoAlgosAddr addr = ACCELERATOR_TOP_ADDR_INIT;
   GoodCryptoAlgosConfig* config = (GoodCryptoAlgosConfig*) accelConfig;

   RegAddr* view = &addr.lastResult_0;
   for(int i = 0; i < 16; i++){
      VersatUnitWrite(view[i].addr,0,iv[i]);
   }

   config->FullAES_AESFirstAdd.lastResult_0.disabled = 1;
}

void EncryptWithIV(uint8_t* data,uint8_t* result){
   GoodCryptoAlgosConfig* config = (GoodCryptoAlgosConfig*) accelConfig;

   GoodCryptoAlgosAddr addr = ACCELERATOR_TOP_ADDR_INIT;

   RegAddr* view = &addr.state_0;
   for(int i = 0; i < 16; i++){
      VersatUnitWrite(view[i].addr,0,data[i]);
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESFirstAdd);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 0;

   StartAccelerator();

   ActivateMergedAccelerator(MergeType_FullAES_AESRound);
   
   for(int i = 0; i < 9; i++){
      config->FullAES_AESFirstAdd.key_0.selectedOutput0 = i + 1;
      EndAccelerator();
      StartAccelerator();
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESLastRound);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 10; 
   config->FullAES_AESFirstAdd.lastResult_0.disabled = 0;

   EndAccelerator();
   StartAccelerator();
   EndAccelerator();

   config->FullAES_AESFirstAdd.lastResult_0.disabled = 1;

   for(int ii = 0; ii < 16; ii++){
      result[ii] = VersatUnitRead(view[ii].addr,0);
   }
}

void DecryptWithIV(uint8_t* data,uint8_t* result){
   GoodCryptoAlgosConfig* config = (GoodCryptoAlgosConfig*) accelConfig;

   GoodCryptoAlgosAddr addr = ACCELERATOR_TOP_ADDR_INIT;

   RegAddr* view = &addr.state_0;
   for(int i = 0; i < 16; i++){
      VersatUnitWrite(view[i].addr,0,data[i]);
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESInvFirstAdd);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 10;

   StartAccelerator();

   ActivateMergedAccelerator(MergeType_FullAES_AESInvRound);
   
   for(int i = 9; i > 0; i--){
      config->FullAES_AESFirstAdd.key_0.selectedOutput0 = i;
      EndAccelerator();
      StartAccelerator();
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESInvLastRound);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 0;
   config->FullAES_AESFirstAdd.lastResult_0.disabled = 0;

   EndAccelerator();
   StartAccelerator();
   EndAccelerator();

   config->FullAES_AESFirstAdd.lastResult_0.disabled = 1;

   for(int ii = 0; ii < 16; ii++){
      result[ii] = VersatUnitRead(view[ii].addr,0);
   }
}

void Encrypt256WithIV(uint8_t* data,uint8_t* result){
   GoodCryptoAlgosConfig* config = (GoodCryptoAlgosConfig*) accelConfig;

   GoodCryptoAlgosAddr addr = ACCELERATOR_TOP_ADDR_INIT;

   RegAddr* view = &addr.state_0;
   for(int i = 0; i < 16; i++){
      VersatUnitWrite(view[i].addr,0,data[i]);
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESFirstAdd);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 0;

   StartAccelerator();

   ActivateMergedAccelerator(MergeType_FullAES_AESRound);
   
   for(int i = 0; i < 13; i++){
      config->FullAES_AESFirstAdd.key_0.selectedOutput0 = i + 1;
      EndAccelerator();
      StartAccelerator();
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESLastRound);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 14; 
   config->FullAES_AESFirstAdd.lastResult_0.disabled = 0;

   EndAccelerator();
   StartAccelerator();
   EndAccelerator();

   config->FullAES_AESFirstAdd.lastResult_0.disabled = 1;

   for(int ii = 0; ii < 16; ii++){
      result[ii] = VersatUnitRead(view[ii].addr,0);
   }
}

void Decrypt256WithIV(uint8_t* data,uint8_t* result){
   GoodCryptoAlgosConfig* config = (GoodCryptoAlgosConfig*) accelConfig;

   GoodCryptoAlgosAddr addr = ACCELERATOR_TOP_ADDR_INIT;

   RegAddr* view = &addr.state_0;
   for(int i = 0; i < 16; i++){
      VersatUnitWrite(view[i].addr,0,data[i]);
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESInvFirstAdd);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 14;

   StartAccelerator();

   ActivateMergedAccelerator(MergeType_FullAES_AESInvRound);
   
   for(int i = 13; i > 0; i--){
      config->FullAES_AESFirstAdd.key_0.selectedOutput0 = i;
      EndAccelerator();
      StartAccelerator();
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESInvLastRound);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 0; 
   config->FullAES_AESFirstAdd.lastResult_0.disabled = 0;

   EndAccelerator();
   StartAccelerator();
   EndAccelerator();

   config->FullAES_AESFirstAdd.lastResult_0.disabled = 1;

   for(int ii = 0; ii < 16; ii++){
      result[ii] = VersatUnitRead(view[ii].addr,0);
   }
}

void CBC128(const char* key,const char* iv,const char* plaintext,const char* expected){
   uint8_t keyBuffer[16];
   uint8_t ivBuffer[16];
   uint8_t dataBuffer[32];
   uint8_t encrypted[32];
   uint8_t decrypted[32];

   InitAES();

   HexStringToHex((char*) keyBuffer,key);
   HexStringToHex((char*) ivBuffer,iv);
   HexStringToHex((char*) dataBuffer,plaintext);

   ExpandKey128(keyBuffer);
   LoadIV(ivBuffer);

   SignalLoop();
   EncryptWithIV(dataBuffer,encrypted);
   EncryptWithIV(dataBuffer + 16,encrypted + 16);

   InitInvAES();

   LoadIV(ivBuffer);
   DecryptWithIV(encrypted,decrypted);

   LoadIV(encrypted); // Need to load directly the last 
   DecryptWithIV(encrypted + 16,decrypted + 16);

   printf("CBC128:\n");
   printf("Encrypted:");
   PrintResult(encrypted);
   printf(" ");
   PrintResult(encrypted + 16);
   printf("\n");
   printf(" Expected:%s",expected);
   printf("\n");
   printf("\n");
   printf("Decrypted:");
   PrintResult(decrypted);
   printf(" ");
   PrintResult(decrypted + 16);
   printf("\n");
   printf(" Expected:%.32s",plaintext);
   printf(" %s",plaintext + 32);
   printf("\n");
}

void CBC256(const char* key,const char* iv,const char* plaintext,const char* expected){
   uint8_t keyBuffer[32];
   uint8_t ivBuffer[16];
   uint8_t dataBuffer[32];
   uint8_t encrypted[32];
   uint8_t decrypted[32];

   InitAES();

   HexStringToHex((char*) keyBuffer,key);
   HexStringToHex((char*) ivBuffer,iv);
   HexStringToHex((char*) dataBuffer,plaintext);

   ExpandKey256(keyBuffer);
   LoadIV(ivBuffer);

   Encrypt256WithIV(dataBuffer,encrypted);
   Encrypt256WithIV(dataBuffer + 16,encrypted + 16);

   InitInvAES();

   LoadIV(ivBuffer);
   Decrypt256WithIV(encrypted,decrypted);

   LoadIV(encrypted); // Need to load directly the last 
   Decrypt256WithIV(encrypted + 16,decrypted + 16);

   printf("CBC256:\n");
   printf("Encrypted:");
   PrintResult(encrypted);
   printf(" ");
   PrintResult(encrypted + 16);
   printf("\n");
   printf(" Expected:%s",expected);
   printf("\n");
   printf("\n");
   printf("Decrypted:");
   PrintResult(decrypted);
   printf(" ");
   PrintResult(decrypted + 16);
   printf("\n");
   printf(" Expected:%.32s",plaintext);
   printf(" %s",plaintext + 32);
   printf("\n");
}

//CTR - Counter, need to xor plaintext at the end.
void EncryptWithLastAddition(uint8_t* data,uint8_t* result,uint8_t* lastAddition){
   GoodCryptoAlgosConfig* config = (GoodCryptoAlgosConfig*) accelConfig;

   GoodCryptoAlgosAddr addr = ACCELERATOR_TOP_ADDR_INIT;

   RegAddr* view = &addr.state_0;
   for(int i = 0; i < 16; i++){
      VersatUnitWrite(view[i].addr,0,data[i]);
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESFirstAdd);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 0;

   StartAccelerator();

   ActivateMergedAccelerator(MergeType_FullAES_AESRound);
   
   for(int i = 0; i < 9; i++){
      config->FullAES_AESFirstAdd.key_0.selectedOutput0 = i + 1;
      EndAccelerator();
      StartAccelerator();
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESLastRound);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 10; 

   EndAccelerator();

   {
      RegAddr* view = &addr.lastValToAdd_0;
      for(int i = 0; i < 16; i++){
         VersatUnitWrite(view[i].addr,0,lastAddition[i]);
      }
   }

   StartAccelerator();
   EndAccelerator();

   {
      RegAddr* view = &addr.lastValToAdd_0;
      for(int i = 0; i < 16; i++){
         VersatUnitWrite(view[i].addr,0,0);
      }
   }

   for(int ii = 0; ii < 16; ii++){
      result[ii] = VersatUnitRead(view[ii].addr,0);
   }
}

void Encrypt256WithLastAddition(uint8_t* data,uint8_t* result,uint8_t* lastAddition){
   GoodCryptoAlgosConfig* config = (GoodCryptoAlgosConfig*) accelConfig;

   GoodCryptoAlgosAddr addr = ACCELERATOR_TOP_ADDR_INIT;

   RegAddr* view = &addr.state_0;
   for(int i = 0; i < 16; i++){
      VersatUnitWrite(view[i].addr,0,data[i]);
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESFirstAdd);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 0;

   StartAccelerator();

   ActivateMergedAccelerator(MergeType_FullAES_AESRound);
   
   for(int i = 0; i < 13; i++){
      config->FullAES_AESFirstAdd.key_0.selectedOutput0 = i + 1;
      EndAccelerator();
      StartAccelerator();
   }

   ActivateMergedAccelerator(MergeType_FullAES_AESLastRound);
   config->FullAES_AESFirstAdd.key_0.selectedOutput0 = 14; 

   EndAccelerator();

   {
      RegAddr* view = &addr.lastValToAdd_0;
      for(int i = 0; i < 16; i++){
         VersatUnitWrite(view[i].addr,0,lastAddition[i]);
      }
   }

   StartAccelerator();
   EndAccelerator();

   {
      RegAddr* view = &addr.lastValToAdd_0;
      for(int i = 0; i < 16; i++){
         VersatUnitWrite(view[i].addr,0,0);
      }
   }

   for(int ii = 0; ii < 16; ii++){
      result[ii] = VersatUnitRead(view[ii].addr,0);
   }
}

void CTR128(const char* key,const char* counter,const char* plaintext,const char* expected){
   uint8_t keyBuffer[16];
   uint8_t counterBuffer[16];
   uint8_t dataBuffer[32];
   uint8_t encrypted[32];
   uint8_t decrypted[32];

   InitAES();

   HexStringToHex((char*) keyBuffer,key);
   HexStringToHex((char*) counterBuffer,counter);
   HexStringToHex((char*) dataBuffer,plaintext);

   ExpandKey128(keyBuffer);

{
   SignalLoop();
   EncryptWithLastAddition(counterBuffer,encrypted,dataBuffer);

   char* view = (char*) (counterBuffer + 15);
   *view += 1;
   char* view2 = (char*) (counterBuffer + 14);
   *view2 += 1;

   EncryptWithLastAddition(counterBuffer,encrypted + 16,dataBuffer + 16);
}

   HexStringToHex((char*) counterBuffer,counter);

{
   EncryptWithLastAddition(counterBuffer,decrypted,encrypted);

   char* view = (char*) (counterBuffer + 15);
   *view += 1;
   char* view2 = (char*) (counterBuffer + 14);
   *view2 += 1;

   EncryptWithLastAddition(counterBuffer,decrypted + 16,encrypted + 16);
}

   printf("CTR128:\n");
   printf("Encrypted:");
   PrintResult(encrypted);
   printf(" ");
   PrintResult(encrypted + 16);
   printf("\n");
   printf(" Expected:%s",expected);
   printf("\n");
   printf("\n");
   printf("Decrypted:");
   PrintResult(decrypted);
   printf(" ");
   PrintResult(decrypted + 16);
   printf("\n");
   printf(" Expected:%.32s",plaintext);
   printf(" %s",plaintext + 32);
   printf("\n");
}

void CTR256(const char* key,const char* counter,const char* plaintext,const char* expected){
   uint8_t keyBuffer[32];
   uint8_t counterBuffer[16];
   uint8_t dataBuffer[32];
   uint8_t encrypted[32];
   uint8_t decrypted[32];

   InitAES();

   HexStringToHex((char*) keyBuffer,key);
   HexStringToHex((char*) counterBuffer,counter);
   HexStringToHex((char*) dataBuffer,plaintext);

   ExpandKey256(keyBuffer);

{
   SignalLoop();
   Encrypt256WithLastAddition(counterBuffer,encrypted,dataBuffer);

   char* view = (char*) (counterBuffer + 15);
   *view += 1;
   char* view2 = (char*) (counterBuffer + 14);
   *view2 += 1;

   Encrypt256WithLastAddition(counterBuffer,encrypted + 16,dataBuffer + 16);
}

   HexStringToHex((char*) counterBuffer,counter);

{
   Encrypt256WithLastAddition(counterBuffer,decrypted,encrypted);

   char* view = (char*) (counterBuffer + 15);
   *view += 1;
   char* view2 = (char*) (counterBuffer + 14);
   *view2 += 1;

   Encrypt256WithLastAddition(counterBuffer,decrypted + 16,encrypted + 16);
}

   printf("CTR256:\n");
   printf("Encrypted:");
   PrintResult(encrypted);
   printf(" ");
   PrintResult(encrypted + 16);
   printf("\n");
   printf(" Expected:%s",expected);
   printf("\n");
   printf("\n");
   printf("Decrypted:");
   PrintResult(decrypted);
   printf(" ");
   PrintResult(decrypted + 16);
   printf("\n");
   printf(" Expected:%.32s",plaintext);
   printf(" %s",plaintext + 32);
   printf("\n");
}


static uint32_t initialStateValues[] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
static uint32_t kConstants0[] = {0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174};
static uint32_t kConstants1[] = {0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967};
static uint32_t kConstants2[] = {0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070};
static uint32_t kConstants3[] = {0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};

static uint32_t* kConstants[4] = {kConstants0,kConstants1,kConstants2,kConstants3};

// GLOBALS
static bool initVersat = false;

static void store_bigendian_32(uint8_t *x, uint32_t u) {
   x[3] = (uint8_t) u;
   u >>= 8;
   x[2] = (uint8_t) u;
   u >>= 8;
   x[1] = (uint8_t) u;
   u >>= 8;
   x[0] = (uint8_t) u;
}

void InitVersatSHA(){
   GoodCryptoAlgosConfig* config = (GoodCryptoAlgosConfig*) accelConfig;
   SHAConfig* sha = &config->SHA;

   *sha = (SHAConfig){0};
//   ActivateMergedAccelerator(SHA);
   ConfigureSimpleVRead(&sha->MemRead,16,nullptr);

   sha->cMem0_mem.iterA = 1;
   sha->cMem0_mem.incrA = 1;
   sha->cMem0_mem.perA = 16;
   sha->cMem0_mem.dutyA = 16;
   sha->cMem0_mem.startA = 0;
   sha->cMem0_mem.shiftA = 0;

   for(int ii = 0; ii < 16; ii++){
      VersatUnitWrite(TOP_cMem0_mem_addr,ii,kConstants[0][ii]);
   }
   for(int ii = 0; ii < 16; ii++){
      VersatUnitWrite(TOP_cMem1_mem_addr,ii,kConstants[1][ii]);
   }
   for(int ii = 0; ii < 16; ii++){
      VersatUnitWrite(TOP_cMem2_mem_addr,ii,kConstants[2][ii]);
   }
   for(int ii = 0; ii < 16; ii++){
      VersatUnitWrite(TOP_cMem3_mem_addr,ii,kConstants[3][ii]);
   }

   ACCEL_TOP_Swap_enabled = 1;
}

static size_t versat_crypto_hashblocks_sha256(const uint8_t *in, size_t inlen) {
   while (inlen >= 64) {
      ACCEL_TOP_MemRead_ext_addr = (iptr) in;
   
      // Loads data + performs work
      RunAccelerator(1);

      if(!initVersat){
         VersatUnitWrite(TOP_State_s_0_reg_addr,0,initialStateValues[0]);
         VersatUnitWrite(TOP_State_s_1_reg_addr,0,initialStateValues[1]);
         VersatUnitWrite(TOP_State_s_2_reg_addr,0,initialStateValues[2]);
         VersatUnitWrite(TOP_State_s_3_reg_addr,0,initialStateValues[3]);
         VersatUnitWrite(TOP_State_s_4_reg_addr,0,initialStateValues[4]);
         VersatUnitWrite(TOP_State_s_5_reg_addr,0,initialStateValues[5]);
         VersatUnitWrite(TOP_State_s_6_reg_addr,0,initialStateValues[6]);
         VersatUnitWrite(TOP_State_s_7_reg_addr,0,initialStateValues[7]);
         initVersat = true;
      }

      in += 64;
      inlen -= 64;
   }

   return inlen;
}

void VersatSHA(uint8_t *out, const uint8_t *in, size_t inlen) {
   uint8_t padded[128];
   uint64_t bytes = inlen;

   versat_crypto_hashblocks_sha256(in, inlen);
   in += inlen;
   inlen &= 63;
   in -= inlen;

   for (size_t i = 0; i < inlen; ++i) {
      padded[i] = in[i];
   }
   padded[inlen] = 0x80;

   if (inlen < 56) {
      for (size_t i = inlen + 1; i < 56; ++i) {
         padded[i] = 0;
      
    }  padded[56] = (uint8_t) (bytes >> 53);
      padded[57] = (uint8_t) (bytes >> 45);
      padded[58] = (uint8_t) (bytes >> 37);
      padded[59] = (uint8_t) (bytes >> 29);
      padded[60] = (uint8_t) (bytes >> 21);
      padded[61] = (uint8_t) (bytes >> 13);
      padded[62] = (uint8_t) (bytes >> 5);
      padded[63] = (uint8_t) (bytes << 3);
      versat_crypto_hashblocks_sha256(padded, 64);
   } else {
      for (size_t i = inlen + 1; i < 120; ++i) {
         padded[i] = 0;
      }
      padded[120] = (uint8_t) (bytes >> 53);
      padded[121] = (uint8_t) (bytes >> 45);
      padded[122] = (uint8_t) (bytes >> 37);
      padded[123] = (uint8_t) (bytes >> 29);
      padded[124] = (uint8_t) (bytes >> 21);
      padded[125] = (uint8_t) (bytes >> 13);
      padded[126] = (uint8_t) (bytes >> 5);
      padded[127] = (uint8_t) (bytes << 3);
      versat_crypto_hashblocks_sha256(padded, 128);
   }

   RunAccelerator(1);

   store_bigendian_32(&out[0*4],(uint32_t) VersatUnitRead(TOP_State_s_0_reg_addr,0));
   store_bigendian_32(&out[1*4],(uint32_t) VersatUnitRead(TOP_State_s_1_reg_addr,0));
   store_bigendian_32(&out[2*4],(uint32_t) VersatUnitRead(TOP_State_s_2_reg_addr,0));
   store_bigendian_32(&out[3*4],(uint32_t) VersatUnitRead(TOP_State_s_3_reg_addr,0));
   store_bigendian_32(&out[4*4],(uint32_t) VersatUnitRead(TOP_State_s_4_reg_addr,0));
   store_bigendian_32(&out[5*4],(uint32_t) VersatUnitRead(TOP_State_s_5_reg_addr,0));
   store_bigendian_32(&out[6*4],(uint32_t) VersatUnitRead(TOP_State_s_6_reg_addr,0));
   store_bigendian_32(&out[7*4],(uint32_t) VersatUnitRead(TOP_State_s_7_reg_addr,0));

   initVersat = false; // At the end of each run, reset
}

void SingleTest(Arena* arena){
   const char* key128 = "2b7e151628aed2a6abf7158809cf4f3c";

   //                    vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
   //                                                    vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
   const char* key256 = "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4";
   //                       vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
   //                                                       vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
   const char* plaintext = "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e51";
   const char* iv = "000102030405060708090a0b0c0d0e0f";
   const char* counter = "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff";

   ECB128(key128,plaintext,"3ad77bb40d7a3660a89ecaf32466ef97 f5d3d58503b9699de785895a96fdbaaf");
   ECB256(key256,plaintext,"f3eed1bdb5d2a03c064b5a7e3db181f8 591ccb10d410ed26dc5ba74a31362870");

   CBC128(key128,iv,plaintext,"7649abac8119b246cee98e9b12e9197d 5086cb9b507219ee95db113a917678b2");
   CBC256(key256,iv,plaintext,"f58c4c04d6e5f1ba779eabfb5f7bfbd6 9cfc4e967edb808d679f777bc6702c7d");

   CTR128(key128,counter,plaintext,"874d6191b620e3261bef6864990db6ce 9806f66b7970fdff8617187bb9fffdff");
   CTR256(key256,counter,plaintext,"601ec313775789a5b7a7f504bbf3d228 f443e3ca4d62b59aca84e990cacaf5c5");

   {
   //unsigned char msg_64[] = { 0x5a, 0x86, 0xb7, 0x37, 0xea, 0xea, 0x8e, 0xe9, 0x76, 0xa0, 0xa2, 0x4d, 0xa6, 0x3e, 0x7e, 0xd7, 0xee, 0xfa, 0xd1, 0x8a, 0x10, 0x1c, 0x12, 0x11, 0xe2, 0xb3, 0x65, 0x0c, 0x51, 0x87, 0xc2, 0xa8, 0xa6, 0x50, 0x54, 0x72, 0x08, 0x25, 0x1f, 0x6d, 0x42, 0x37, 0xe6, 0x61, 0xc7, 0xbf, 0x4c, 0x77, 0xf3, 0x35, 0x39, 0x03, 0x94, 0xc3, 0x7f, 0xa1, 0xa9, 0xf9, 0xbe, 0x83, 0x6a, 0xc2, 0x85, 0x09 };
   
   unsigned char msg[] = {0x54,0x65,0x73,0x74,0x20,0x73,0x74,0x72,0x69,0x6e,0x67,0x2c,0x20,0x63,0x6f,0x6e,0x74,0x61,0x69,0x6e,0x73,0x20,0x6d,0x75,0x6c,0x74,0x69,0x70,0x6c,0x65,0x20,0x63,0x68,0x61,0x72,0x61,0x63,0x74,0x65,0x72,0x73,0x20,0x69,0x6e,0x20,0x6f,0x72,0x64,0x65,0x72,0x20,0x74,0x6f,0x20,0x74,0x65,0x73,0x74,0x20,0x74,0x68,0x65,0x20,0x62,0x6c,0x6f,0x63,0x6b,0x20,0x73,0x74,0x72,0x65,0x61,0x6d,0x20,0x70,0x72,0x6f,0x70,0x65,0x72,0x6c,0x79};

   static const int HASH_SIZE = (256/8);
   
   InitVersatSHA();

   unsigned char digest[256];
   for(int i = 0; i < 256; i++){
      digest[i] = 0;
   }

   VersatSHA(digest,msg,ARRAY_SIZE(msg));

   char buffer[2048];
   GetHexadecimal((char*) digest,buffer, HASH_SIZE);
   Assert_Eq(buffer,"42e61e174fbb3897d6dd6cef3dd2802fe67b331953b06114a65c772859dfc1aa"); 
   }
}
