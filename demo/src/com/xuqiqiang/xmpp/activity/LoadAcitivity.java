package com.xuqiqiang.xmpp.activity;

import com.xuqiqiang.xmpp.R;

import android.app.Activity;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.animation.LinearInterpolator;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

public abstract class LoadAcitivity extends Activity {

    private ImageView loadAnimation;

    public int scene;
    public static final int SCENE_LOGIN = 0, SCENE_WAIT = 1,
            SCENE_CONMPLETE = 2;

    private String load_message;

    protected final int MESSAGE_COMPLETE = 1, MESSAGE_ERROR = 2;

    protected Handler mHandler = new Handler() {
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MESSAGE_COMPLETE:
                onHandler_MESSAGE_COMPLETE();
                break;
            case MESSAGE_ERROR:
                onHandler_MESSAGE_ERROR();
                break;
            }
            super.handleMessage(msg);
        }
    };

    protected void sendHandlerMessage(int message) {
        Message msg = new Message();
        msg.what = message;
        mHandler.sendMessage(msg);
    }

    protected abstract void onHandler_MESSAGE_COMPLETE();

    protected abstract void onHandler_MESSAGE_ERROR();

    public void onLoad(String message) {

        onLoad();
        TextView tvWait = (TextView) (findViewById(R.id.waitTextView));
        tvWait.setText(message);

        load_message = message;

    }

    public void onLoad() {

        scene = SCENE_WAIT;

        setContentView(R.layout.load_layout); // 加载界面
        loadAnimation = (ImageView) findViewById(R.id.loadAnimation);
        loadAnimation.setBackgroundResource(R.drawable.loading_gif);
        Animation mAnimation = AnimationUtils
                .loadAnimation(this, R.anim.rotate); // 旋转动画
        mAnimation.setInterpolator(new LinearInterpolator());
        loadAnimation.startAnimation(mAnimation); // 初始化动画

    }

    protected abstract void onLoadEvent(); // 重试

    protected void setTitle(String str) {
        TextView title = (TextView) findViewById(R.id.title);
        title.setText(str);
    }

    protected void setTitle(View view, String str) {
        TextView title = (TextView) view.findViewById(R.id.title);
        title.setText(str);
    }

    protected void onCompleted() {
        loadAnimation.clearAnimation();

        scene = SCENE_CONMPLETE;
    }

    protected void showMessage(String message) {
        TextView tvWait = (TextView) (findViewById(R.id.waitTextView));
        tvWait.setText(message);
        loadAnimation.clearAnimation();
        loadAnimation.setVisibility(View.GONE);
    }

    public void showError(String message) {
        TextView tvWait = (TextView) (findViewById(R.id.waitTextView));
        tvWait.setText(message);

        loadAnimation.clearAnimation();
        loadAnimation.setBackgroundResource(R.drawable.icon_cry);

        Button btWait = (Button) (findViewById(R.id.waitButton));
        btWait.setVisibility(View.VISIBLE);
        btWait.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                onLoad(load_message);
                onLoadEvent();
            }
        });
    }
}