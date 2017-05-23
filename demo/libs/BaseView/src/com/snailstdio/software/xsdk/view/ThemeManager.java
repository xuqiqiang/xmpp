package com.snailstdio.software.xsdk.view;

import android.content.Context;

import com.snailstdio.software.xsdk.baseview.R;
import com.snailstdio.software.xsdk.utils.Cache;

public class ThemeManager {

    private Context context;

    protected static ThemeManager instance;

    protected int theme;

    public static final String THEME = "theme";
    public static final int THEME_BLUE = 0, THEME_GREY_BLUE = 1,
            THEME_BROWN = 2, THEME_ORANGE = 3, THEME_RED = 4, THEME_CYAN = 5,
            THEME_GREEN = 6, THEME_PURPLE = 7;

    public int background_color;
    public int systembar_color;
    public int button_color;
    public int button_pressed_color;
    public int setting_menu_pressed_color;
    public int navigation_drawer_activated_background_color;
    public int popmenu_background;
    public int popmenu_item_bg;
    public int handwriting_menu_background;
    public int switch_background;
    public int toast_background;

    protected ThemeManager(Context context) {
        this.context = context;
        init();
    }

    public static void createInstance(Context context) {
        if (instance == null)
            instance = new ThemeManager(context);
    }

    public static ThemeManager getInstance() {
        return instance;
    }

    protected void init() {
        theme = Cache.read_int(THEME, THEME_GREY_BLUE);
        if (theme == THEME_BLUE) {
            background_color = R.color.theme_blue;
            systembar_color = R.color.systembar_blue;
            button_color = R.color.theme_blue;
            button_pressed_color = R.color.theme_pressed_blue;
            setting_menu_pressed_color = R.color.setting_menu_pressed_color_blue;
            navigation_drawer_activated_background_color = R.color.navigation_drawer_activated_background_color_blue;
            popmenu_background = R.drawable.menu_bg_blue;
            popmenu_item_bg = R.drawable.popmenu_item_bg;
            handwriting_menu_background = R.color.theme_blue;
            switch_background = R.drawable.checkbox_blue;
            toast_background = R.drawable.toast_bg_blue;
        } else if (theme == THEME_GREY_BLUE) {
            background_color = R.color.theme_grey_blue;
            systembar_color = R.color.systembar_grey_blue;
            button_color = R.color.theme_grey_blue;
            button_pressed_color = R.color.theme_pressed_grey_blue;
            setting_menu_pressed_color = R.color.setting_menu_pressed_color_grey_blue;
            navigation_drawer_activated_background_color = R.color.navigation_drawer_activated_background_color_grey_blue;
            popmenu_background = R.color.theme_grey_blue;
            popmenu_item_bg = R.drawable.find_button_bg;
            handwriting_menu_background = R.color.theme_grey_blue;
            switch_background = R.drawable.checkbox_grey_blue;
            toast_background = R.drawable.toast_bg_grey_blue;
        } else if (theme == THEME_BROWN) {
            background_color = R.color.theme_brown;
            systembar_color = R.color.systembar_brown;
            button_color = R.color.theme_brown;
            button_pressed_color = R.color.theme_pressed_brown;
            setting_menu_pressed_color = R.color.setting_menu_pressed_color_brown;
            navigation_drawer_activated_background_color = R.color.navigation_drawer_activated_background_color_brown;
            popmenu_background = R.color.theme_brown;
            popmenu_item_bg = R.drawable.find_button_bg;
            handwriting_menu_background = R.color.theme_brown;
            switch_background = R.drawable.checkbox_brown;
            toast_background = R.drawable.toast_bg_brown;
        } else if (theme == THEME_ORANGE) {
            background_color = R.color.theme_orange;
            systembar_color = R.color.systembar_orange;
            button_color = R.color.theme_orange;
            button_pressed_color = R.color.theme_pressed_orange;
            setting_menu_pressed_color = R.color.setting_menu_pressed_color_orange;
            navigation_drawer_activated_background_color = R.color.navigation_drawer_activated_background_color_orange;
            popmenu_background = R.color.theme_orange;
            popmenu_item_bg = R.drawable.find_button_bg;
            handwriting_menu_background = R.color.theme_orange;
            switch_background = R.drawable.checkbox_orange;
            toast_background = R.drawable.toast_bg_orange;
        } else if (theme == THEME_RED) {
            background_color = R.color.theme_red;
            systembar_color = R.color.systembar_red;
            button_color = R.color.theme_red;
            button_pressed_color = R.color.theme_pressed_red;
            setting_menu_pressed_color = R.color.setting_menu_pressed_color_red;
            navigation_drawer_activated_background_color = R.color.navigation_drawer_activated_background_color_red;
            popmenu_background = R.color.theme_red;
            popmenu_item_bg = R.drawable.find_button_bg;
            handwriting_menu_background = R.color.theme_red;
            switch_background = R.drawable.checkbox_red;
            toast_background = R.drawable.toast_bg_red;
        } else if (theme == THEME_CYAN) {
            background_color = R.color.theme_cyan;
            systembar_color = R.color.systembar_cyan;
            button_color = R.color.theme_cyan;
            button_pressed_color = R.color.theme_pressed_cyan;
            setting_menu_pressed_color = R.color.setting_menu_pressed_color_cyan;
            navigation_drawer_activated_background_color = R.color.navigation_drawer_activated_background_color_cyan;
            popmenu_background = R.color.theme_cyan;
            popmenu_item_bg = R.drawable.find_button_bg;
            handwriting_menu_background = R.color.theme_cyan;
            switch_background = R.drawable.checkbox_cyan;
            toast_background = R.drawable.toast_bg_cyan;
        } else if (theme == THEME_GREEN) {
            background_color = R.color.theme_green;
            systembar_color = R.color.systembar_green;
            button_color = R.color.theme_green;
            button_pressed_color = R.color.theme_pressed_green;
            setting_menu_pressed_color = R.color.setting_menu_pressed_color_green;
            navigation_drawer_activated_background_color = R.color.navigation_drawer_activated_background_color_green;
            popmenu_background = R.color.theme_green;
            popmenu_item_bg = R.drawable.find_button_bg;
            handwriting_menu_background = R.color.theme_green;
            switch_background = R.drawable.checkbox_green;
            toast_background = R.drawable.toast_bg_green;
        } else if (theme == THEME_PURPLE) {
            background_color = R.color.theme_purple;
            systembar_color = R.color.systembar_purple;
            button_color = R.color.theme_purple;
            button_pressed_color = R.color.theme_pressed_purple;
            setting_menu_pressed_color = R.color.setting_menu_pressed_color_purple;
            navigation_drawer_activated_background_color = R.color.navigation_drawer_activated_background_color_purple;
            popmenu_background = R.color.theme_purple;
            popmenu_item_bg = R.drawable.find_button_bg;
            handwriting_menu_background = R.color.theme_purple;
            switch_background = R.drawable.checkbox_purple;
            toast_background = R.drawable.toast_bg_purple;
        }
    }

    public boolean isDefaultTheme() {
        return theme == THEME_GREY_BLUE;
    }

    public void reset() {
        init();
    }
}