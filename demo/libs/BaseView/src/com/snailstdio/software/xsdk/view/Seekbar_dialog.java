package com.snailstdio.software.xsdk.view;

import android.app.Activity;
import android.content.DialogInterface.OnClickListener;
import android.text.InputType;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.view.WindowManager;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;

import com.snailstdio.software.xsdk.baseview.R;
import com.snailstdio.software.xsdk.utils.DisplayUtils;

/**
 * 设置数值的对话框
 * 
 * @author xqq
 * */
public class Seekbar_dialog {

    private CustomDialog.Builder customBuilder = null;
    private Activity activity;

    private EditText et;

    private OnCustomSeekBarChangeListener onCustomSeekBarChangeListener = null;

    public Seekbar_dialog(Activity context, String title, String help, int now,
            int max) {

        this.activity = context;

        customBuilder = new CustomDialog.Builder(context);
        customBuilder.setTitle(title).setContentView(initView(help, now, max));

    }

    private View initView(String help, int now, int max) {
        LinearLayout view = new LinearLayout(activity);
        view.setOrientation(LinearLayout.VERTICAL);
        final SeekBar sb = new SeekBar(activity);
        sb.setMax(max);
        sb.setProgress(now);

        sb.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress,
                    boolean fromUser) {
                et.setText(sb.getProgress() + "");
                if (onCustomSeekBarChangeListener != null)
                    onCustomSeekBarChangeListener.onProgressChanged(sb
                            .getProgress());
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });

        view.addView(sb, new LinearLayout.LayoutParams(
                LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT));

        LinearLayout view_point = new LinearLayout(activity);
        view_point.setGravity(Gravity.CENTER);
        view_point.setOrientation(LinearLayout.HORIZONTAL);

        TextView tv = new TextView(activity);
        tv.setTextSize(20);
        tv.setTextColor(0xff000000);
        tv.setText(help);

        view_point.addView(tv, new LinearLayout.LayoutParams(
                LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));

        et = new EditText(activity);
        et.setRawInputType(InputType.TYPE_CLASS_NUMBER);
        et.setText(now + "");
        et.setTextColor(0xff000000);
        et.setBackgroundColor(0xffffffff);
        int padding = (int) DisplayUtils.dip2px(activity, 8);
        et.setPadding(padding, padding, padding, padding);
        view_point.addView(et, new LinearLayout.LayoutParams(
                LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));

        sb.setFocusable(true);

        view.addView(view_point, new LinearLayout.LayoutParams(
                LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT));
        return view;
    }

    public String getText() {
        return et.getText().toString();
    }

    public void setOnClickListener(OnClickListener on_positive_listener,
            OnClickListener on_negative_listener) {
        customBuilder.setPositiveButton(R.string.ok, on_positive_listener);
        if (on_negative_listener != null) {
            customBuilder.setNegativeButton(R.string.cancel,
                    on_negative_listener);
            customBuilder.setOnKeyBackListener(on_negative_listener);
        } else {
            customBuilder.setNegativeButton(R.string.cancel, null);
        }
    }

    public void setOnCustomSeekBarChangeListener(
            OnCustomSeekBarChangeListener onCustomSeekBarChangeListener) {
        this.onCustomSeekBarChangeListener = onCustomSeekBarChangeListener;
    }

    public void setEditText(String str) {
        et.setText(str);
    }

    public void show() {
        CustomDialog dialog = customBuilder.create();
        dialog.show();

        dialog.getWindow().setSoftInputMode(
                WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);

    }

    public interface OnCustomSeekBarChangeListener {
        public void onProgressChanged(int progress);
    }
}