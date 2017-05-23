package com.snailstdio.software.xsdk.activity;

import java.lang.reflect.Field;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager.NameNotFoundException;

import com.snailstdio.software.xsdk.logger.Logger;

public class ActivityUtils {
	public static void startActivity(Activity fromActivity, Intent intent){
		fromActivity.startActivity(intent);
		fromActivity.overridePendingTransition(android.R.anim.fade_in,android.R.anim.fade_out);
	}
	
	public static void startActivityForResult(Activity fromActivity, Intent intent, int requestCode){
		fromActivity.startActivityForResult(intent, requestCode);
		fromActivity.overridePendingTransition(android.R.anim.fade_in,android.R.anim.fade_out);
	}
	
	public static boolean finish(Activity activity){
		activity.finish();
		activity.overridePendingTransition(android.R.anim.fade_in, android.R.anim.fade_out);
		return true;
	}
	
	public static String getVersionName(Context context) {
        String versionName = "Unknown";
        try {
            versionName = context.getPackageManager().getPackageInfo(
                    context.getPackageName(), 0).versionName;
        } catch (NameNotFoundException e) {
            e.printStackTrace();
        }
        return versionName;
    }
	
	public static int getStatusBarHeight(Context context) {
        int result = 0;
        int resourceId = context.getResources().getIdentifier("status_bar_height", "dimen", "android");
        if (resourceId > 0) {
            result = context.getResources().getDimensionPixelSize(resourceId);
        }
        else{
            result = getStatusBarHeight2(context);
        }
        return result;
      }
    
	public static int getStatusBarHeight2(Context context) {
        Class<?> c =  null; 
        Object obj =  null; 
        Field field =  null; 
        int  x = 0, sbar =  0; 
        try  { 
            c = Class.forName("com.android.internal.R$dimen");
            obj = c.newInstance();
            field = c.getField("status_bar_height");
            x = Integer.parseInt(field.get(obj).toString());
            sbar = context.getResources().getDimensionPixelSize(x);
        } catch(Exception e) {
            e.printStackTrace();
        }
        return sbar;
    }
	
	public static boolean isApkInstalled(Context context, String pkg_name,
            int version) {
        PackageInfo packageInfo = null;
        try {
            packageInfo = context.getPackageManager().getPackageInfo(pkg_name,
                    0);
            Logger.d(packageInfo.versionCode + "");
        } catch (NameNotFoundException e) {
            e.printStackTrace();
        }
        if (packageInfo != null && version <= packageInfo.versionCode) {
            return true;
        }
        return false;
    }
}