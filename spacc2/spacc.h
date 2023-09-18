#ifndef _SPACC_H_
#define _SPACC_H_

enum SPACC_ALGO_E {
  SPACC_ALGO_AES = 1,
  SPACC_ALGO_SM4 = 2,         //cv180x unsupport
  SPACC_ALGO_DES = 3,         //cv180x unsupport
  SPACC_ALGO_TDES = 4,        //cv180x unsupport
  SPACC_ALGO_SHA1 = 5,
  SPACC_ALGO_SHA256 = 6,
  SPACC_ALGO_BASE64 = 7,      //cv180x unsupport
};

enum SPACC_ALGO_MODE_E {
  SPACC_ALGO_MODE_ECB,
  SPACC_ALGO_MODE_CBC,
  SPACC_ALGO_MODE_CTR,
  SPACC_ALGO_MODE_OFB,
};

enum SPACC_KEY_SIZE_E {
  SPACC_KEY_SIZE_64BITS,
  SPACC_KEY_SIZE_128BITS,
  SPACC_KEY_SIZE_192BITS,
  SPACC_KEY_SIZE_256BITS,
};

enum SPACC_ACTION_E {
	SPACC_ACTION_ENCRYPTION,
	SPACC_ACTION_DECRYPT,
};

struct spacc_config {
	unsigned char key[32];
	unsigned char iv[16];
	enum SPACC_ALGO_E algo;
  enum SPACC_ALGO_MODE_E mode;
  enum SPACC_KEY_SIZE_E key_size;
	enum SPACC_ACTION_E action;
};

#define IOCTL_SPACC_BASE                'S'
#define IOCTL_SPACC_SET_BUFFER_SIZE			_IOW(IOCTL_SPACC_BASE, 1, unsigned int)
#define IOCTL_SPACC_GET_BUFFER_SIZE			_IOR(IOCTL_SPACC_BASE, 2, unsigned int)
#define IOCTL_SPACC_SET_CONFIG			    _IOW(IOCTL_SPACC_BASE, 3, struct spacc_config)
#define IOCTL_SPACC_GET_CONFIG			    _IOR(IOCTL_SPACC_BASE, 4, struct spacc_config)

#endif
