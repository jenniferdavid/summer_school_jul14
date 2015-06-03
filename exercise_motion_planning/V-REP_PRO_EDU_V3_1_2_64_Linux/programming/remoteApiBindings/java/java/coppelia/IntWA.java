// PUT_VREP_REMOTEAPI_COPYRIGHT_NOTICE_HERE

package coppelia;

public class IntWA
{
    int[] w;

    public IntWA(int i)
    {
        w = new int[i];
    }

    public void initArray(int i)
    {
        w = new int[i];
    }

    public int getLength()
    {
        return w.length;
    }

    public int[] getNewArray(int i)
    {
        w = new int[i];
        return w;
    }

    public int[] getArray()
    {
        return w;
    }
	
    public String getStringFromArray()
    {
		String a="";
		for (int i=0;i<w.length;i++)
		{
			a+=Character.toString((char)((w[i] >>> 24)&255));
			a+=Character.toString((char)((w[i] >>> 16)&255));
			a+=Character.toString((char)((w[i] >>> 8)&255));
			a+=Character.toString((char)(w[i]&255));	
		}
        return a;
    }
	
    public void initArrayFromString(String a)
    {
        w = new int[a.length()/4];
		for (int i=0;i<a.length()/4;i++)
			w[i]=(int)((a.charAt(4*i+0) << 24) + (a.charAt(4*i+1) << 16) + (a.charAt(4*i+2) << 8) + a.charAt(4*i+3));			
    }
}
