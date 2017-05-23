package com.snailstdio.software.xsdk.activity;

import java.util.ArrayList;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.Window;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.snailstdio.software.xsdk.logger.Logger;
import com.snailstdio.software.xsdk.view.PopMenu;
import com.snailstdio.software.xsdk.view.PopMenu.OnPopMenuItemClickListener;
import com.snailstdio.software.xsdk.view.SystemBarTintManager;
import com.snailstdio.software.xsdk.view.ThemeManager;
import com.snailstdio.software.xsdk.view.ToastMaster;
import com.umeng.analytics.MobclickAgent;

public class BaseActivity extends Activity {

    private boolean showMore;
    protected ArrayList<String> popMenu_itemList;
    protected ArrayList<Integer> popMenu_itemListDrawable;
    protected OnPopMenuItemClickListener popMenu_listener;
    protected PopMenu popMenu;
    private TextView titleText;
    protected ImageButton btn_title_back;
    protected ImageButton btn_title_more;

    public static final int RESULT_UNKNOWN_ERROR = -99;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        SystemBarTintManager.initSystemBar(this);
        initContentView();

        initTitleBar();
    }

    protected void initContentView() {
        setContentView(R.layout.base_layout);
    }

    protected void setView(int layoutResID) {
        LayoutInflater mInflater = getLayoutInflater();
        View view = mInflater.inflate(layoutResID, null);
        setView(view);
    }

    protected void setView(View view) {
        LinearLayout content_layout = (LinearLayout) findViewById(R.id.content_layout);
        content_layout.removeAllViews();
        content_layout.addView(view, new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.MATCH_PARENT));
    }

    @Override
    protected void onResume() {
        super.onResume();
        MobclickAgent.onResume(this);
    }

    @Override
    protected void onPause() {
        super.onPause();
        MobclickAgent.onPause(this);
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    protected void initTitleBar() {

        RelativeLayout base_title_bar_layout = (RelativeLayout) (findViewById(R.id.base_title_bar_layout));
        // base_title_bar_layout.setBackgroundColor(Data.mTypedArray.getColor(
        // R.styleable.MyStyle_background_color,getResources().getColor(R.color.base_title_bar_background)));
        ThemeManager mThemeManager = ThemeManager.getInstance();
        if (!mThemeManager.isDefaultTheme()) {
            base_title_bar_layout.setBackgroundColor(getResources().getColor(
                    mThemeManager.background_color));
        }

        titleText = (TextView) (findViewById(R.id.title));

        initTitle(titleText);

        btn_title_back = (ImageButton) findViewById(R.id.btn_title_back);
        btn_title_back.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                onTitleBack();
            }
        });

        initPopMenu();

        btn_title_more = (ImageButton) findViewById(R.id.btn_title_more);
        if (!showMore) {
            btn_title_more.setVisibility(View.GONE);
        } else {
            btn_title_more.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    popMenu.showAsDropDown(v);
                }
            });
        }

    }

    protected void initTitle(TextView titleText) {
        setTitle(getTitle());
    }

    @Override
    public void setTitle(CharSequence title) {
        super.setTitle(title);
        titleText.setText(title);
    }

    protected void setOnTitleTextClickListener(OnClickListener listener) {
        titleText.setOnTouchListener(new OnTouchListener() {

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (event.getAction() == MotionEvent.ACTION_DOWN) {
                    titleText.setTextColor(Color.RED);
                } else if (event.getAction() == MotionEvent.ACTION_UP) {
                    titleText.setTextColor(Color.WHITE);
                }
                return false;
            }

        });
        titleText.setOnClickListener(listener);
    }

    protected void onTitleBack() {
        finish();
    }

    protected void initPopMenu() {
        popMenu_itemList = new ArrayList<String>();
        popMenu_itemListDrawable = new ArrayList<Integer>();
        popMenu_listener = initPopmenuData(popMenu_itemList,
                popMenu_itemListDrawable);
        if (popMenu_itemList.size() == 0) {
            showMore = false;
        } else {
            showMore = true;
            // 初始化弹出菜单
            popMenu = new PopMenu(this, PopMenu.STYLE_BLUE);
            popMenu.setItems(popMenu_itemList);
            popMenu.setItemsDrawable(popMenu_itemListDrawable);
            popMenu.setOnItemClickListener(popMenu_listener);
        }

    }

    public void setPopMenu(PopMenu popMenu) {
        this.popMenu = popMenu;
        if (popMenu != null) {
            showMore = true;
            btn_title_more.setVisibility(View.VISIBLE);
            btn_title_more.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    BaseActivity.this.popMenu.showAsDropDown(v);
                }
            });
        }
    }

    protected OnPopMenuItemClickListener initPopmenuData(
            ArrayList<String> popMenu_itemList,
            ArrayList<Integer> popMenu_itemListDrawable) {
        return null;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        Logger.d("requestCode : " + requestCode + ", resultCode : "
                + resultCode);
        if (resultCode == RESULT_UNKNOWN_ERROR)
            ToastMaster.showToast(this, R.string.unknown_error);
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_MENU) {
            if (showMore) {
                popMenu.showAsDropDown(btn_title_more);
                return true;
            }
        } else if (keyCode == KeyEvent.KEYCODE_BACK) {
            onTitleBack();
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }
}