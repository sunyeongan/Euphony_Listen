//
// Created by desig on 2020-07-16.
//

#include <jni.h>
#include <string>
#include "arms/kiss_fftr.h"
#include <malloc.h>
#include <cstdlib>
#include <cmath>

#define MAX_SHORT 32767.0f

static inline float scale( kiss_fft_scalar val )
{
    if( val < 0 )
        return val * ( 1 / 32768.0f );
    else
        return val * ( 1 / 32767.0f );
}

struct KissFFT
{
    kiss_fftr_cfg config;
    kiss_fft_cpx* spectrum;
    int numSamples;
};

extern "C" JNIEXPORT jlong
Java_co_euphony_rx_KissFFT_create(JNIEnv *env, jobject thiz, jint numSamples) {
    KissFFT* fft = new KissFFT();

    fft->config = kiss_fftr_alloc(numSamples,0,nullptr,nullptr);
    fft->spectrum = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx) * (int)numSamples);
    fft->numSamples = numSamples;
    return (jlong)fft;
}

/*
 * Class:     euphony_lib_receiver_KissFFT
 * Method:    destroy
 * Signature: (J)V
 */
extern "C"
JNIEXPORT void JNICALL
Java_co_euphony_rx_KissFFT_destroy(JNIEnv *env, jobject thiz, jlong handle) {
    KissFFT* fft = (KissFFT*)handle;
    free(fft->config);
    free(fft->spectrum);
    free(fft);
}

extern "C"
JNIEXPORT void JNICALL
Java_co_euphony_rx_KissFFT_doSpectrum(JNIEnv *env, jobject thiz, jlong handle, jobject source,
                                      jint sample_idx, jlong mag_address, jlong dbfs_address) {
    KissFFT* fft = (KissFFT*)handle;
    kiss_fft_scalar* samples = (kiss_fft_scalar*)env->GetDirectBufferAddress( source );
    float* mag = (float*) mag_address;
    float* dbfs = (float*) dbfs_address;

    kiss_fftr( fft->config, samples, fft->spectrum );

    float re = scale(fft->spectrum[sample_idx].r) * fft->numSamples;
    float im = scale(fft->spectrum[sample_idx].i) * fft->numSamples;

    *mag = sqrtf( re * re + im * im ) / (fft->numSamples / 2);
    *dbfs = 10 * log10(4 * (re * re + im * im) / (fft->numSamples * fft->numSamples));
}

extern "C"
JNIEXPORT void JNICALL
Java_co_euphony_rx_KissFFT_doSpectrums(JNIEnv *env, jobject thiz, jlong handle,
                                              jobject source, jobject target) {
    KissFFT* fft = (KissFFT*)handle;
    kiss_fft_scalar* samples = (kiss_fft_scalar*)env->GetDirectBufferAddress( source );
    float* spectrum = (float*)env->GetDirectBufferAddress( target );


    kiss_fftr( fft->config, samples, fft->spectrum );    //<--- fatal signal 11 (SIGSEV) at 0x00000400

    int len = fft->numSamples / 2 + 1;  // <=---  <--- fatal signal 11 (SIGSEV) at 0x00000408
    int len_halfOfNumSamples = fft->numSamples / 2;
    // int len = 6; // <-- for debugging
    int start = len * (17500.0 / 22050.0);
    for( int i = start; i < len; i++ )
    {
        float re = scale(fft->spectrum[i].r) * fft->numSamples;
        float im = scale(fft->spectrum[i].i) * fft->numSamples;

        // mag(k) = 2 * SQUARE_ROOT(re * re + im * im) / number of samples
        spectrum[i] = sqrtf(re*re + im*im) / len_halfOfNumSamples;

        /*
        if( i > 0 ) spectrum[i] = sqrtf(re*re + im*im) / (fft->numSamples / 2);
        else spectrum[i] = sqrtf(re*re + im*im) / (fft->numSamples);
        */
    }
}


/*
 * Class:     euphony_lib_receiver_KissFFT
 * Method:    spectrum_for_phase
 * Signature: (JLjava/nio/ShortBuffer;Ljava/nio/FloatBuffer;)V
 */
extern "C"
JNIEXPORT void JNICALL
Java_co_euphony_rx_KissFFT_spectrum_1for_1phase(JNIEnv *env, jobject thiz, jlong handle,
                                                       jobject source, jobject target) {
    KissFFT* fft = (KissFFT*)handle;
    kiss_fft_scalar* samples = (kiss_fft_scalar*)env->GetDirectBufferAddress( source );
    float* spectrum = (float*)env->GetDirectBufferAddress( target );

    kiss_fftr( fft->config, samples, fft->spectrum );

    int len = fft->numSamples / 2 + 1;
    int start = (int)(len * (16500.0 / 22050.0));

    for( int i = start; i < len; i++ )
    {
        float re = scale(fft->spectrum[i].r) * fft->numSamples;
        float im = scale(fft->spectrum[i].i) * fft->numSamples;

        spectrum[i] = atan2(im,re) * 180.0 / 3.141592;
    }
}

/*
 * Class:     euphony_lib_receiver_KissFFT
 * Method:    getRealPart
 * Signature: (JLjava/nio/ShortBuffer;)V
 */
extern "C"
JNIEXPORT void JNICALL
Java_co_euphony_rx_KissFFT_getRealPart(JNIEnv *env, jobject thiz, jlong handle,
                                              jobject real) {
    KissFFT* fft = (KissFFT*)handle;
    short* target = (short*)env->GetDirectBufferAddress(real);
    for( int i = 0; i < fft->numSamples / 2; i++ )
        target[i] = fft->spectrum[i].r;
}

/*
 * Class:     euphony_lib_receiver_KissFFT
 * Method:    getImagPart
 * Signature: (JLjava/nio/ShortBuffer;)V
 */
extern "C"
JNIEXPORT void JNICALL
Java_co_euphony_rx_KissFFT_getImagePart(JNIEnv *env, jobject thiz, jlong handle,
                                               jobject imag) {
    KissFFT* fft = (KissFFT*)handle;
    short* target = (short*)env->GetDirectBufferAddress(imag);
    for( int i = 0; i < fft->numSamples / 2; i++ )
        target[i] = fft->spectrum[i].i;
}