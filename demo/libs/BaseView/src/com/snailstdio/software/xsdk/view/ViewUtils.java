package com.snailstdio.software.xsdk.view;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.GradientDrawable;
import android.graphics.drawable.StateListDrawable;
import android.os.Build;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.TextView;

import com.snailstdio.software.xsdk.baseview.R;
import com.snailstdio.software.xsdk.logger.Logger;
import com.snailstdio.software.xsdk.utils.Cache;
import com.snailstdio.software.xsdk.utils.DisplayUtils;

public class ViewUtils {

    public static void setShapeViewListener(Context context, View view,
            final OnClickListener listener) {
        ThemeManager mThemeManager = ThemeManager.getInstance();
        if (mThemeManager.isDefaultTheme()) {
            view.setOnClickListener(listener);
        } else {
            view.setOnTouchListener(new OnTouchClickListener(
                    context,
                    view,
                    context.getResources()
                            .getColor(
                                    ThemeManager.getInstance().setting_menu_pressed_color),
                    context.getResources().getColor(
                            ThemeManager.getInstance().background_color),
                    listener));
        }
    }

    public static void setSettingOptionsListener(Context context,
            View[] options, final OnClickListener listener) {
        ThemeManager mThemeManager = ThemeManager.getInstance();
        if (mThemeManager.isDefaultTheme()) {
            for (View option : options) {
                option.setOnClickListener(listener);
            }
        } else {
            for (View option : options) {
                option.setOnTouchListener(new OnTouchClickListener(
                        context,
                        option,
                        context.getResources().getColor(R.color.white),
                        context.getResources()
                                .getColor(
                                        ThemeManager.getInstance().setting_menu_pressed_color),
                        listener));
            }
        }
    }

    public static class OnTouchClickListener implements OnTouchListener {

        private View touchView;
        private OnClickListener listener;
        private boolean isCancel;
        private int colorNormal;
        private int colorPressed;
        private float dy;
        private float offset;

        public OnTouchClickListener(Context context, View touchView,
                int colorNormal, int colorPressed, OnClickListener listener) {
            this.touchView = touchView;
            this.listener = listener;
            this.colorNormal = colorNormal;
            this.colorPressed = colorPressed;
            offset = DisplayUtils.dip2px(context, 20);
            onFocusChanged(false);
        }

        @Override
        public boolean onTouch(View v, MotionEvent event) {
            Logger.d(event.getAction() + "," + event.getY());
            if (event.getAction() == MotionEvent.ACTION_DOWN) {
                isCancel = false;
                dy = event.getY();
                onFocusChanged(true);
                return true;
            } else if (event.getAction() == MotionEvent.ACTION_MOVE) {
                if (!isCancel) {
                    if (Math.abs(event.getY() - dy) > offset) {
                        isCancel = true;
                        onFocusChanged(false);
                    }
                }
            } else if (event.getAction() == MotionEvent.ACTION_CANCEL) {
                if (!isCancel) {
                    isCancel = true;
                    onFocusChanged(false);
                }
            } else if (event.getAction() == MotionEvent.ACTION_UP) {
                if (isCancel) {
                    return false;
                }
                listener.onClick(touchView);
                onFocusChanged(false);
            }
            return false;
        }

        private void onFocusChanged(boolean isFocus) {
            StateListDrawable sld = (StateListDrawable) touchView
                    .getBackground();
            GradientDrawable gd = (GradientDrawable) sld.getCurrent();
            if (isFocus) {
                gd.setColor(colorPressed);
            } else {
                gd.setColor(colorNormal);
            }
        }

    }

    public static void initSettingCheckBox(Activity context, int optionId,
            int checkBoxId, final String key, boolean defaultChecked) {
        initSettingCheckBox(context, context.findViewById(optionId),
                (CheckBox) context.findViewById(checkBoxId), key,
                defaultChecked);
    }

    public static void initSettingCheckBox(Context context, View option,
            final CheckBox checkBox, final String key, boolean defaultChecked) {

        ThemeManager mThemeManager = ThemeManager.getInstance();
        if (!mThemeManager.isDefaultTheme()) {
            checkBox.setButtonDrawable(mThemeManager.switch_background);
        }

        checkBox.setChecked(Cache.read_Boolean(key, defaultChecked));
        checkBox.setOnCheckedChangeListener(new OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                    boolean isChecked) {
                Cache.write_Boolean(key, isChecked);
            }

        });

        OnClickListener listener = new OnClickListener() {
            @Override
            public void onClick(View v) {
                checkBox.setChecked(!checkBox.isChecked());
            }
        };

        if (mThemeManager.isDefaultTheme()) {
            option.setOnClickListener(listener);
        } else {
            option.setOnTouchListener(new OnTouchClickListener(
                    context,
                    option,
                    context.getResources().getColor(R.color.white),
                    context.getResources()
                            .getColor(
                                    ThemeManager.getInstance().setting_menu_pressed_color),
                    listener));
        }
    }

    @TargetApi(Build.VERSION_CODES.JELLY_BEAN)
    public static void setBackgroundDrawable(View view, Drawable drawable) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN) {
            view.setBackgroundDrawable(drawable);
        } else {
            view.setBackground(drawable);
        }
    }

    @TargetApi(Build.VERSION_CODES.JELLY_BEAN)
    public static float getLineSpacingExtra(Context context, TextView view) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
            return view.getLineSpacingExtra();
        } else {
            return DisplayUtils.dip2px(context, 8);
        }
    }
}