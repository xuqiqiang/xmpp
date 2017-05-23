package com.snailstdio.software.xsdk.view;

import android.content.Context;
import android.util.AttributeSet;
import android.view.ViewStub;
import android.widget.ListView;

public class NoScrollListview extends ListView {

    public NoScrollListview(Context context, AttributeSet attrs) {
        super(context, attrs);
        addFooterView(new ViewStub(context));
    }

    /**
     * 设置不滚动
     */
    public void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int expandSpec = MeasureSpec.makeMeasureSpec(Integer.MAX_VALUE >> 2,
                MeasureSpec.AT_MOST);
        super.onMeasure(widthMeasureSpec, expandSpec);

    }

}