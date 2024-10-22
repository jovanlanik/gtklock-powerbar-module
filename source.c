// gtklock-powerbar-module
// Copyright (c) 2022 Jovan Lanik

// Power module

#include "gtklock-module.h"

#define MODULE_DATA(x) (x->module_data[self_id])
#define POWERBAR(x) ((struct powerbar *)MODULE_DATA(x))

extern void config_load(const char *path, const char *group, GOptionEntry entries[]);

struct powerbar {
	GtkWidget *revealer;
	GtkWidget *box;
	GtkWidget *reboot_button;
	GtkWidget *poweroff_button;
	GtkWidget *suspend_button;
	GtkWidget *userswitch_button;
	GtkWidget *logout_button;
};

const gchar module_name[] = "powerbar";
const guint module_major_version = 4;
const guint module_minor_version = 0;

static int self_id;

static gboolean show_labels = FALSE;
static gboolean linked_buttons = FALSE;
static gchar *reboot_command = "systemctl reboot";
static gchar *poweroff_command = "systemctl -i poweroff";
static gchar *suspend_command = "systemctl suspend";
static gchar *userswitch_command = NULL;
static gchar *logout_command = NULL;

GOptionEntry module_entries[] = {
	{ "show-labels", 0, 0, G_OPTION_ARG_NONE, &show_labels, NULL, NULL },
	{ "linked-buttons", 0, 0, G_OPTION_ARG_NONE, &linked_buttons, NULL, NULL },
	{ "reboot-command", 0, 0, G_OPTION_ARG_STRING, &reboot_command, NULL, NULL },
	{ "poweroff-command", 0, 0, G_OPTION_ARG_STRING, &poweroff_command, NULL, NULL },
	{ "suspend-command", 0, 0, G_OPTION_ARG_STRING, &suspend_command, NULL, NULL },
	{ "userswitch-command", 0, 0, G_OPTION_ARG_STRING, &userswitch_command, NULL, NULL },
	{ "logout-command", 0, 0, G_OPTION_ARG_STRING, &logout_command, NULL, NULL },
	{ NULL },
};

static void button_clicked(GtkButton *self, gpointer user_data) {
	g_spawn_command_line_async(user_data, NULL);
}

static void setup_powerbar(struct Window *ctx) {
	if(MODULE_DATA(ctx) != NULL) {
		gtk_widget_destroy(POWERBAR(ctx)->revealer);
		g_free(MODULE_DATA(ctx));
		MODULE_DATA(ctx) = NULL;
	}
	MODULE_DATA(ctx) = g_malloc(sizeof(struct powerbar));

	POWERBAR(ctx)->revealer = gtk_revealer_new();
	g_object_set(POWERBAR(ctx)->revealer, "margin", 5, NULL);
	gtk_widget_set_halign(POWERBAR(ctx)->revealer, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(POWERBAR(ctx)->revealer, GTK_ALIGN_END);
	gtk_widget_set_name(POWERBAR(ctx)->revealer, "powerbar-revealer");
	gtk_revealer_set_reveal_child(GTK_REVEALER(POWERBAR(ctx)->revealer), TRUE);
	gtk_revealer_set_transition_type(GTK_REVEALER(POWERBAR(ctx)->revealer), GTK_REVEALER_TRANSITION_TYPE_NONE);
	gtk_overlay_add_overlay(GTK_OVERLAY(ctx->overlay), POWERBAR(ctx)->revealer);

	if(linked_buttons) {
		POWERBAR(ctx)->box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
		gtk_button_box_set_layout(GTK_BUTTON_BOX(POWERBAR(ctx)->box), GTK_BUTTONBOX_EXPAND);
	} else POWERBAR(ctx)->box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_widget_set_halign(POWERBAR(ctx)->box, GTK_ALIGN_CENTER);
	gtk_widget_set_name(POWERBAR(ctx)->box, "powerbar-box");
	gtk_container_add(GTK_CONTAINER(POWERBAR(ctx)->revealer), POWERBAR(ctx)->box);

	if(poweroff_command && poweroff_command[0] != '\0') {
		POWERBAR(ctx)->poweroff_button = gtk_button_new_from_icon_name("system-shutdown-symbolic", GTK_ICON_SIZE_BUTTON);
		gtk_widget_set_name(POWERBAR(ctx)->poweroff_button, "poweroff-button");
		//gtk_widget_set_tooltip_text(POWERBAR(ctx)->poweroff_button, "Poweroff");
		gtk_button_set_always_show_image(GTK_BUTTON(POWERBAR(ctx)->poweroff_button), TRUE);
		if(show_labels) gtk_button_set_label(GTK_BUTTON(POWERBAR(ctx)->poweroff_button), "Poweroff");
		g_signal_connect(POWERBAR(ctx)->poweroff_button, "clicked", G_CALLBACK(button_clicked), poweroff_command);
		gtk_container_add(GTK_CONTAINER(POWERBAR(ctx)->box), POWERBAR(ctx)->poweroff_button);
	}

	if(reboot_command && reboot_command[0] != '\0') {
		POWERBAR(ctx)->reboot_button = gtk_button_new_from_icon_name("system-reboot-symbolic", GTK_ICON_SIZE_BUTTON);
		gtk_widget_set_name(POWERBAR(ctx)->reboot_button, "reboot-button");
		//gtk_widget_set_tooltip_text(POWERBAR(ctx)->reboot_button, "Reboot");
		gtk_button_set_always_show_image(GTK_BUTTON(POWERBAR(ctx)->reboot_button), TRUE);
		if(show_labels) gtk_button_set_label(GTK_BUTTON(POWERBAR(ctx)->reboot_button), "Reboot");
		g_signal_connect(POWERBAR(ctx)->reboot_button, "clicked", G_CALLBACK(button_clicked), reboot_command);
		gtk_container_add(GTK_CONTAINER(POWERBAR(ctx)->box), POWERBAR(ctx)->reboot_button);
	}

	if(suspend_command && suspend_command[0] != '\0') {
		POWERBAR(ctx)->suspend_button = gtk_button_new_from_icon_name("gtklock-powerbar-moon-outline-symbolic", GTK_ICON_SIZE_BUTTON);
		gtk_widget_set_name(POWERBAR(ctx)->suspend_button, "suspend-button");
		//gtk_widget_set_tooltip_text(POWERBAR(ctx)->suspend_button, "Suspend");
		gtk_button_set_always_show_image(GTK_BUTTON(POWERBAR(ctx)->suspend_button), TRUE);
		if(show_labels) gtk_button_set_label(GTK_BUTTON(POWERBAR(ctx)->suspend_button), "Suspend");
		g_signal_connect(POWERBAR(ctx)->suspend_button, "clicked", G_CALLBACK(button_clicked), suspend_command);
		gtk_container_add(GTK_CONTAINER(POWERBAR(ctx)->box), POWERBAR(ctx)->suspend_button);
	}

	if(userswitch_command && userswitch_command[0] != '\0') {
		// No suspend icon in GTK...
		POWERBAR(ctx)->userswitch_button = gtk_button_new_from_icon_name("system-users-symbolic", GTK_ICON_SIZE_BUTTON);
		gtk_widget_set_name(POWERBAR(ctx)->userswitch_button, "userswitch-button");
		//gtk_widget_set_tooltip_text(POWERBAR(ctx)->userswitch_button, "Switch user");
		gtk_button_set_always_show_image(GTK_BUTTON(POWERBAR(ctx)->userswitch_button), TRUE);
		if(show_labels) gtk_button_set_label(GTK_BUTTON(POWERBAR(ctx)->userswitch_button), "Switch user");
		g_signal_connect(POWERBAR(ctx)->userswitch_button, "clicked", G_CALLBACK(button_clicked), userswitch_command);
		gtk_container_add(GTK_CONTAINER(POWERBAR(ctx)->box), POWERBAR(ctx)->userswitch_button);
	}

	if(logout_command && logout_command[0] != '\0') {
		POWERBAR(ctx)->logout_button = gtk_button_new_from_icon_name("system-log-out-symbolic", GTK_ICON_SIZE_BUTTON);
		gtk_widget_set_name(POWERBAR(ctx)->logout_button, "logout-button");
		//gtk_widget_set_tooltip_text(POWERBAR(ctx)->logout_button, "Log out");
		gtk_button_set_always_show_image(GTK_BUTTON(POWERBAR(ctx)->logout_button), TRUE);
		if(show_labels) gtk_button_set_label(GTK_BUTTON(POWERBAR(ctx)->logout_button), "Log out");
		g_signal_connect(POWERBAR(ctx)->logout_button, "clicked", G_CALLBACK(button_clicked), logout_command);
		gtk_container_add(GTK_CONTAINER(POWERBAR(ctx)->box), POWERBAR(ctx)->logout_button);
	}

	gtk_widget_show_all(POWERBAR(ctx)->revealer);
}

void on_activation(struct GtkLock *gtklock, int id) {
	self_id = id;

	GtkIconTheme *theme = gtk_icon_theme_get_default();
	gtk_icon_theme_add_resource_path(theme, "/gtklock/powerbar/icons");
}

void on_focus_change(struct GtkLock *gtklock, struct Window *win, struct Window *old) {
	setup_powerbar(win);
	if(gtklock->hidden)
		gtk_revealer_set_reveal_child(GTK_REVEALER(POWERBAR(win)->revealer), FALSE);
	if(old != NULL && win != old)
		gtk_revealer_set_reveal_child(GTK_REVEALER(POWERBAR(old)->revealer), FALSE);
}

void on_window_destroy(struct GtkLock *gtklock, struct Window *ctx) {
	if(MODULE_DATA(ctx) != NULL) {
		gtk_widget_destroy(POWERBAR(ctx)->revealer);
		g_free(MODULE_DATA(ctx));
		MODULE_DATA(ctx) = NULL;
	}
}

void on_idle_hide(struct GtkLock *gtklock) {
	if(gtklock->focused_window) {
		GtkRevealer *revealer = GTK_REVEALER(POWERBAR(gtklock->focused_window)->revealer);	
		gtk_revealer_set_reveal_child(revealer, FALSE);
	}
}

void on_idle_show(struct GtkLock *gtklock) {
	if(gtklock->focused_window) {
		GtkRevealer *revealer = GTK_REVEALER(POWERBAR(gtklock->focused_window)->revealer);	
		gtk_revealer_set_reveal_child(revealer, TRUE);
	}
}

