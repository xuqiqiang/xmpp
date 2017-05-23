package com.snailstdio.software.xsdk.view;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.snailstdio.software.xsdk.baseview.R;

public class ToastMaster {

    private static Toast sToast = null;

    private ToastMaster() {
    }

    public static void showToast(Context context, CharSequence text) {
        Toast toast = new Toast(context);
        toast.setDuration(Toast.LENGTH_SHORT);
        toast.setView(getView(context, text));
        setToast(toast);
        toast.show();
    }

    public static void showToast(Context context, int textId) {
        showToast(context, context.getString(textId));
    }

    public static void showToastLong(Context context, CharSequence text) {
        Toast toast = new Toast(context);
        toast.setDuration(Toast.LENGTH_LONG);
        toast.setView(getView(context, text));
        setToast(toast);
        toast.show();
    }

    public static void showToastLowPriority(Context context, CharSequence text) {
        Toast toast = new Toast(context);
        toast.setDuration(Toast.LENGTH_SHORT);
        toast.setView(getView(context, text));
        toast.show();
    }

    public static void showToastLowPriority(Context context, int textId) {
        showToastLowPriority(context, context.getString(textId));
    }

    public static void setToast(Toast toast) {
        if (sToast != null)
            sToast.cancel();
        sToast = toast;
    }

    public static void cancelToast() {
        if (sToast != null)
            sToast.cancel();
        sToast = null;
    }

    private static View getView(Context context, CharSequence text) {
        // if (view == null) {
        // LayoutInflater inflater = LayoutInflater.from(context);
        // view = inflater.inflate(R.layout.toast, null);
        // }
        LayoutInflater inflater = LayoutInflater.from(context);
        View view = inflater.inflate(R.layout.toast, null);

        RelativeLayout toast_layout = (RelativeLayout) view
                .findViewById(R.id.toast_layout);
        ThemeManager mThemeManager = ThemeManager.getInstance();
        if (!mThemeManager.isDefaultTheme()) {
            toast_layout.setBackgroundResource(mThemeManager.toast_background);
        }

        TextView tv_text = (TextView) view.findViewById(R.id.text);
        tv_text.setText(text);
        return view;
    }

}
