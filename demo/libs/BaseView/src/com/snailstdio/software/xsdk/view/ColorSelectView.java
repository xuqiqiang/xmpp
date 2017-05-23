package com.snailstdio.software.xsdk.view;

import android.app.Activity;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.os.Handler;
import android.util.DisplayMetrics;
import android.view.MotionEvent;
import android.view.View;

public class ColorSelectView extends View {
    private Paint mPaint;
    private Paint mSelectedPaint;
    private Paint mShadowPaint;
    private static final int[] mColors = new int[] { 0xFFE6C972, 0xFFE6D296,
            0xFFF9F5ED, 0xFFFFFFFF, 0xFF72E686, 0xFF72E6BF, 0xFF72D4E6,
            0xFFCCCCCC, 0xFF729AE6, 0xFF8672E6, 0xFFBF72E6, 0xFF999999 };

    public static final int DEFAULT_COLOR_ID = 3;

    private static int COLUMNS = 4;

    private OnColorSelectedListener mListener;

    private float width, height;
    private float circle_radius;
    private float circle_start_x;
    private float circle_start_y;

    private int mSelectColorId;

    private int mTouchId;

    public interface OnColorSelectedListener {
        public static int DELAY = 100;

        public void onColorSelected(int colorId, int color);
    }

    public static int getColor(int id) {
        return mColors[id];
    }

    public ColorSelectView(Activity activity, OnColorSelectedListener l,
            int colorId) {
        super(activity);

        DisplayMetrics dm = new DisplayMetrics();
        activity.getWindowManager().getDefaultDisplay().getMetrics(dm);
        float w = dm.widthPixels;
        width = (float) (w * 0.8);
        height = (float) (w * 0.6);
        circle_radius = (float) (width * 0.1);
        circle_start_x = (float) ((width - circle_radius * 8) / 5f);
        circle_start_y = (float) ((height - circle_radius * 6) / 4f);
        mListener = l;

        mSelectColorId = colorId;
        mTouchId = -1;

        mPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mPaint.setStyle(Paint.Style.FILL);

        mSelectedPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mSelectedPaint.setStrokeWidth(2);
        mSelectedPaint.setStyle(Paint.Style.STROKE);

        mShadowPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mShadowPaint.setStyle(Paint.Style.FILL);
        mShadowPaint.setColor(0xFF000000);
        mShadowPaint.setAlpha(0x10);
    }

    @Override
    protected void onDraw(Canvas canvas) {

        for (int i = 0; i < mColors.length; i++) {
            int row = i / COLUMNS;
            int col = i % COLUMNS;
            if (i == 2) {
                mPaint.setColor(0xFFE5DCD7);
            } else
                mPaint.setColor(mColors[i]);
            float x = circle_start_x * (1 + col) + circle_radius
                    * (2 * col + 1);
            float y = circle_start_y * (1 + row) + circle_radius
                    * (2 * row + 1);

            canvas.drawCircle(x, y, circle_radius, mPaint);

            if (i == mSelectColorId) {
                canvas.drawCircle(x, y,
                        circle_radius + mSelectedPaint.getStrokeWidth() * 2,
                        mSelectedPaint);
            }

            if (i == mTouchId) {
                canvas.drawCircle(x, y, circle_radius, mShadowPaint);
            }
        }

    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        setMeasuredDimension((int) width, (int) height);
    }

    private int getTouchId(float x, float y) {
        int col = (int) (x / (circle_start_x + circle_radius * 2));
        int row = (int) (y / (circle_start_y + circle_radius * 2));
        float dx = circle_start_x * (1 + col) + circle_radius * (2 * col + 1)
                - x;
        float dy = circle_start_y * (1 + row) + circle_radius * (2 * row + 1)
                - y;
        if (dx * dx + dy * dy < circle_radius * circle_radius)
            return row * COLUMNS + col;
        return -1;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        float x = event.getX();
        float y = event.getY();
        final int touchId = getTouchId(x, y);
        switch (event.getAction()) {
        case MotionEvent.ACTION_DOWN:
            mTouchId = touchId;
            if (touchId == -1)
                break;
            invalidate();
            break;
        case MotionEvent.ACTION_MOVE:
            break;
        case MotionEvent.ACTION_UP:
            if (mTouchId != -1) {
                if (touchId == mTouchId) {
                    mSelectColorId = touchId;
                    new Handler().postDelayed(new Runnable() {

                        @Override
                        public void run() {
                            mListener
                                    .onColorSelected(touchId, mColors[touchId]);
                        }

                    }, OnColorSelectedListener.DELAY);

                }
                mTouchId = -1;
                invalidate();
            }
            break;
        }
        return true;
    }
}
