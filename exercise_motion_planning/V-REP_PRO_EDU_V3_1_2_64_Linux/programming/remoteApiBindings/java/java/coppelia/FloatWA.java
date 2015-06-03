// PUT_VREP_REMOTEAPI_COPYRIGHT_NOTICE_HERE

package coppelia;

public class FloatWA
{
    float[] w;

    public FloatWA(int i)
    {
        w = new float[i];
    }

    public void initArray(int i)
    {
        w = new float[i];
    }

    public float[] getArray()
    {
        return w;
    }

    public int getLength()
    {
        return w.length;
    }

    public float[] getNewArray(int i)
    {
        w = new float[i];
        for (int k = 0; k < i; k++)
            w[k] = 0.0f;
        return w;
    }

    public String getStringFromArray()
    {
		String a="";
		for (int i=0;i<w.length;i++)
		{
			int iw=Float.floatToIntBits(w[i]);
			a+=Character.toString((char)((iw >>> 24)&255));
			a+=Character.toString((char)((iw >>> 16)&255));
			a+=Character.toString((char)((iw >>> 8)&255));
			a+=Character.toString((char)(iw&255));	
		}
        return a;
    }
	
    public void initArrayFromString(String a)
    {
        w = new float[a.length()/4];
		for (int i=0;i<a.length()/4;i++)
		{
			int iw=(int)((a.charAt(4*i+0) << 24) + (a.charAt(4*i+1) << 16) + (a.charAt(4*i+2) << 8) + a.charAt(4*i+3));
			w[i]=Float.intBitsToFloat(iw);
		}				
    }
}