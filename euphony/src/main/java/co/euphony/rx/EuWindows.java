package co.euphony.rx;

import java.nio.ByteBuffer;
import java.util.*;

public class EuWindows {
	private short mWindowNumber;
	
	//windows
	public static final int RECTANGULAR = 0;
	public static final int TRIANGLAR = 1;
	public static final int HANNING = 2;
	public static final int HAMMING =3;
	public static final int BLACKMAN = 4;
	public static final int KAISER =5;
	
	private ByteBuffer mBuffer = null;
	private	int	mBufferSize	= 0;
	private byte[] mRawData;
	private double KaiserAlph = 32.0;        
	private double KaiserWindowSize = 45;
	private double HammingAlph = 54.0;   
	private double BlackmanAlph = 0.16;



	public EuWindows(short mWindowNumber, ByteBuffer buffer, int bufferSize)
	{
		mBuffer	=	buffer;
		mBufferSize	=	bufferSize;
		mRawData = buffer.array();
		mWindowNumber = mWindowNumber;
	}

	void Processor()
	{
		if ((mBuffer != null)&&(mBufferSize != 0))// check buffer buffersize
		{
			switch (mWindowNumber)
			{
			case RECTANGULAR:
				RectangularWindow();
				break;
			case TRIANGLAR:
				TrianglarWindow();
				break;	
			case HANNING:
				HanningWindow();
				break;
			case HAMMING:
				HammingWindow();
				break;
			case BLACKMAN:
				BlackmanWindow();
				break;
			case KAISER:
				KaiserWindow();
				break;
			}
		}
	}
	
	void RectangularWindow()
	{
		for(int i = 0; i < mRawData.length; i++)
			mRawData[i] = (byte) ( mRawData[i] * 1.0 );
		mBuffer = ByteBuffer.wrap(mRawData);
	
	}
	
	void TrianglarWindow(){
		for(int i = 0; i < mRawData.length; i++)
			mRawData[i] = (byte) ( ( mRawData[i] ) * (( mRawData.length / 2.0 - Math.abs( i - ( mRawData.length - 1.0 ) / 2.0 ) ) / ( mRawData.length / 2.0 ))) ;
		mBuffer = ByteBuffer.wrap(mRawData);
	}
		
	void HanningWindow()
	{
		for(int i = 0; i < mRawData.length; i++)
			mRawData[i] = (byte)( ( mRawData[i] ) * ( 0.5 * ( 1 - Math.cos( ( 2 * Math.PI * i ) / ( 512 - 1 ) ) ) ) ) ;	
		mBuffer = ByteBuffer.wrap(mRawData);
	}

	void HammingWindow(){
		for(int i = 0; i < mRawData.length; i++)
			mRawData[i] = (byte)( ( mRawData[i] ) * ( HammingAlph + ( 1.0 - HammingAlph ) * Math.cos( 2.0 * Math.PI * i / mRawData.length + Math.PI ) )) ;
		mBuffer = ByteBuffer.wrap(mRawData);    
	}

	void BlackmanWindow(){
		for(int i = 0; i < mRawData.length; i++)
			mRawData[i] = (byte) ( ( mRawData[i] ) * (( 1.0 - BlackmanAlph ) / 2.0 - 0.5 * Math.cos( 2.0 * Math.PI * i / ( mRawData.length - 1.0 ) ) 
			+ ( BlackmanAlph / 2.0 ) * Math.cos( 4.0 * Math.PI * i / ( mRawData.length - 1.0 ) ))) ;
		mBuffer = ByteBuffer.wrap(mRawData);
	}
	
	void KaiserWindow(){
		double n = KaiserWindowSize / 2.0;
		for(int i = 0; i < mRawData.length; i++)
		{
			mRawData[i] = (byte) ( ( mRawData[i] ) * (Util.I0( KaiserAlph * Math.sqrt( 1.0 - Math.pow(2.0 * ( (double) i - n ) / KaiserWindowSize , 2 ) ) ) / Util.I0(KaiserAlph)));		
		}
		mBuffer = ByteBuffer.wrap(mRawData);
	}
	
	static class Util{
		public static double I0(double x)
		{
			double	eps = 10E-9;
			int	n = 1;
			double	S = 1.0;
			double	D = 1.0;
			while (D > eps * S)
			{
				double	T = x / (2.0 * n);
				n++;
				D *= (T * T);
				S += D;
			}
			return S;
		}
	}
	
	public ByteBuffer getBuffer() {
		return mBuffer;
	}

	public void setBuffer(ByteBuffer mBuffer) {
		this.mBuffer = mBuffer;
	}

	public int getBufferSize() {
		return mBufferSize;
	}

	public void setBufferSize(int mBufferSize) {
		this.mBufferSize = mBufferSize;
	}

	public short getWindowNumber() {
		return mWindowNumber;
	}

	public void setWindowNumber(short mWindowNumber) {
		this.mWindowNumber = mWindowNumber;
	}
	public double getKaiserAlph() {
		return KaiserAlph;
	}

	public void setKaiserAlph(double kaiserAlph) {
		KaiserAlph = kaiserAlph;
	}

	public double getKaiserWindowSize() {
		return KaiserWindowSize;
	}

	public void setKaiserWindowSize(double kaiserWindowSize) {
		KaiserWindowSize = kaiserWindowSize;
	}

	public double getHammingAlph() {
		return HammingAlph;
	}

	public void setHammingAlph(double hammingAlph) {
		HammingAlph = hammingAlph;
	}	
	
	public double getBlackmanAlph() {
		return BlackmanAlph;
	}

	public void setBlackmanAlph(double blackmanAlph) {
		BlackmanAlph = blackmanAlph;
	}	
}