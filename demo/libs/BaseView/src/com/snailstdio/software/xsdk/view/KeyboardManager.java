package com.snailstdio.software.xsdk.view;

import android.app.Activity;
import android.content.Context;
import android.graphics.Rect;
import android.os.Handler;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;

public class KeyboardManager {
    private static final String TAG = "KeyboardManager";

    private Activity context;
    private InputMethodManager mInputMethodManager;

    private int mLastBottom = -1;
    private int mBottomWhenKeyboardHidden;

    private boolean isInitOnGlobalLayoutListener;
    private OnKeyboardStatusChangeListener listener;

    public interface OnKeyboardStatusChangeListener {
        public void onKeyboardStatusChange(boolean isShow);
    }

    public KeyboardManager(Activity context) {
        this.context = context;
        mInputMethodManager = (InputMethodManager) context
                .getSystemService(Context.INPUT_METHOD_SERVICE);
    }

    public void setKeyboardHidden() {
        context.getWindow().setSoftInputMode(
                WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);
    }

    public void hiddenKeyboard(View view) {
        mInputMethodManager.hideSoftInputFromWindow(view.getWindowToken(), 0);// InputMethodManager.HIDE_NOT_ALWAYS);
    }

    public void showKeyboard(final View view) {
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                mInputMethodManager.showSoftInput(view,
                        InputMethodManager.RESULT_SHOWN);
            }
        }, 200);
    }

    public void setOnKeyboardStatusChangeListener(
            OnKeyboardStatusChangeListener listener) {
        this.listener = listener;

        if (!isInitOnGlobalLayoutListener && listener != null) {
            isInitOnGlobalLayoutListener = true;
            final View rootView = context.getWindow().getDecorView();

            rootView.getViewTreeObserver().addOnGlobalLayoutListener(
                    new ViewTreeObserver.OnGlobalLayoutListener() {
                        public void onGlobalLayout() {

                            Rect r = new Rect();
                            rootView.getWindowVisibleDisplayFrame(r);

                            int bottom = r.bottom;

                            if (bottom == mLastBottom) {
                                return;
                            }
                            if (bottom > mBottomWhenKeyboardHidden) {
                                mBottomWhenKeyboardHidden = bottom;
                            }
                            if (mLastBottom == -1) {
                                mLastBottom = bottom;
                                return;
                            }
                            mLastBottom = bottom;
                            if (KeyboardManager.this.listener != null) {
                                KeyboardManager.this.listener
                                        .onKeyboardStatusChange(bottom < mBottomWhenKeyboardHidden);
                            }

                        }
                    });
        }
    }
}