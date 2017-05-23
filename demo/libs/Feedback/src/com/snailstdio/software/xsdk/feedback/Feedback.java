package com.snailstdio.software.xsdk.feedback;

import java.util.ArrayList;

import android.app.ProgressDialog;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.snailstdio.software.xsdk.activity.BaseActivity;
import com.snailstdio.software.xsdk.view.PopMenu.OnPopMenuItemClickListener;
import com.snailstdio.software.xsdk.view.ToastMaster;

public class Feedback extends BaseActivity {
    private static final int MAX_INPUT_NUM = 300;
    private ProgressDialog progressDialog;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setView(R.layout.feedback);
        init();
    }

    private void init() {
        final EditText userid = (EditText) findViewById(R.id.userid);
        final EditText content = (EditText) findViewById(R.id.content);
        Button submit = (Button) findViewById(R.id.submit);
        Button ret = (Button) findViewById(R.id.ret);

        final TextView left_word_num = (TextView) findViewById(R.id.left_word_num);
        left_word_num.setText(MAX_INPUT_NUM + "");

        content.addTextChangedListener(new TextWatcher() {
            @Override
            public void onTextChanged(CharSequence s, int start, int before,
                    int count) {
            }

            @Override
            public void beforeTextChanged(CharSequence s, int start, int count,
                    int after) {
            }

            @Override
            public void afterTextChanged(Editable s) {
                int text = MAX_INPUT_NUM - s.length();
                left_word_num.setText(String.valueOf(text));
                if (text < 0)
                    left_word_num.setTextColor(Color.RED);
                else
                    left_word_num.setTextColor(Color.BLUE);
            }

        });

        submit.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View arg0) {

                String input = content.getText().toString();
                if (TextUtils.isEmpty(input)) {
                    Animation shake = AnimationUtils.loadAnimation(
                            Feedback.this, R.anim.shake_x);
                    content.startAnimation(shake);
                    ToastMaster.showToast(Feedback.this,
                            R.string.feedback_input);
                    return;
                } else if (input.length() > MAX_INPUT_NUM) {
                    Animation shake = AnimationUtils.loadAnimation(
                            Feedback.this, R.anim.shake_x);
                    content.startAnimation(shake);
                    ToastMaster.showToast(Feedback.this,
                            R.string.feedback_input_too_much);
                    return;
                }

                String myContent = input;
                String user = userid.getText().toString();

                progressDialog = ProgressDialog.show(Feedback.this, null,
                        getString(R.string.feedback_submiting), true, true);

                QueryAsyncTask asyncTask = new QueryAsyncTask(myContent, user);
                asyncTask.execute("");
            }

        });

        ret.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View arg0) {
                Feedback.this.finish();
            }

        });

    }

    private class QueryAsyncTask extends AsyncTask {
        private String myContent, user;

        public QueryAsyncTask(String myContent, String user) {
            this.myContent = myContent;
            this.user = user;
        }

        @Override
        protected void onPostExecute(Object result) {
            progressDialog.dismiss();

            String isSubmit = (String) result;

            if (isSubmit == null || isSubmit.equals("ERROR")) {
                ToastMaster.showToast(Feedback.this, R.string.network_disabled);
            } else {
                ToastMaster.showToast(Feedback.this, R.string.feedback_success);
                Feedback.this.finish();
            }

            super.onPostExecute(result);
        }

        @Override
        protected Object doInBackground(Object... params) {
            return MailSender.submitFeedback(Feedback.this, myContent, user);
        }
    }

    @Override
    protected OnPopMenuItemClickListener initPopmenuData(
            ArrayList<String> popMenu_itemList,
            ArrayList<Integer> popMenu_itemListDrawable) {
        // TODO Auto-generated method stub
        return null;
    }
}