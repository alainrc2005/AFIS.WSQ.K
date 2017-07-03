/*******************************************************************************

License: 
This software was developed at the National Institute of Standards and 
Technology (NIST) by employees of the Federal Government in the course 
of their official duties. Pursuant to title 17 Section 105 of the 
United States Code, this software is not subject to copyright protection 
and is in the public domain. NIST assumes no responsibility  whatsoever for 
its use by other parties, and makes no guarantees, expressed or implied, 
about its quality, reliability, or any other characteristic. 

Disclaimer: 
This software was developed to promote biometric standards and biometric
technology testing for the Federal Government in accordance with the USA
PATRIOT Act and the Enhanced Border Security and Visa Entry Reform Act.
Specific hardware and software products identified in this software were used
in order to perform the software development.  In no case does such
identification imply recommendation or endorsement by the National Institute
of Standards and Technology, nor does it imply that the products and equipment
identified are necessarily the best available for the purpose.  

*******************************************************************************/

#ifndef _SWAP_H
#define _SWAP_H

#define swap_uint_bytes(_ui_) \
{ \
        unsigned int _b_ = _ui_; \
        unsigned char *_f_ = (unsigned char *)&(_b_); \
        unsigned char *_t_ = (unsigned char *)&(_ui_); \
        _t_[3] = _f_[0]; \
        _t_[2] = _f_[1]; \
        _t_[1] = _f_[2]; \
        _t_[0] = _f_[3]; \
}

#define swap_int_bytes(_ui_) \
{ \
        int _b_ = _ui_; \
        unsigned char *_f_ = (unsigned char *)&(_b_); \
        unsigned char *_t_ = (unsigned char *)&(_ui_); \
        _t_[3] = _f_[0]; \
        _t_[2] = _f_[1]; \
        _t_[1] = _f_[2]; \
        _t_[0] = _f_[3]; \
}

#define swap_ushort_bytes(_us_) \
	{ \
	   unsigned short _b_ = _us_; \
	   unsigned char *_f_ = (unsigned char *)&(_b_); \
	   unsigned char *_t_ = (unsigned char *)&(_us_); \
	   _t_[1] = _f_[0]; \
	   _t_[0] = _f_[1]; \
	}

#define swap_short_bytes(_a_) \
	{ \
	   short _b_ = _a_; \
	   char *_f_ = (char *) &_b_; \
	   char *_t_ = (char *) &_a_; \
	   _t_[1] = _f_[0]; \
	   _t_[0] = _f_[1]; \
	}

#define swap_float_bytes(_flt_) \
{ \
        float _b_ = _flt_; \
        unsigned char *_f_ = (unsigned char *)&(_b_); \
        unsigned char *_t_ = (unsigned char *)&(_flt_); \
        _t_[3] = _f_[0]; \
        _t_[2] = _f_[1]; \
        _t_[1] = _f_[2]; \
        _t_[0] = _f_[3]; \
}

#define swap_short(_a_) \
	{ \
	   short _b_ = _a_; \
	   char *_f_ = (char *) &_b_; \
	   char *_t_ = (char *) &_a_; \
	   _t_[1] = _f_[0]; \
	   _t_[0] = _f_[1]; \
	}

#define swap_image_shorts(_data,_swidth,_sheight) \
	{ \
	unsigned short *_sdata = (unsigned short *)_data; \
	int _i,_wdlen=16; \
 	for (_i = 0;_i<(int)((_swidth/_wdlen)*_sheight);_i++) \
	   swap_short(_sdata[_i]);\
        }

#define swap_int(_a_, _b_) \
	{ \
           int _t_ = _a_; \
           _a_ = _b_; \
           _b_ = _t_; \
        }

#define swap_float(_a_, _b_) \
	{ \
           float _t_ = _a_; \
           _a_ = _b_; \
           _b_ = _t_; \
        }

#define swap_string(_a_, _b_) \
	{ \
	   char *_t_ = _a_; \
	   _a_ = _b_; \
	   _b_ = _t_; \
	}

#endif /* !_SWAP_H */
