#ifndef __MY_FFT_H__
#define __MY_FFT_H__


typedef struct complex_s //复数类型
{
  float real;		//实部
  float imag;		//虚部
}complex_s;
 
#define PI 3.1415926535897932384626433832795028841971
 
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

///////////////////////////////////////////
void conjugate_complex(int n,complex_s in[],complex_s out[]);
void c_plus(complex_s a,complex_s b,complex_s *c);//复数加
void c_mul(complex_s a,complex_s b,complex_s *c) ;//复数乘
void c_sub(complex_s a,complex_s b,complex_s *c);	//复数减法
void c_div(complex_s a,complex_s b,complex_s *c);	//复数除法
void fft(int N,complex_s f[]);//傅立叶变换 输出也存在数组f中
void ifft(int N,complex_s f[]); // 傅里叶逆变换
void c_abs(complex_s f[],float out[],int n);//复数数组取模
////////////////////////////////////////////

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif
