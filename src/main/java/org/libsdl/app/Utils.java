package org.libsdl.app;

import org.apache.http.Header;
import org.apache.http.HttpResponse;
import org.apache.http.StatusLine;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.BasicHttpParams;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;
import org.apache.http.util.EntityUtils;
import org.json.JSONObject;

public class Utils {
	 static public JSONObject requestUrl(String baseUrl, String url ,int timeout)
	 {
			try{
				final String strUrl = baseUrl + url;
				DefaultHttpClient httpClient = null;
				if(timeout > -1) //for check alive,about 5 mins 
				{
					HttpParams httpParams = new BasicHttpParams();
					HttpConnectionParams.setConnectionTimeout(httpParams, 2000);
				    HttpConnectionParams.setSoTimeout(httpParams, timeout);
				    httpClient = new DefaultHttpClient(httpParams);
				}else
					httpClient = new DefaultHttpClient();
			
				
				HttpPost httppost = new HttpPost(strUrl);
				
				httppost.setHeader("Accept", "application/json");
				httppost.setHeader("Content-type", "application/json");
				 
				HttpResponse response = httpClient.execute(httppost);
				Header cookieHdr = response.getFirstHeader("Set-Cookie");
			 
				
				StatusLine status = response.getStatusLine();
				if(status.getStatusCode() != 200)
				{ 
					 return null ;
				}
				
				String strRes = EntityUtils.toString(response.getEntity(), "UTF-8");
				
				return new JSONObject(strRes);
		 
			
			}catch(Exception e)
			{
				e.printStackTrace();
			}
		
			return null;
	 }

}
